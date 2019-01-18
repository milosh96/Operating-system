/*
 * Queue.cpp
 *
 *  Created on: May 14, 2017
 *      Author: OS1
 */
#include "Queue.h"
#include "SCHEDULE.h"
#include "lock.h"
#include "SQueue.h"
#include "Kernel.h"

void Queue::put(PCB* p){
	_lock
	Elem *novi=new Elem(p);
	if (first==0) first=novi;
	else last->next=novi;

	last=novi;
	_unlock
};

PCB* Queue::get(){
	if(first==0) return 0;
	_lock
	Elem *curr=first;
	first=first->next;
	if(first==0) last=0;
	PCB *pcb=curr->pcb;
	delete curr;			//ne brise pcb, nije def. destruktor
	_unlock
	return pcb;
};



Queue::~Queue(){
	_lock
	Elem* curr=this->first,*deletable;

	while(curr!=0){
		deletable=curr;
		curr=curr->next;
		delete deletable;
	}
	first=0;
	last=0;
	_unlock
};

ID Queue::findIDof(ID Id){
	_lock
	Elem *curr=first;
	while(curr!=0){
			if(curr->pcb->threadID==Id) {
				_unlock
				return curr->pcb->threadID;
			}
		else curr=curr->next;
	}
	_unlock
	return 0;
};

Thread *Queue::findThreadwithID(ID threadID){
	_lock
	Elem *curr=first;

	while(curr!=0){
		if(curr->pcb->threadID==threadID) {
			_unlock
			return curr->pcb->thread;
		}
		else curr=curr->next;
	}
	_unlock
	return 0;
};

int Queue::deleteFromQueue(PCB* pcb){
	/*
	 * koristimo pri apdejtu, kada hocemo da izbacimo iz waitQueue-a
	 */
	_lock
	Elem *curr=first, *previous=0;

	while(curr!=0){
		if(curr->pcb==pcb) break;
		else{
			previous=curr;
			curr=curr->next;
		}
	}
	if(curr==0) {
		_unlock
		return 0;
	}
	else if(curr==first){
		first=first->next;
	}
	else{
		previous->next=curr->next;
	}
	if(curr==last) last=previous;

	delete curr;
	_unlock
	return 1;
};

void Queue::update(){
	/*
	 STARA VERZIJA za SORTED
	 * koristimo kod uspavanih niti, da smanjimo njihovo vreme cekanja ili da ih potpuno izbacimo
	 * !!!!!kod niti koje su uspavane

	Elem *curr=first;
		//prepravka nije dobro; smanjimo pa onda imamo while petlju!!!!!!
		//u wait ubacujemo samo one kojima je 0 wait
		//update da vrati neku vrednost onda?
	curr->pcb->timeWait-=1;
	if(curr->pcb->timeWait==0){		//==55
		while(curr!=0){
			curr->pcb->state=READY;
			Scheduler::put(curr->pcb);		//sta ako je vec u scheduleru; izvadjenja-bila running??
			first=first->next;
					//wait->deleteFromQueue(curr->pcb); u waitu su samo sa 0
			if(first==0){
				last=0;
						return;		//
			}
			curr=curr->next;
			if(curr->pcb->timeWait!=0) break;
		}

	}
	else ;	//return 0, pa u sledecem redu return 1 za wait/signal kod semafora
	*/
	_lock
	Elem *curr=first;
	while(curr!=0){
		PCB *pcb=curr->pcb;
		pcb->timeWait--;
		if(pcb->timeWait==0){
			curr=curr->next;
			this->deleteFromQueue(pcb);

			SQueue::Str* s = KernelSem::sQueue->head;
				while(s != 0) {
						int ret=s->sem->waitQueue->deleteFromQueue(pcb);
						if(ret == 1)						//sta je ovo
							s->sem->value++;
						s=s->next;
				}

			pcb->state=READY;
			if(pcb!=Kernel::idle->getPCB())
				Scheduler::put(pcb);
		}
		else curr=curr->next;
	}

	_unlock
};

void Queue::putSorted(PCB *pcb){
	_lock
	Elem *curr=first;
	Elem *previous=0;
	Elem *novi=new Elem(pcb);
	if(first==0){
		first=last=novi;
	}
	while(curr!=0){
		if(pcb->timeWait-curr->pcb->timeWait>0) {
			pcb->timeWait-=curr->pcb->timeWait;
			previous=curr;
			curr=curr->next;
		}
		else{
			previous->next=novi;
			novi->next=curr;
			curr->pcb->timeWait-=pcb->timeWait;
			_unlock
			return;
		}
	}
	previous->next=novi;
	last=novi;
	_unlock
};


void Queue::deleteFromSorted(PCB *pcb){
	_lock
	Elem *curr=first;
	Elem *prev=0;
	while(curr!=0){
		Elem *tmp=curr->next;

		if(pcb==tmp){
			if(curr->next!=0){
				curr->next->pcb->timeWait+=curr->pcb->timeWait;
			}
			if(curr==first) first=first->next;
			else prev->next=curr->next;
			if(curr==last) last=prev;
		}
		else {
			prev=curr;
			curr=curr->next;
		}
	}
	_unlock
};

int Queue::empty(){
	return first==0;
}

