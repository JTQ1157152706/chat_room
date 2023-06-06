#include <stdlib.h>
#include "threadpool.h"

//	线程池的消费者线程的入口函数
static void* run(void* arg)
{
	ThreadPool* thread = arg;
	for(;;)
	{
		//	负责消费数据 能返回意味着成功拿到了数据
		void* data = pop_threadpool(arg);
		//	拿到数据后，执行真正的消费者线程的业务逻辑函数
		thread->workfp(data);
	}
}

//	创建线程池
ThreadPool* create_threadpool(int thread_cnt,int queue_cal,WorkFP workfp)
{
	//	申请线程池内存
	ThreadPool* thread = malloc(sizeof(ThreadPool));
	//	申请线程ID数组内存
	thread->tids = malloc(sizeof(pthread_t)*thread_cnt);
	//	创建仓库队列
	thread->store = create_queue(queue_cal);
	
	//	初始化成员
	thread->thread_cnt = thread_cnt;
	thread->workfp = workfp;
	
	pthread_mutex_init(&thread->flock,NULL);
	pthread_mutex_init(&thread->rlock,NULL);
	pthread_cond_init(&thread->full,NULL);
	pthread_cond_init(&thread->empty,NULL);

	return thread;
}

//	启动线程池
void start_threadpool(ThreadPool* threadpool)
{
	for(int i=0; i<threadpool->thread_cnt; i++)
	{
		pthread_create(&threadpool->tids[i],NULL,run,threadpool);	
	}
}


//	生产数据
void push_threadpool(ThreadPool* threadpool,void* data)
{
	//	队尾加锁
	pthread_mutex_lock(&threadpool->rlock);

	//	如果一直队满，则睡入条件变量，不生产
	while(full_queue(threadpool->store))
	{
		//	唤醒消费者线程
		pthread_cond_signal(&threadpool->empty);
		//	睡入条件变量并解锁队尾
		pthread_cond_wait(&threadpool->full,&threadpool->rlock);
	}

	//	存数据到仓库中
	push_queue(threadpool->store,data);
	//	唤醒消费者线程
	pthread_cond_signal(&threadpool->empty);
	//	解锁队尾
	pthread_mutex_unlock(&threadpool->rlock);
}


//	消费数据
void* pop_threadpool(ThreadPool* threadpool)
{
	//	队头加锁
	pthread_mutex_lock(&threadpool->flock);

	//	如果一直空，不消费
	while(empty_queue(threadpool->store))
	{
		//	唤醒生产者
		pthread_cond_signal(&threadpool->full);
		//	睡入空条件变量并解锁
		pthread_cond_wait(&threadpool->empty,&threadpool->flock);
	}

	//	消费数据
	void* data = front_queue(threadpool->store);
	pop_queue(threadpool->store);
	//	唤醒生产者
	pthread_cond_signal(&threadpool->full);
	//	解锁队头
	pthread_mutex_unlock(&threadpool->flock);
	return data;
}


//	销毁线程池
void destroy_threadpool(ThreadPool* threadpool)
{
	for(int i=0; i<threadpool->thread_cnt; i++)
	{
		pthread_cancel(threadpool->tids[i]);
	}
	destory_queue(threadpool->store);
	free(threadpool->tids);
	pthread_mutex_destroy(&threadpool->flock);
	pthread_mutex_destroy(&threadpool->rlock);
	pthread_cond_destroy(&threadpool->empty);
	pthread_cond_destroy(&threadpool->full);
	free(threadpool);
}



