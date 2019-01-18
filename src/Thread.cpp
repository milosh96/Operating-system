/*
 * Thread.cpp
 *
 *  Created on: May 18, 2017
 *      Author: OS1
 */

#include "Thread.h"
#include "lock.h"
#include "SCHEDULE.h"
#include "Kernel.h"
#include "PCB.h"
unsigned int Thread::maskedS=0;
unsigned int Thread::blockedS=0;
#include "Scheduler.h"
#include "stdio.h"
void Thread::start(){
	_lock
	myPCB->state=READY;
	myPCB->createStack();
	Scheduler::put(this->myPCB);
	_unlock
};

Thread::Thread(StackSize stackSize , Time timeSlice ){
	_lock 				//new mora da se zakljuca, moze da nastane problem
	myPCB=new PCB(this, stackSize,timeSlice);
	paused=0;
	if(myPCB->threadID>2){
			maskedT=Kernel::running->thread->maskedT;
			blockedT=Kernel::running->thread->blockedT;
			handlers=new SignalHandler[16];
			for(int i=0;i<16;i++)
				handlers[i]=Kernel::running->thread->handlers[i];
			//da li i signals da kopiramo, to nisu podesavanja
			signals=new QueueI();
			parent=(PCB *) Kernel::running;
	}
	else{
		parent=0;
		maskedT=0;
		blockedT=0;
		handlers=new SignalHandler[16];
		for (int i=0;i<16;i++)
			handlers[i]=0;
		handlers[0]=handle0;
		signals=new QueueI();
	}
	_unlock
};
void Thread::handle0(){
	//suspenduje nit

	PCB::identificationQueue->deleteFromQueue((PCB*)Kernel::running);
	Kernel::running->state=FINISHED;

	PCB *temp;
	while(!Kernel::running->waitQueue->empty()){
			temp=Kernel::running->waitQueue->get();
			if(temp!=0){
				temp->state=READY;		//a ubacivanje u scheduler?
				Scheduler::put(temp);
			}
	}
	Kernel::running->thread->signal(2);
	if(Kernel::running->thread->parent!=0) Kernel::running->thread->parent->thread->signal(1);

	Kernel::running->lockFlag=0;
	Kernel::dispatch();

};
Thread::~Thread(){
	//prepraviti za II zadatak
	_lock
	//this->waitToComplete();		//ne mozemo da unistimo objekat ako se this nit nije zavrsila
	delete myPCB;
	delete signals;
	delete handlers;
	_unlock
};
ID Thread::getId() const{
	return myPCB->getId();
};
 ID Thread::getRunningId(){
	return Kernel::running->getId();
};

Thread * Thread::getThreadById(ID id){
	return PCB::identificationQueue->findThreadwithID(id);   //sadrzi svaki Thread stvoren, osim starting i idle
};
void dispatch() {
	Kernel::dispatch();
};
PCB *Thread::getPCB(){
	return myPCB;
}
void Thread::waitToComplete(){
	_lock
	/*
	 * cekamo da se zavrsi nit nad this;
	 * pozivajuca nit/running se blokira dok this nit ne zavrsi
	 */

	//nit je zavrsila
	if(myPCB->state==FINISHED){
		_unlock
		return;
	}

	//nit ne ceka samu sebe
	if (myPCB ==(PCB*) Kernel::running){
		_unlock
		return;
	}

	//nit ne radi nista, potrebno ako su sve blocked
	if(myPCB->state==NEW){
		_unlock
		return;
	}

	if(this == Kernel::idle){
		_unlock
		return;
	}

	//pocetna nit ne treba da se ceka
	if(this == Kernel::starting){
		_unlock
		return;
	}

	Kernel::running->state=BLOCKED;
	myPCB->waitQueue->put((PCB*)Kernel::running);		//a time Queue

	_unlock
	dispatch(); 	//sta je, da li running vracamo u scheduler


};


//-------------------------ZADATAK 2 DODATAK--------------------

