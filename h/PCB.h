/*
 * PCB.h
 *
 *  Created on: May 14, 2017
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_


#include "Queue.h"
#include "Thread.h"
typedef unsigned int Time;
typedef enum {NEW, READY, RUNNING, BLOCKED, FINISHED} State;

class Queue;
class Thread;
class PCB{

public:
	PCB(Thread* myT, StackSize stackSize, Time timeSlice);
	~PCB();

	//regular PCB getters
	StackSize getStackSize() const;
	Time getTimeSlice() const;
	State getState() const;
	Thread* getThread() const;
	ID getId() const;

	void runSignals();
	ID threadID;
	void createStack();
	Queue *waitQueue;
	//static Queue* sleepQueue;	//da li nam treba uopste
	static Queue* identificationQueue; //??
	static Time minTimeSlice;
	State state;

	int flag;			//mozda unsigned?
	unsigned lockFlag;
	Time timeSlice;
	Time timeTaken;
	Time timeWait;
	static int numID;
	Thread *thread;
	StackSize stackSize;
	unsigned* stack;		//a ne int?, vrvno 1 rec
	unsigned int sp, ss, bp;
};



#endif /* PCB_H_ */
