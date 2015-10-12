/*
 * led.h
 *
 * Created: 10/12/2015 9:01:06 AM
 *  Author: smiller6
 */ 
#include <asf.h>

#ifndef LED_H_
#define LED_H_

#define LED_LOW 0
#define LED_HIGH 1

#define LED_ON	LED_LOW		//low = on
#define LED_OFF	LED_HIGH	//high = off

extern void init_led_gpio_ports(void);

extern void set_led(uint32_t led, int set);




#endif /* LED_H_ */