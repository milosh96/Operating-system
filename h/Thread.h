/*
 * Thread.h
 *
 *  Created on: May 18, 2017
 *      Author: OS1
 */

#ifndef THREAD_H_
#define THREAD_H_

typedef unsigned StackSize;				//umesto unsigned long?
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;
typedef int UID;
class PCB; // Kernel's implementation of a user's thread

//Zadatak 2
typedef unsigned SignalId;
typedef void (*SignalHandler)();

#include "QueueI.h"

class Thread {
public:
	 void start();
	 void waitToComplete();
	 virtual ~Thread();
	 ID getId() const;
	 static ID getRunningId();
	 static Thread * getThreadById(ID id);
	 PCB *getPCB();
	 Thread ( StackSize stackSize=defaultStackSize , Time timeSlice=defaultTimeSlice);//proteceted?

	 //Zadatak 2
	 void signal(SignalId signal);
	 void registerHandler(SignalId signal, SignalHandler handler);
	 SignalHandler getHandler(SignalId signal);
	 void maskSignal(SignalId signal);
	 static void maskSignalGlobally(SignalId signal);
	 void unmaskSignal(SignalId signal);
	 static void unmaskSignalGlobally(SignalId signal);
	 void blockSignal(SignalId signal);
	 static void blockSignalGlobally(SignalId signal);
	 void unblockSignal(SignalId signal);
	 static void unblockSignalGlobally(SignalId signal);
	 static void pause();
	 SignalHandler *handlers;
	 static void handle0() ;
	 QueueI *signals;
	 unsigned int blockedT;
	 static unsigned int blockedS;
	 unsigned int maskedT;
	 static unsigned int maskedS;
	 PCB *parent;
	 int paused;
	 //kraj zadatka 2

protected:
	 friend class PCB;

	 virtual void run() {}
	 friend class IdleThread;
	 friend class Kernel;
private:
	 PCB* myPCB;
};
void dispatch ();

extern void tick();

#endif /* THREAD_H_ */
