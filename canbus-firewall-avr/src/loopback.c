/*
* loopback.c
*
* Created: 10/12/2015 11:15:48 AM
*  Author: smiller6
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
		return true;
	}
	//disable output for safety of port
	gpio_local_disable_pin_output_driver(LOOP_B);
	return false;
}

 bool test_loopback(void)
{
	return test_loopback_delayed(DELAY_LOOPBACK_US);
}