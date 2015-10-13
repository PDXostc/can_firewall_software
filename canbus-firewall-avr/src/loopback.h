/*
 * loopback.h
 *
 * Created: 10/12/2015 10:56:24 AM
 *  Author: smiller6
 */ 

#include <asf.h>

#ifndef LOOPBACK_H_
#define LOOPBACK_H_

#ifndef LOOP_A_PIN
#error LOOP_A_PIN NOT SET
#else
#define LOOP_A	LOOP_A_PIN
#endif

#ifndef LOOP_B_PIN
#error LOOP_B_PIN NOT SET
#else
#define LOOP_B	LOOP_B_PIN
#endif

//tested value of 3us appears to provide enough time for propogation
#define DELAY_LOOPBACK_US	3  

void init_loopback_gpio_ports(void);

bool test_loopback_delayed(unsigned long delay);

extern bool test_loopback(void);



#endif /* LOOPBACK_H_ */