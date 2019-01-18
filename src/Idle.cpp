/*
 * Idle.cpp
 *
 *  Created on: Jun 2, 2017
 *      Author: OS1
 */


#include "Idle.h"
#include "PCB.h"
#include "lock.h"


void IdleThread::run() {
	while(1);
};

IdleThread::IdleThread():Thread(256,PCB::minTimeSlice){

};

void IdleThread::start(){
		_lock;
		myPCB->state=READY;
		myPCB->createStack();			//ne stavlja se u SCHEDULER!!!
		_unlock;
	}

