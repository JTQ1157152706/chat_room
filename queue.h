#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct Queue
{
	void** arr; 	//	内存首地址
	size_t cal;		//	容量
	size_t front;	//	队头位置
	size_t rear;	//	队尾位置
}Queue;

//	创建
Queue* create_queue(size_t cal);
//	销毁
void destory_queue(Queue* queue);
//	队空
bool empty_queue(Queue* queue);
//	队满
bool full_queue(Queue* queue);
//	入队
void push_queue(Queue* queue,void* data);
//	出队
void pop_queue(Queue* queue);
//	队头
void* front_queue(Queue* queue);
//	队尾
void* rear_queue(Queue* queue);

#endif//QUEUE_H



