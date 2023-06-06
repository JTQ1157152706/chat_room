#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "threadpool.h"

#define MAX_CLIENT (20)
#define BUF_SIZE (4096)

//	客户端对象数组
NetWork* clients[MAX_CLIENT];

//	往clients添加一个客户端对象
int add_client(NetWork* nw)
{
	for(int i=0; i<MAX_CLIENT; i++)
	{
		if(NULL == clients[i])
		{
			clients[i] = nw;
			return i;
		}
	}
	//	一定能找到空位置
}

//	转发消息给其他在线客户端
void send_other_clients(int index,char* buf)
{
	for(int i=0; i<MAX_CLIENT; i++)
	{
		if(NULL != clients[i] && i != index)
		{
			send_nw(clients[i],buf,strlen(buf)+1);	
		}
	}
}

//	消费线程的业务函数
void work(void* arg)
{
	//	开始服务新连接的客户端
	//	添加到客户端数组
	int index = add_client(arg);
	
	char* buf = malloc(BUF_SIZE);
	//	接收昵称
	int ret = recv_nw(clients[index],buf,BUF_SIZE);
	if(0 >= ret || 0 == strncmp(buf,"quit",4))
	{
		close_nw(clients[index]);
		clients[index] = NULL;
		free(buf);
		return;
	}
	strcat(buf," 进入聊天室,大家欢迎!");
	send_other_clients(index,buf);
	
	buf[ret-1] = ':';
	char* msg =  buf+ret;

	for(;;)
	{
		//	接收客户端消息并转发
		int msg_ret = recv_nw(clients[index],msg,BUF_SIZE-ret);
		if(0 >= msg_ret || 0 == strncmp(msg,"quit",4))
		{
			strcpy(msg,"退出聊天室!");
			send_other_clients(index,buf);
			close_nw(clients[index]);
			clients[index] = NULL;
			free(buf);
			return;
		}
		send_other_clients(index,buf);
	}
}

int main(int argc,const char* argv[])
{
	if(3 != argc)
	{
		printf("User: ./sever <ip> <port>\n");
		return EXIT_SUCCESS;
	}

	//	创建线程池
	ThreadPool* thread = create_threadpool(MAX_CLIENT,20,work);
	//	启动线程池
	start_threadpool(thread);

	//	初始化网络通信
	NetWork* svr_nw = init_nw(SOCK_STREAM,atoi(argv[2]),argv[1],true);
	if(NULL == svr_nw)
	{
		printf("网络错误,请检查\n");
		destroy_threadpool(thread);
		return EXIT_FAILURE;
	}

	for(;;)
	{
		//	等待客户端连接
		NetWork* cli_nw = accept_nw(svr_nw);
		if(NULL == cli_nw)
		{
			printf("网络有异常\n");
			destroy_threadpool(thread);
			close_nw(svr_nw);
			return EXIT_FAILURE;
		}
		
		//	添加到仓库
		push_threadpool(thread,cli_nw);
	}
}