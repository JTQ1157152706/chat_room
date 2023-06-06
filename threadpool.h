#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include "queue.h"

//	消费者线程真正干活的业务逻辑函数格式
typedef void (*WorkFP)(void*);

typedef struct ThreadPool
{
	int thread_cnt;				//	线程数量
	pthread_t* tids;			//	线程ID数组
	Queue* store;				//	队列仓库
	WorkFP workfp;				//	业务逻辑函数
	pthread_mutex_t flock;		//	队头互斥量
	pthread_mutex_t rlock;		//	队尾互斥量
	pthread_cond_t empty;		//	空仓条件变量
	pthread_cond_t full;		//	满仓条件变量
}ThreadPool;

//	创建线程池
ThreadPool* create_threadpool(int thread_cnt,int queue_cal,WorkFP workfp);

//	启动线程池
void start_threadpool(ThreadPool* threadpool);

//	生产数据
void push_threadpool(ThreadPool* threadpool,void* data);

//	消费数据
void* pop_threadpool(ThreadPool* threadpool);

//	销毁线程池
void destroy_threadpool(ThreadPool* threadpool);


#endif//THREADPOOL_H
