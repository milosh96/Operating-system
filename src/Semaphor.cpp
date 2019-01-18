/*
 * Semaphore.cpp
 *
 *  Created on: May 30, 2017
 *      Author: OS1
 */

#include "Semaphor.h"

#include <stdlib.h>
#include "lock.h"
#include "KernelS.h"

class KernelSem;

Semaphore::Semaphore(int init){
	_lock
	myImpl = new KernelSem(init);				//neophodno lock?
	_unlock
}

Semaphore::~Semaphore(){
	_lock
	delete myImpl;
	_unlock
}

int Semaphore::wait(Time time){
	_lock
	int rez = myImpl->wait(time)
;	_unlock
	return rez;
}

void Semaphore::signal(){
	_lock
	myImpl->signal();
	_unlock
}

int Semaphore::val() const{
	return myImpl->val();
}

