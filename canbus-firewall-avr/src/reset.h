/*
 * reset.h
 *
 * Created: 12/8/2015 10:22:58 AM
 *  Author: smiller6
 */ 


#ifndef RESET_H_
#define RESET_H_

#include <asf.h>

// minimum time value, 1s
#define WDT_MIN_VALUE_US		1000000

// default options for the watchdog timer
wdt_opt_t wdt_opt_default;

/* Use the Atmel Software Framework driver for basic WDT operation.
 * Can be started with
 * 
 * wdt_enable(&options).
 * 
 * Disable with 
 * 
 * wdt_disable() 
 * 
 * and clear running timer with
 * 
 * wdt_clear().
 * 
 */

#endif /* RESET_H_ */