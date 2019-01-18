/*
 * SQueue.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: OS1
 */



#include "SQueue.h"
#include "Kernel.h"
#include "lock.h"
#include "stdio.h"

SQueue::SQueue(){
	head = 0;
	last = 0;
}

void SQueue::put(KernelSem* ksem) {

	_lock
	Str* novi = new Str(ksem);
	if(head == 0) head=novi;
	else last->next = novi;
	last = novi;

	_unlock
}

void SQueue::remove(KernelSem* ksem)
{

	_lock
	if(head==0) {
		_unlock
		return;
	}

	Str* curr = head;
	Str *prev=0;
	while(curr!=0 && curr->sem!=ksem) {

		prev=curr;
		curr=curr->next;
	}

	if(curr==0) {

		_unlock
		return;
	}

	if(head->sem==ksem) {

		head=head->next;
		if(head==0)
			last=0;
		delete curr;
		_unlock
		return;
	}

	if(last != 0 && last->sem==ksem)
		last=prev;

	prev->next=curr->next;
	curr->next=0;

	delete curr;


	_unlock
}

int SQueue::isEmpty() {

	_lock
	if (head == 0) {

		_unlock
		return 1;
	}

	_unlock
	return 0;
}

SQueue::~SQueue() {

	_lock

	while(head != 0)
	{
		Str* old = head;
		head=head->next;
		old->next=0;
		delete old;
	}

	_unlock
}


