/*
 * QueueI.cpp
 *
 *  Created on: Jun 28, 2017
 *      Author: OS1
 */


#include "Thread.h"
#include "QueueI.h"
#include "lock.h"
SignalId QueueI::get(){
	_lock
	if (first==0){
		_unlock
		return 99;
	}
	ElemI *curr=first;
	SignalId s=first->signal;
	first=first->next;
	delete curr;
	if(first==0)
		last=0;
	_unlock
	return s;
};

int QueueI::empty(){
	return first==0;
};

void QueueI::put(SignalId s){
	_lock
	ElemI *novi=new ElemI(s);
	if(first==0) first=novi;
	else last->next=novi;
	last=novi;
	_unlock
};


QueueI::~QueueI(){
	_lock
	ElemI* curr=this->first,*deletable;

	while(curr!=0){
		deletable=curr;
		curr=curr->next;
		delete deletable;
	}
	first=0;
	last=0;
	_unlock
};
