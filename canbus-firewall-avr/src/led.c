/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * led.c
 *
 */ 
#include "led.h"

void init_led_gpio_ports()
{
	#ifdef LED_01
	gpio_configure_pin(LED_01, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	#endif
	
	#ifdef LED_02
	gpio_configure_pin(LED_02, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	#endif
	
	//more if needed, etc...
}

void inline set_led(uint32_t led, int set)
{
	if(set == 0)
	{
		gpio_set_pin_low(led);
	} else {
		gpio_set_pin_high(led);
	}
}