/*
 * Semaphore.h
 *
 *  Created on: May 30, 2017
 *      Author: OS1
 */

#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_


typedef unsigned int Time;
class KernelSem;


class Semaphore{

public:
	int val() const;
	virtual	int wait(Time t=0);
	virtual void signal();
	Semaphore(int init=1);
	virtual ~Semaphore();

private:
	KernelSem *myImpl;
};


#endif /* SEMAPHOR_H_ */
