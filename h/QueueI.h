/*
 * QueueI.h
 *
 *  Created on: Jun 28, 2017
 *      Author: OS1
 */

#ifndef QUEUEI_H_
#define QUEUEI_H_
#include "Thread.h"

class QueueI{
private:
	struct ElemI {
		SignalId signal;
		ElemI* next;
		ElemI (SignalId p, ElemI* nnext=0) {
			signal=p;
			next=nnext;
		}
	};


public:
	ElemI *first, *last;
	QueueI():first(0),last(0) {}
	~QueueI();
	void put(SignalId p);
	SignalId get();
	int empty() ;

};


#endif /* QUEUEI_H_ */
