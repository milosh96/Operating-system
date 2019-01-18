/*
 * IVTEntry.cpp
 *
 *  Created on: Jun 8, 2017
 *      Author: OS1
 */
#include "KernelE.h"
#include <dos.h>
#include <stdlib.h>
#include "IVTEntry.h"
class KernelEv;

IVTEntry* IVTEntries[256]; // global array of IVT entries (256)

/*constructor, destructor*/
IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt interruptRoutine) : entryNum(ivtNo), kernelEvent(NULL) {
	IVTEntries[ivtNo] = this;
#ifndef BCC_BLOCK_IGNORE
	oldRoutine = getvect(entryNum);
	setvect(entryNum, interruptRoutine);
#endif
}

IVTEntry::~IVTEntry(){
#ifndef BCC_BLOCK_IGNORE
	setvect(entryNum, oldRoutine);
#endif
}

//dereferencing pointer to old routine
void IVTEntry::callOldRoutine(){
	(*oldRoutine) ();
}

//delegate signal to kernelEv
void IVTEntry::signal() {
	if(kernelEvent != 0) kernelEvent->signal();
}

//static getter
IVTEntry* IVTEntry::getEntry(IVTNo ivtNo){
	return IVTEntries[ivtNo];
}

//setter
void IVTEntry::setKernelEv(KernelEv* event){
	kernelEvent = event;
}
