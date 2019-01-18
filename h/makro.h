/*
 * makro.h
 *
 *  Created on: Jun 27, 2017
 *      Author: OS1
 */

#ifndef MAKRO_H_
#define MAKRO_H_


#define PREPAREENTRY(numEntry, callOld)\
\
void interrupt inter##numEntry(...); \
\
IVTEntry newEntry##numEntry(numEntry, &inter##numEntry); \
\
void interrupt inter##numEntry(...){\
	newEntry##numEntry.signal();\
	if(callOld == 1) newEntry##numEntry.callOldRoutine();\
	dispatch();\
}


#endif /* MAKRO_H_ */
