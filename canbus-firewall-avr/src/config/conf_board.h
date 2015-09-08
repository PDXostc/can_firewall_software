/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

/* Pin Mapping for GPIO */
#define GPIO_PIN_CAN_RX_NORTH       AVR32_CANIF_RXLINE_0_1_PIN      //Pin 45 -> IVI
#define GPIO_PIN_CAN_TX_NORTH       AVR32_CANIF_TXLINE_0_1_PIN      //Pin 46 -> IVI
#define GPIO_PIN_CAN_RX_SOUTH       AVR32_CANIF_RXLINE_1_5_PIN      //Pin 39 -> CAR
#define GPIO_PIN_CAN_TX_SOUTH       AVR32_CANIF_TXLINE_1_5_PIN      //Pin 40 -> CAR

#define GPIO_FUNCTION_CAN_RX_NORTH  AVR32_CANIF_RXLINE_0_1_FUNCTION     
#define GPIO_FUNCTION_CAN_TX_NORTH  AVR32_CANIF_TXLINE_0_1_FUNCTION     
#define GPIO_FUNCTION_CAN_RX_SOUTH  AVR32_CANIF_RXLINE_1_5_FUNCTION     
#define GPIO_FUNCTION_CAN_TX_SOUTH  AVR32_CANIF_TXLINE_1_5_FUNCTION

#define BOARD_OSC0_HZ           12000000
#define BOARD_OSC0_STARTUP_US   2000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC32_HZ          32768
#define BOARD_OSC32_STARTUP_US  71000
#define BOARD_OSC32_IS_XTAL     true

#endif // CONF_BOARD_H
