#ifndef NETWORK_H
#define NETWORK_H
#include <stdbool.h>
#include <stdio.h>
#include <netinet/in.h>

typedef struct NetWork
{
	int type;					//	通信协议TCP/UDP
	int sock_fd;				//	socket描述符
	struct sockaddr_in addr;	//	通信地址
	socklen_t addrlen;			//	地址结构体字节数
	bool is_svr;				//	判断客户端还是服务器
}NetWork;

typedef struct sockaddr* SP;

//	创建NetWork，创建socket、初始化通信地址、绑定、监听、连接
NetWork* init_nw(int type,short port,const char* ip,bool is_svr);

//	TCP服务器的等待连接
NetWork* accept_nw(NetWork* svr_nw);

//	发送数据
int send_nw(NetWork* nw,const void* buf,size_t len);

//	接收数据
int recv_nw(NetWork* nw,void* buf,size_t len);

//	关闭socket，释放NetWork
void close_nw(NetWork* nw);

#endif//NETWORK_H
