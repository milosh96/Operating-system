/*
 * main.c
 *
 *  Created on: May 18, 2017
 *      Author: OS1
 */
#include <iostream.h>

#include "Idle.h"
#include "Thread.h"
#include "PCB.h"
#include "Kernel.h"
#include "Queue.h"

extern int userMain(int argc1, char* argv1[]);

int main(int argc, char* argv[]){


	Kernel::initialization();
	int value =userMain(argc, argv);
	Kernel::restore();





	printf("\nKRAJ!");
	/*cout<<"\nKraj!\n";
	int i;
	cin >> i;
*/
	return value;


}


/*
void tick(){
	printf("Pozz");
};
*/
