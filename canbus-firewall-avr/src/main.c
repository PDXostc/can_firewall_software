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
volatile can_msg_t CAN_MOB_NORTH_RX_SOUTH_TX[NB_MOB_CHANNEL] __attribute__ ((section (".hsb_ram_loc")));
volatile can_msg_t CAN_MOB_SOUTH_RX_NORTH_TX[NB_MOB_CHANNEL] __attribute__ ((section (".hsb_ram_loc")));
#elif defined (__ICCAVR32__)
volatile __no_init can_msg_t CAN_MOB_NORTH_RX_SOUTH_TX[NB_MOB_CHANNEL] @0xA0000000;
volatile __no_init can_msg_t CAN_MOB_SOUTH_RX_NORTH_TX[NB_MOB_CHANNEL] @0xA0000000;
#endif


/* Call backs */
void can_out_callback_north_rx(U8 handle, U8 event){
    //TODO
    //stub
}
void can_out_callback_north_tx(U8 handle, U8 event){
    //TODO
    //stub
}
void can_out_callback_south_rx(U8 handle, U8 event){
    //TODO
    //stub
}
void can_out_callback_south_tx(U8 handle, U8 event){
    //TODO
    //stub
}
void can_prepare_data_to_send_north(void){
    //TODO
    //stub
    //Init channel north
    can_init(CAN_CH_NORTH,
    ((U32)&CAN_MOB_SOUTH_RX_NORTH_TX[0]),
    CANIF_CHANNEL_MODE_NORMAL,
    can_out_callback_north_tx);
    //Allocate mob for TX
    north_tx_msg[0].handle = can_mob_alloc(CAN_CH_NORTH);
    
    //Check no mob available
    //if(south_tx_msg[0].handle==CAN_CMD_REFUSED){
    ////--this was provided by the example...this won't infinite loop?
    ////--check in debugger, inspect behavior
    //while(true);
    //}
    //--example has conversion of data to meet adc standard from dsp lib.. not sure if necessary
#if DBG_CAN_MSG
    print_dbg("\r\nNorth_CAN_msg\n\r");
    print_dbg_ulong(north_tx_msg[0].handle);
    print_dbg_ulong(north_tx_msg[0].can_msg->data.u64);
#endif
    
    can_tx(CAN_CH_NORTH,
    north_tx_msg[0].handle,
    north_tx_msg[0].dlc,
    north_tx_msg[0].req_type,
    north_tx_msg[0].can_msg);
    
    //or ??
    //while(north_tx_msg[0].handle==CAN_CMD_REFUSED);
}

void can_prepare_data_to_receive_north(void){
    //TODO
    //stub
    //Init channel north
    can_init(CAN_CH_NORTH, 
            ((U32)&CAN_MOB_NORTH_RX_SOUTH_TX[0]),
            CANIF_CHANNEL_MODE_NORMAL,
            can_out_callback_north_rx);
    //Allocate mob for TX
    north_rx_msg[0].handle = can_mob_alloc(CAN_CH_NORTH);
    
    //Check no mob available
    //if(north_rx_msg[0].handle==CAN_CMD_REFUSED){
        ////--this was provided by the example...this won't infinite loop?
        ////--check in debugger, inspect behavior
        //while(true);
    //}
    //--example has conversion of data to meet adc standard from dsp lib.. not sure if necessary
    
    can_rx(CAN_CH_NORTH,
            north_rx_msg[0].handle,
            north_rx_msg[0].req_type,
            north_rx_msg[0].can_msg);
    
    //or ??
    //while(north_tx_msg[0].handle==CAN_CMD_REFUSED);
}

void can_prepare_data_to_send_south(void){
    //TODO
    //stub
     //Init channel north
     can_init(CAN_CH_SOUTH,
     ((U32)&CAN_MOB_NORTH_RX_SOUTH_TX[0]),
     CANIF_CHANNEL_MODE_NORMAL,
     can_out_callback_south_tx);
     //Allocate mob for TX
     south_tx_msg[0].handle = can_mob_alloc(CAN_CH_SOUTH);
     
     //Check no mob available
     //if(south_tx_msg[0].handle==CAN_CMD_REFUSED){
     ////--this was provided by the example...this won't infinite loop?
     ////--check in debugger, inspect behavior
     //while(true);
     //}
     //--example has conversion of data to meet adc standard from dsp lib.. not sure if necessary
#if DBG_CAN_MSG
     print_dbg("\r\nSouth_CAN_msg\n\r");
     print_dbg_ulong(north_tx_msg[0].handle);
     print_dbg_ulong(north_tx_msg[0].can_msg->data.u64);
#endif

     can_tx(CAN_CH_SOUTH,
     south_tx_msg[0].handle,
     south_tx_msg[0].dlc,
     south_tx_msg[0].req_type,
     south_tx_msg[0].can_msg);
     
     //or ??
     //while(north_tx_msg[0].handle==CAN_CMD_REFUSED);
}

void can_prepare_data_to_receive_south(void){
    //TODO
    //stub
}

void process_test(can_mob_t *msg_in, can_mob_t *msg_out) {
    
}
int main (void)
{
    /* Insert system clock initialization code here (sysclk_init()). */
    sysclk_init();
    
    board_init();
    
    //init debug printing for usart
    init_dbg_rs232(sysclk_get_pba_hz());
    
    print_dbg("\r======INITIALIZED======\n\r");
    
    #if DBG_CLKS
    /* test return of clocks */
    //clk_main = sysclk_get_main_hz();
    //clk_cpu = sysclk_get_cpu_hz();
    print_dbg("\n\rMain Clock Freq\n\r");
    print_dbg_ulong(sysclk_get_main_hz());
    print_dbg("\n\rCPU Freq\n\r");
    print_dbg_ulong(sysclk_get_cpu_hz());
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
    
    //test
    int count = 0;
    //while(true) {
        print_dbg("\n\rData Send");
        print_dbg_ulong(count);
        can_prepare_data_to_send_north();
        can_prepare_data_to_send_south();
        count++;
    //}
    
    // -- TODO: Interrupt Handling?
    // -- global interrupts?
    //TODO:
    /*
    * State machine process queue:
    * RX both channels
    * process msgs
    * TX both channels
    */
    
    ///* Initialize CAN channels */
    //// IVI channel 0
    //can_init(CAN_CH_NORTH, ((uint32_t)&CAN_MOB_NORTH_RX_SOUTH_TX[0]), CANIF_CHANNEL_MODE_NORMAL, can_out_callback_north);
    //// CAR channel 1
    //can_init(CAN_CH_SOUTH, ((uint32_t)&CAN_MOB_SOUTH_RX_NORTH_TX[0]), CANIF_CHANNEL_MODE_NORMAL, can_out_callback_south);
}
