/*
 * Kernel.cpp
 *
 *  Created on: May 20, 2017
 *      Author: OS1
 */

#include "Kernel.h"
#include "PCB.h"
#include "Thread.h"
#include "SCHEDULE.h"
#include "lock.h"
#include <dos.h>
#include <stdlib.h>
#include "KernelS.h"

//inizijalizacija static komponenti!!

volatile PCB *Kernel::running=0;
Thread *Kernel::starting=0;
IdleThread *Kernel::idle=0;
interruptfunction Kernel::oldTimer=0;
volatile int Kernel::explicit_dispatch=0;



void Kernel::initialization(){
	/*
	 * promena tajmera
	 * i  stvaranje 2 niti--idle i starting?
	 */
	_lock
#ifndef BCC_BLOCK_IGNORE
	oldTimer=getvect(8);
	setvect(0x60,oldTimer);
	setvect(0x08,&timerIR);
#endif
	starting = new Thread(0x10000, PCB::minTimeSlice);	//5 umesto min time
	starting->myPCB->state = READY;
	running = starting->myPCB;
	idle = new IdleThread();
	idle->start();		//ipak ide u scheduler?


	_unlock

};

void Kernel::restore(){
	/*
	 * vracanje u pocetno stanje
	 * verovatno zavrsavamo sa ostalim nitima
	 */

	if (running!=starting->myPCB) return;
	_lock
	Queue *old=PCB::identificationQueue;
	delete old;
/*	old=PCB::sleepQueue;
	delete old;
*/
#ifndef BCC_BLOCK_IGNORE
	setvect(0x08,oldTimer);
#endif

	delete starting;
	//zavrsavanje niti, brisanje!!

	_unlock
};


void Kernel::dispatch(){
	_lock
	explicit_dispatch=1;
	timerIR();		//interrupt nam pushuje kontrolne registre, pa imamo manje posla
	explicit_dispatch=0;
	_unlock
};

void Kernel::wrapper(){
	running->thread->run();
	_lock
	PCB::identificationQueue->deleteFromQueue((PCB*)running);
	running->state=FINISHED;

	PCB *temp;
	while(!running->waitQueue->empty()){
		temp=running->waitQueue->get();
		if(temp!=0){
			temp->state=READY;		//a ubacivanje u scheduler?
			Scheduler::put(temp);
		}
	}
	running->thread->signal(2);
	if(running->thread->parent!=0) running->thread->parent->thread->signal(1);
	_unlock
	dispatch();		//nisu stavili unlock?

};

//pushuje sve na stek(ax,bx...);ne sme da ima lokalne promenljive--->resenje:static ili global
void interrupt Kernel::timerIR(...){

	static volatile unsigned int tempSP, tempSS,tempBP;		//volatile obavezno?
	static PCB *nextThread;
	//pozivanje obicnog tajmera, da ostatak programa moze da funkcionise
	if (explicit_dispatch==0){
		tick();			//sta tacno radi, i koja biblioteka
		//(*oldTimer)();
		#ifndef BCC_BLOCK_IGNORE
		asm int 60h;
		#endif
	}

	//azuriranje vremenskog cekanja
	if (explicit_dispatch==0)
		KernelSem::noNullQueue->update();		//treba da dodamo waitingQ? da li da cuvamo semafor uz svaku nit/pcb

	if ((explicit_dispatch==0)&&(running->timeSlice!=0))
		running->timeTaken+=PCB::minTimeSlice;

	if((explicit_dispatch==0)&&(running->timeTaken<running->timeSlice||running->timeSlice==0))
			return;

	if(running->lockFlag==1)
		return;
	explicit_dispatch=0;
	//stanje treba da bude running?!!!
	if((running->state==READY)&&(running!=idle->myPCB)){//zasto mora ready provera(svaki sl)
		Scheduler::put((PCB*)running);
	}



	/*
	 * promena konteksta
	 * pronalazenje niti koja je ready
	 * menjane sp i ss
	 */



		nextThread=Scheduler::get();
		if (nextThread==0){
			/*if(!PCB::identificationQueue->empty())				//na starting se prebacujemo ako je ona jedina ostala
				nextThread=idle->myPCB;
			else
				nextThread=starting->myPCB;
			*/
			nextThread=idle->myPCB;
		}

											//trazimo dalje; kada se ovo moze desiti?


		//actual contex switch
		asm{
			mov tempSS,ss			//da li treba jos nesto !!!!bp!!!
			mov tempSP,sp
			mov tempBP,bp
		}
		running->ss=tempSS;
		running->sp=tempSP;
		running->bp=tempBP;


		running=nextThread;
		tempSS=running->ss;
		tempSP=running->sp;
		tempBP=running->bp;
		asm{
			mov sp,tempSP
			mov ss,tempSS
			mov bp,tempBP
		}

		running->timeTaken=0;
		running->runSignals();
}
