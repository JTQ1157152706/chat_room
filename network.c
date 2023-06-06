#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "network.h"

//	创建NetWork，创建socket、初始化通信地址、绑定、监听、连接
NetWork* init_nw(int type,short port,const char* ip,bool is_svr)
{
	NetWork* nw = malloc(sizeof(NetWork));
	nw->type = type;
	nw->sock_fd = socket(AF_INET,type,0);
	if(0 > nw->sock_fd)
	{
		perror("init_nw:socket");
		free(nw);
		return NULL;
	}

	//	初始化通信地址
	nw->addr.sin_family = AF_INET;
	nw->addr.sin_port = htons(port);
	nw->addr.sin_addr.s_addr = inet_addr(ip);
	nw->addrlen = sizeof(nw->addr);

	nw->is_svr = is_svr;

	//	是服务端
	if(is_svr)
	{
		if(bind(nw->sock_fd,(SP)&nw->addr,nw->addrlen))
		{
			perror("init_nw:bind");
			free(nw);
			return NULL;
		}
		
		//	是TCP
		if(SOCK_STREAM == nw->type)
		{
			if(listen(nw->sock_fd,10))
			{
				perror("init_nw:listen");
				free(nw);
				return NULL;
			}
		}
	}
	else if(SOCK_STREAM == nw->type)
	{
	//	是TCP客户端
		if(connect(nw->sock_fd,(SP)&nw->addr,nw->addrlen))
		{	
			perror("init_nw:connect");
			free(nw);
			return NULL;		
		}
	}
	return nw;
}

//	TCP服务器的等待连接
NetWork* accept_nw(NetWork* svr_nw)
{
	if(SOCK_STREAM != svr_nw->type || false == svr_nw->is_svr)
	{
		printf("type use:SOCK_STREAM  is_svr use:true\n");
		return NULL;
	}

	//	申请连接者的NetWork
	NetWork* cli_nw = malloc(sizeof(NetWork));
	cli_nw->type = svr_nw->type;
	cli_nw->is_svr = true;
	cli_nw->sock_fd = accept(svr_nw->sock_fd,(SP)&cli_nw->addr,&cli_nw->addrlen);
	if(0 > cli_nw->sock_fd)
	{
		perror("accept_nw:accept");
		free(cli_nw);
		return NULL;
	}
	return cli_nw;
}

//	发送数据 send/sendto
int send_nw(NetWork* nw,const void* buf,size_t len)
{
	int ret = 0;
	if(SOCK_STREAM == nw->type)
	{
		ret = send(nw->sock_fd,buf,len,0);
		if(0 > ret)
		{
			perror("send_nw:send");
		}
	}
	else
	{
		ret = sendto(nw->sock_fd,buf,len,0,(SP)&nw->addr,nw->addrlen);
		if(0 > ret)
		{
			perror("send_nw:sendto");	
		}
	}
	return ret;
}

//	接收数据
int recv_nw(NetWork* nw,void* buf,size_t len)
{
	int ret = 0;
	if(SOCK_STREAM == nw->type)
	{
		ret = recv(nw->sock_fd,buf,len,0);
		if(0 > ret)
		{
			perror("recv_nw:recv");
		}
	}
	else
	{
		ret = recvfrom(nw->sock_fd,buf,len,0,(SP)&nw->addr,&nw->addrlen);
		if(0 > ret)
		{
			perror("recv_nw:recv");	
		}
	}
	return ret;
	
}

//	关闭socket，释放NetWork
void close_nw(NetWork* nw)
{
	close(nw->sock_fd);
	free(nw);
}
