/*
 * Queue
 *
 *  Created on: May 14, 2017
 *      Author: OS1
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include "PCB.h"
#include "Thread.h"

class PCB;
class Thread;

class Queue{
private:
	struct Elem {
		PCB *pcb;
		Elem* next;
		Elem (PCB* p, Elem* nnext=0) {
			pcb=p;
			next=nnext;
		}
	};


public:
	Elem *first, *last;
	Queue():first(0),last(0) {}
	~Queue();
	void put(PCB *p);
	PCB* get();
	//ubacimo za sorted insert i get?
	int empty() ;
	ID findIDof(UID uID);
	Thread *findThreadwithID(ID threadID);
	int deleteFromQueue(PCB* pcb);
	void update();

	void deleteFromSorted(PCB *pcb);
	void putSorted(PCB *pcb);
};



#endif /* QUEUE_H_ */
