/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * sleep.c
 *
 */ 

#include "sleep.h"

void sleep_mode_start()
{
    //disable CAN channels
    CANIF_disable(0);
    CANIF_disable(1);
    
    //wait for actual disable
    while(CANIF_channel_enable_status(0));
    while(CANIF_channel_enable_status(1));
    
    //enable wakeup
    CANIF_enable_wakeup(0);
    CANIF_enable_wakeup(1);
    
    //Go to sleep
    SLEEP(AVR32_PM_SMODE_IDLE);
}