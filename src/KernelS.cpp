/*
 * KernelSem.cpp
 *
 *  Created on: May 30, 2017
 *      Author: OS1
 */


#include "KernelS.h"
#include "Queue.h"
#include "Kernel.h"
//imamo lock u klasi Semafore!
#include "SCHEDULE.h"
#include "Queue.h"
#include "SQueue.h"
#include "PCB.h"
#include "lock.h"


Queue *KernelSem::noNullQueue=new Queue();
SQueue *KernelSem::sQueue=new SQueue();
class Queue;
KernelSem::KernelSem(int init){
	value=init;
	waitQueue=new Queue();
	sQueue->put(this);
};

KernelSem::~KernelSem(){
	_lock
	//ubacivanje preostalih u Scheduler
	if(waitQueue!=0){
		Elem *curr=waitQueue->first;
		while(curr!=0){
			PCB *p=waitQueue->get();				//a sta je sa brisanjem iz nonullQueue
			p->state=READY;
			if(p!=Kernel::idle->getPCB())
				Scheduler::put(p);
			 if (p->timeWait>0)
			 	noNullQueue->deleteFromQueue(p);

			curr=waitQueue->first;
		}


	}
	delete waitQueue;
	if (!sQueue->isEmpty())
		sQueue->remove(this);
	_unlock
};
void KernelSem::signal(){
	_lock
	value++;
	if(value<=0){			//vadimo 1 iz waitQueue-a, a ako je zbog vremean moze vise
		PCB* pcb=waitQueue->get();
		pcb->state=READY;
		pcb->flag=1;

		if(pcb->timeWait>0)
			noNullQueue->deleteFromQueue(pcb);

		if(pcb != Kernel::idle->getPCB()) {
			Scheduler::put(pcb);
		}
	}
	_unlock



};
int KernelSem::wait(Time waitTime){
	_lock
	value--;
	if(value<0){

		if (waitTime>0){
			Kernel::running->timeWait=waitTime;
			noNullQueue->put((PCB*)Kernel::running);
		}

		Kernel::running->state=BLOCKED;
		if(waitQueue!=0)
			waitQueue->put((PCB*)Kernel::running);		//mozda put Sorted

		_unlock
		dispatch();

		_lock
		if(Kernel::running->flag==1){
			Kernel::running->flag=0;
			_unlock
			return 1;
		}
		_unlock
		return 0;
	}
	_unlock
	return 1;
};

int KernelSem::val() const{
	return this->value;
};

