/*
 * PCB.cpp
 *
 *  Created on: May 14, 2017
 *      Author: OS1
 */

#include "PCB.h"
#include "lock.h"
#include "Queue.h"
#include "QueueI.h"
#include <dos.h>
#include <stdlib.h>
#include "Kernel.h"
#include "Thread.h"

class Kernel;
int PCB::numID=0;
//Queue *PCB::sleepQueue=new Queue();					//niti koje su uspavane
Queue *PCB::identificationQueue=new Queue();		//sve niti koje su stvorene
Time PCB::minTimeSlice=1;

PCB::PCB(Thread* myT, StackSize stackSize, Time timeSlice){
	_lock

	this->threadID=++numID;
	this->thread=myT;
	this->stackSize=stackSize;
	this->timeSlice=timeSlice;
	this->state=NEW;
	this->waitQueue=new Queue();
	timeTaken=0;
	timeWait=0;
	flag=0;
	lockFlag=0;
	if (threadID!=2) PCB::identificationQueue->put(this);  //ubacujemo sve sem starting i idle niti
														  //moramo njih prvo da stvorimo
	_unlock
};

PCB::~PCB(){
	_lock
	delete stack;
	delete waitQueue;
	_unlock
};

void PCB::createStack(){
 	_lock
	int n = 0;
	n = this->stackSize;
	n = n / sizeof(unsigned);
	unsigned* St = new unsigned[n];

	this->stack = St;

	#ifndef BCC_BLOCK_IGNORE
	stack[n-1] = 0x200;
	stack[n-2] = FP_SEG(&(Kernel::wrapper));
	stack[n-3] = FP_OFF(&(Kernel::wrapper));

	this->ss = FP_SEG(St+n-12);
	this->sp = FP_OFF(St+n-12);
	this->bp = FP_OFF(St+n-12);
	#endif
	_unlock

};

StackSize PCB::getStackSize() const{
	return this->stackSize;
};

ID PCB::getId() const{
	return this->threadID;
};

void PCB::runSignals(){
	_lock
		while(!Kernel::running->thread->signals->empty()){
			ElemI *curr=Kernel::running->thread->signals->first;
			if(curr==0){
				_unlock
				return;
			}
			SignalId s=curr->signal;
			SignalHandler sh=Kernel::running->thread->getHandler(s);
				//treba provera da li je blocked, pa ako jeste, vracamo ga na I mesto?
			if(sh!=0){
				unsigned int maska=1;
				maska=maska<<s;
				if ((maska&Thread::blockedS)||(maska&Kernel::running->thread->blockedT)){
							//trazimo prvi koji nije blokiran ili maskiran
					ElemI *prev=0;
					while(curr!=0){
						prev=curr;
						curr=curr->next;
						//curr!=0
						if(curr==0){
							_unlock
							return;
						}
						maska=1;
						maska=maska<<curr->signal;

						if ((maska&Thread::blockedS)||(maska&Kernel::running->thread->blockedT)){
							continue;		//samo ga preskacemo u nizu
						}
						prev->next=curr->next;
						s=curr->signal;
						sh=Kernel::running->thread->getHandler(s);
						delete curr;
						break;
					}
				}
				else s=Kernel::running->thread->signals->get();
				if(sh==0) continue;
				Kernel::running->lockFlag=1;

					asm{
						pushf
						sti
					}
					(*sh)();
					asm popf
					Kernel::running->lockFlag=0;
			}
			else s=Kernel::running->thread->signals->get();
		 }

		_unlock
};


