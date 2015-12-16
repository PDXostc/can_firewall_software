/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/


#ifndef CONF_BOARD_H
#define CONF_BOARD_H 

#define BOARD_OSC0_HZ           16000000
#define BOARD_OSC0_STARTUP_US   2000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC32_HZ          32768
#define BOARD_OSC32_STARTUP_US  71000
#define BOARD_OSC32_IS_XTAL     true

#ifndef USER_BOARD
#define USER_BOARD
//User board case (ours) //using usart1...
#  define DBG_USART               (&AVR32_USART1)
#  define DBG_USART_RX_PIN        AVR32_USART1_RXD_0_1_PIN		//PD12 -> Pin 54
#  define DBG_USART_RX_FUNCTION   AVR32_USART1_RXD_0_1_FUNCTION
#  define DBG_USART_TX_PIN        AVR32_USART1_TXD_0_1_PIN		//PD11 -> Pin 53
#  define DBG_USART_TX_FUNCTION   AVR32_USART1_TXD_0_1_FUNCTION
#  define DBG_USART_IRQ           AVR32_USART1_IRQ
#  define DBG_USART_BAUDRATE      57600

#endif

/* LED Ports
 * Please ensure these settings do not conflict with other desired ports*/
#define LED_01_PIN		AVR32_PIN_PD27	//red
#define LED_02_PIN		AVR32_PIN_PD28	//blue

//Loopback terminator
#define LOOP_A_PIN	AVR32_PIN_PC19		//Pin 43
#define LOOP_B_PIN	AVR32_PIN_PC20		//Pin 44

//SPI and Chip select
#define MISO_PIN	AVR32_PIN_PD01		//Pin 48
#define MOSI_PIN	AVR32_PIN_PD00		//PIN 47
#define	SCLK_PIN	AVR32_PIN_PD02		//PIN 49
#define NPCS_0_PIN	AVR32_PIN_PD03		//CAR_CS	//Pin 50
#define NPCS_2_PIN	AVR32_PIN_PC17		//IVI_CS	//Pin 41

//MCP signals
#define CAR_RESET_PIN	AVR32_PIN_PD13		//PIN 55
#define CAR_INT_PIN		AVR32_PIN_PD29		//PIN 60
#define CAR_STBY_PIN	AVR32_PIN_PD14		//PIN 56
#define IVI_RESET_PIN	AVR32_PIN_PC04		//PIN 37
#define IVI_INT_PIN		AVR32_PIN_PC03		//PIN 34
#define IVI_STBY_PIN	AVR32_PIN_PC05		//PIN 38

//Manual Interrupt Pins
#define MCP_MACHINE_INT_PIN		AVR32_PIN_PA05
#define PROC_INT_PIN			AVR32_PIN_PA21

#endif // CONF_BOARD_H
