/*
 * Kernel.h
 *
 *  Created on: May 20, 2017
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "PCB.h"
#include "Thread.h"
#include <stdio.h>
#include "Idle.h"
typedef void interrupt (*interruptfunction)(...);

class IdleThread;
class Kernel{

private:
	static volatile int explicit_dispatch;

	static interruptfunction oldTimer;

public:
	Kernel();
	~Kernel();
	static void interrupt timerIR(...);
	static void initialization();
	static void restore();
	static void dispatch();
	static void wrapper();

	static volatile PCB *running;
	static Thread *starting;
	static IdleThread *idle;
	friend class Thread;
	friend class PCB;

};


#endif /* KERNEL_H_ */
