/*
 * KernelE.cpp
 *
 *  Created on: Jun 8, 2017
 *      Author: OS1
 */

#include "KernelE.h"

#include "PCB.h"
#include "lock.h"
#include "SCHEDULE.h"
#include "Kernel.h"
#include "IVTEntry.h"
#include <iostream.h>
class PCB;

//global array with information about each entry (256 entries)
extern IVTEntry* IVTEntries[];

/*constructor, destructor*/
KernelEv::KernelEv(IVTNo ivtNo) : val(0), entryNum(ivtNo) {
	eventCreator = (PCB*)Kernel::running;
	IVTEntries[ivtNo]->setKernelEv(this);
}

KernelEv::~KernelEv() {
	IVTEntries[entryNum]->setKernelEv(NULL);
}

/*wait*/
void KernelEv::block() {
	eventCreator->state=BLOCKED;
	//_unlock_
	dispatch();
}

void KernelEv::wait() {
	if(eventCreator == Kernel::running){
		if(val == 0) {
			block();
		}
		else val=0;
	}
}

/*signal, val can be in {-1, 0, 1}, -1,  if val == 1 don't increment*/
void KernelEv::deblock(){
	eventCreator->state=READY;
	Scheduler::put(eventCreator);
	eventCreator=0;
}

void KernelEv::signal() {
	//if(val < 1 && val++ < 0) deblock();
	if(eventCreator==0) val=1;
	else if(val==0){
		val++;
		deblock();
	}
}
