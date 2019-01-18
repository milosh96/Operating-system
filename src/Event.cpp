/*
 * event.cpp
 *
 *  Created on: Jun 8, 2017
 *      Author: OS1
 */

#include "Event.h"
#include "KernelE.h"
#include "lock.h"

class KernelEv;

Event::Event(IVTNo n){
	_lock
	myImpl = new KernelEv(n);
	_unlock
}

Event::~Event(){
	_lock
	delete myImpl;
	_unlock
}

void Event::wait(){
	_lock
	myImpl->wait();
	_unlock
}

void Event::signal(){
	_lock
	myImpl->signal();
	_unlock
}




