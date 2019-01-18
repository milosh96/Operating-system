/*
 * SQueue.h
 *
 *  Created on: Jun 26, 2017
 *      Author: OS1
 */

#ifndef SQUEUE_H_
#define SQUEUE_H_

#include "KernelS.h"
class KernelSem;

class SQueue{

public:
	struct Str{
	KernelSem* sem;
	Str* next;
	Str(KernelSem* p, Str* sled = 0){

		sem = p;
		next = sled;
	}
};
	Str* head,*last;
	friend class KernelSem;
	friend class Queue;

	SQueue();
	void put(KernelSem* p);
	void remove(KernelSem* p);
	int isEmpty();
	~SQueue();

};




#endif /* SQUEUE_H_ */
