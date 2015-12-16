/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * sleep.h
 *
 */ 


/*
*  Sleep routine(s) for our main program
*/

#ifndef SLEEP_H_
#define SLEEP_H_

#include <asf.h>

/**
 * \brief Turn off active peripherals, set wakeup modes, and initiate sleep of micro
 * 
 * \param 
 * 
 * \return extern void
 */
extern void sleep_mode_start(void);



#endif /* SLEEP_H_ */