/*
 * IdleThread.h
 *
 *  Created on: May 20, 2017
 *      Author: OS1
 */

#ifndef IDLE_H_
#define IDLE_H_

#include "Thread.h"
#include "lock.h"

class IdleThread:public Thread{

public:
	IdleThread();
	void run() ;

	void start();
};



#endif /* IDLE_H_ */
