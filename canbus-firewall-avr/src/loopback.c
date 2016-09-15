/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
* loopback.c
*
*/
#include "loopback.h"

void init_loopback_gpio_ports()
{
	gpio_local_init();
	gpio_configure_pin(LOOP_A, GPIO_DIR_INPUT | GPIO_PULL_UP);
	gpio_configure_pin(LOOP_B, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
}

bool test_loopback_delayed(unsigned long delay)
{
	//init ports, ensuring settings
	init_loopback_gpio_ports();
	//wait for propogation
	delay_us(delay);
	//get the initial value of our pin to check after toggle
	bool initial_a = gpio_local_get_pin_value(LOOP_A);
	//toggle B, expecting change in A
	gpio_local_tgl_gpio_pin(LOOP_B);
	//wait for propogation
	delay_us(delay);
	//expect A has changed
	if (gpio_local_get_pin_value(LOOP_A) != initial_a)
	{
		//disable output for safety of port
		gpio_local_disable_pin_output_driver(LOOP_B);
		
		#if DBG_LED_USE_LED_LOOPBACK
		set_led(LED_02, LED_ON);
		#endif
		
		return true;
	}
	//disable output for safety of port
	gpio_local_disable_pin_output_driver(LOOP_B);
	
	#if DBG_LED_USE_LED_LOOPBACK
	set_led(LED_02, LED_OFF);
	set_led(LED_01, LED_ON);
	#endif
	
	return false;
}

 bool test_loopback(void)
{
	return test_loopback_delayed(DELAY_LOOPBACK_US);
}