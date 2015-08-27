/**
* \file
*
* \brief Empty user application template
*
*/

/**
* \mainpage User Application template doxygen documentation
*
* \par Empty user application template
*
* Bare minimum empty user application template
*
* \par Content
*
* -# Include the ASF header files (through asf.h)
* -# "Insert system clock initialization code here" comment
* -# Minimal main function that starts with a call to board_init()
* -# "Insert application code here" comment
*
*/

/*
* Include header files for all drivers that have been imported from
* Atmel Software Framework (ASF).
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/
#include <asf.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#include "intc.h"
#include "can.h"
#include "canif.h"
#include "usart.h"
#include "print_funcs.h"
#include "conf_debug.h"
#include "conf_can.h"
#include "conf_messages.h"
//#include "conf_can_example.h"

uint32_t clk_main, clk_cpu, clk_periph, clk_busa, clk_busb;

// CAN MOB allocation into HSB RAM
#if defined (__GNUC__) && defined (__AVR32__)
volatile can_msg_t CAN_MOB_NORTH[NB_MOB_CHANNEL] __attribute__ ((section (".hsb_ram_loc")));
volatile can_msg_t CAN_MOB_SOUTH[NB_MOB_CHANNEL] __attribute__ ((section (".hsb_ram_loc")));
#elif defined (__ICCAVR32__)
volatile __no_init can_msg_t CAN_MOB_NORTH[NB_MOB_CHANNEL] @0xA0000000;
volatile __no_init can_msg_t CAN_MOB_SOUTH[NB_MOB_CHANNEL] @0xA0000000;
#endif

/* Call backs */
void can_out_callback_north(U8 handle, U8 event){
    //TODO
    //stub
}
void can_out_callback_south(U8 handle, U8 event){
    //TODO
    //stub
}
int main (void)
{
    /* Insert system clock initialization code here (sysclk_init()). */
    sysclk_init();
    
    board_init();
    
    //init debug printing for usart
    init_dbg_rs232(sysclk_get_pba_hz());
    
    #if DBG_CLKS
    /* test return of clocks */
    //clk_main = sysclk_get_main_hz();
    //clk_cpu = sysclk_get_cpu_hz();

    switch (sysclk_get_cpu_hz())
    {
        case 60000000:
        print_dbg("\n\rCPU Clock: 60MHZ\n\r");
        print_dbg("");
        break;
        default:
        print_dbg("\n\rcpu mhz outside expectations\n\r");
    }
    #endif
    /* Insert application code here, after the board has been initialized. */
    
    /* Setup generic clock for CAN */
    scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
    SCIF_GCCTRL_CPUCLOCK,
    AVR32_SCIF_GC_NO_DIV_CLOCK,
    0);
    
    #if DBG_CLKS
    print_dbg("\n\rGeneric clock setup\n\r");
    #endif

    /* Enable generic clock */
    scif_gc_enable(AVR32_SCIF_GCLK_CANIF);
    
    #if DBG_CLKS
    print_dbg("\n\rGeneric clock enabled\n\r");
    #endif

    /* Create GPIO Mappings for CAN */
    static const gpio_map_t CAN_GPIO_MAP = 
    {
        {            GPIO_PIN_CAN_RX_NORTH, GPIO_FUNCTION_CAN_RX_NORTH        },
        {            GPIO_PIN_CAN_TX_NORTH, GPIO_FUNCTION_CAN_TX_NORTH        },
        {            GPIO_PIN_CAN_RX_SOUTH, GPIO_FUNCTION_CAN_RX_SOUTH        },
        {            GPIO_PIN_CAN_TX_SOUTH, GPIO_FUNCTION_CAN_TX_SOUTH        }
    };
    /* Assign GPIO to CAN */
    gpio_enable_module(CAN_GPIO_MAP, sizeof(CAN_GPIO_MAP) / sizeof(CAN_GPIO_MAP[0]));
    
    // -- TODO: Interrupt Handling?
    
    /* Initialize CAN channels */
    // IVI channel 0
    can_init(CAN_CH_NORTH, ((uint32_t)&CAN_MOB_NORTH[0]), CANIF_CHANNEL_MODE_NORMAL, can_out_callback_north);
    // CAR channel 1
    can_init(CAN_CH_SOUTH, ((uint32_t)&CAN_MOB_SOUTH[0]), CANIF_CHANNEL_MODE_NORMAL, can_out_callback_south);
}
