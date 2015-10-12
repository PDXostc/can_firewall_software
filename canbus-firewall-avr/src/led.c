/*
 * led.c
 *
 * Created: 10/12/2015 9:17:41 AM
 *  Author: smiller6
 */ 
#include "led.h"

void init_led_gpio_ports()
{
	#ifdef LED_01
	gpio_configure_pin(LED_01, GPIO_DIR_OUTPUT | LED_OFF);
	set_led(LED_01, LED_OFF);
	#endif
	
	#ifdef LED_02
	gpio_configure_pin(LED_02, GPIO_DIR_OUTPUT | LED_OFF);
	set_led(LED_02, LED_OFF);
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