void Thread::signal(SignalId signal){
	_lock
	if((signal<0)||(signal>15)){
		_unlock
		return;
	}
	unsigned int maska=1;
	maska=maska<<signal;
	if((Thread::maskedS&maska)||(maskedT&maska)){
		_unlock
		return;
	}
	if((Thread::blockedS&maska)||(blockedT&maska)){
		signals->put(signal);
		_unlock
		return;
	}
	//if paused unpause
	if(myPCB->thread->paused==1){
		paused=0;
		myPCB->state=READY;
		signals->put(signal);
		Scheduler::put(myPCB);
		_unlock
		return;
	}
	if(Kernel::running==myPCB){
		SignalHandler sh=getHandler(signal);
		if (sh!= 0){
			int temp=Kernel::running->lockFlag;
			Kernel::running->lockFlag=1;
			asm{
				pushf
				sti
			}
			sh();
			asm{
				popf
			}
			Kernel::running->lockFlag=temp;
		}
	}
	else signals->put(signal);

	//_unlock pa poziv funkcije
	_unlock
	//signalHandle();
	/*
	while(!signals->empty()){
		ElemI *curr=signals->first;
		SignalId s=signals->get();
		SignalHandler sh=getHandler(s);
		//treba provera da li je blocked, pa ako jeste, vracamo ga na I mesto?
		if(sh!=0){
			unsigned int maska=1;
			maska=maska<<s;
			if ((maska&maskedS)||(maska&maskedT)){
				continue;
			}
			if ((maska&blockedS)||(maska&blockedT)){
					//trazimo prvi koji nije blokiran ili maskiran
				ElemI *prev=0;
				while(curr!=0){
					prev=curr;
					curr=curr->next;
					//curr!=0
					maska=1;
					maska=maska<<curr->signal;
					if ((maska&maskedS)||(maska&maskedT)){
								prev->next=curr->next;
								delete curr;
								curr=prev->next;
					}
				}
				if(curr==0) continue;
			}
			Kernel::running->lockFlag=1;
			_unlock
			//guard pa unguard
			asm{
				pushf
				sti
			}
			(*sh)();
			asm popf
			_lock
			Kernel::running->lockFlag=0;
		}
 	}
	_unlock
	*/
};
void Thread::registerHandler(SignalId signal, SignalHandler handler){
	_lock
		if((signal<0)||(signal>15)){
				_unlock
				return ;
		}
	if((signal==1)||(signal==2)){
		if((PCB*)Kernel::running!=Kernel::starting->getPCB()){
			_unlock
			return;
		}
	}
	handlers[signal]=handler;
	_unlock
};
SignalHandler Thread::getHandler(SignalId signal){
	_lock
	if((signal<0)||(signal>15)){
			_unlock
			return 0;
	}
	_unlock
	return handlers[signal];
};
void Thread::maskSignal(SignalId signal){
	_lock
	if((signal<0)||(signal>15)){
				_unlock
				return ;
		}
	unsigned int maska=1;
	maska=maska<<signal;
	maskedT|=maska;
	_unlock
};
void Thread::maskSignalGlobally(SignalId signal){
	_lock
		if((signal<0)||(signal>15)){
					_unlock
					return ;
			}
		unsigned int maska=1;
		maska=maska<<signal;
		maskedS|=maska;
	_unlock
};
void Thread::unmaskSignal(SignalId signal){
	_lock
		if((signal<0)||(signal>15)){
					_unlock
					return ;
			}
		unsigned int maska=1;
		maska=maska<<signal;
		maska=~maska;
		maskedT&=maska;
		_unlock
};
void Thread::unmaskSignalGlobally(SignalId signal){
	_lock
		if((signal<0)||(signal>15)){
					_unlock
					return ;
			}
		unsigned int maska=1;
		maska=maska<<signal;
		maska=~maska;
		maskedS&=maska;
		_unlock

};
void Thread::blockSignal(SignalId signal){
	_lock
		if((signal<0)||(signal>15)){
					_unlock
					return ;
			}
	unsigned int maska=1;
	maska=maska<<signal;
	blockedT|=maska;
	_unlock
};
void Thread::blockSignalGlobally(SignalId signal){
	_lock
			if((signal<0)||(signal>15)){
						_unlock
						return ;
				}
		unsigned int maska=1;
		maska=maska<<signal;
		blockedS|=maska;
		_unlock
};
void Thread::unblockSignal(SignalId signal){
	_lock
			if((signal<0)||(signal>15)){
						_unlock
						return ;
				}
	unsigned int maska=1;
	maska=maska<<signal;
	maska=~maska;
	blockedT&=maska;
	_unlock
};
void Thread::unblockSignalGlobally(SignalId signal){
	_lock
				if((signal<0)||(signal>15)){
							_unlock
							return ;
					}
		unsigned int maska=1;
		maska=maska<<signal;
		maska=~maska;
		blockedS&=maska;
	_unlock

};
void Thread::pause(){
	_lock
	//nit se suspenduje----stavljamo u scheduler pa dispatch()?
	Kernel::running->state=BLOCKED;
	Kernel::running->thread->paused=1;
	_unlock
	Kernel::dispatch();
};
