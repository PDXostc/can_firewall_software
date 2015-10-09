/*
 * sleep.c
 *
 * Created: 9/14/2015 12:08:08 PM
 *  Author: smiller6
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
    SLEEP(AVR32_PM_SMODE_STANDBY);
}