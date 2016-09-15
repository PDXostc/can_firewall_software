/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * reset.c
 *
 */ 

#include "reset.h"

// default options for the watchdog timer, provided as one shot simple reset after
// one second case
wdt_opt_t wdt_opt_default = {
	// timeout of wdt, wdt reset after this period
	.us_timeout_period = WDT_MIN_VALUE_US,
	// (Clock source) use internal RC oscillator timer as default for wdt
	.cssel = WDT_CLOCK_SOURCE_SELECT_RCSYS,
	// flash calibration redone after reset
	.fcd = false,
	// store final value, control register not locked
	.sfv = false,
	// mode basic, uses PSEL time
	.mode = WDT_BASIC_MODE,
	// (Disable after reset) After wdt reset, the wdt will not be reenabled
	.dar = true
	};