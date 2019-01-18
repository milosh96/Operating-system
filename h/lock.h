/*
 * lock.h
 *
 *  Created on: May 18, 2017
 *      Author: OS1
 */

#ifndef LOCK_H_
#define LOCK_H_


// ili samo cli i sti?

#define _lock asm {\
		pushf;\
		cli;\
	}


#define _unlock asm popf;




#endif /* LOCK_H_ */
