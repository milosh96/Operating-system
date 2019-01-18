/*
 * KernelSem.h
 *
 *  Created on: May 30, 2017
 *      Author: OS1
 */

#ifndef KERNELS_H_
#define KERNELS_H_

#include "Queue.h"
#include "SQueue.h"
class SQueue;
/*
 * Kernel obj?
 */
class KernelSem{
public:
	friend class Semaphore;
	KernelSem(int init=1);
	~KernelSem();
	void signal();
	int wait(Time t);
	int val() const;
//private:
	int value;
	Queue *waitQueue;			//pomocu signal, ali moramo i iz II da izvadimo
	static Queue *noNullQueue;	//nakon vremena vadimo
	static SQueue *sQueue;

	friend class Thread;
	friend class Queue;
};


#endif /* KERNELS_H_ */
