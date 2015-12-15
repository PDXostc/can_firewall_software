/*
 * reset.c
 *
 * Created: 12/8/2015 10:23:16 AM
 *  Author: smiller6
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