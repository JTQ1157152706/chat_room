#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "network.h"

void* run(void* arg)
{
	char buf[4096] = {};
	NetWork* nw = arg;
	for(;;)
	{
		int ret = recv_nw(nw,buf,sizeof(buf));
		if(0 >= ret)
		{
			printf("服务器忙碌中,稍后再试\n");	
			close_nw(nw);
			exit(EXIT_FAILURE);
		}
		printf("\r%s\n>>>",buf);
		fflush(stdout);
	}
}

int main(int argc,const char* argv[])
{
	if(3 != argc)
	{
		printf("User: ./client <ip> <port>\n");
		return EXIT_SUCCESS;
	}

	NetWork* nw = init_nw(SOCK_STREAM,atoi(argv[2]),argv[1],false);
	if(NULL == nw)
	{
		printf("请检查参数\n");
		return EXIT_SUCCESS;
	}
	
	printf("登录成功，请输入昵称:");
	char buf[4096] = {};
	scanf("%s",buf);
	send_nw(nw,buf,strlen(buf)+1);

	pthread_t tid;
	pthread_create(&tid,NULL,run,nw);
	
	for(;;)
	{
		printf(">>>");
		scanf("%s",buf);
		int ret = send_nw(nw,buf,strlen(buf)+1);
		if(0 == strncmp(buf,"quit",4))
		{
			printf("退出聊天室\n");
			close_nw(nw);
			return EXIT_SUCCESS;
		}
		if(0 >= ret)
		{
			printf("服务器忙碌中,稍后再试\n");	
			close_nw(nw);
			return EXIT_FAILURE;
		}
	}
}
