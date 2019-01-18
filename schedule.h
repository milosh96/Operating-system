//schedule.h

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "pcb.h"

class PCB;

class Scheduler {
public:
	static void put(PCB*);
	static PCB* get();
};

#endif
