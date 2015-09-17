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
#include "conf_rules.h"
#include "rules.h"
#include "sleep.h"
#include "test.h"
#include "polarssl/sha2.h"
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



//SRAM Allocation for loaded filter rulesets
static rule_t can_ruleset_north[16];
static rule_t can_ruleset_south[16];

//pointer to working rulesets, incoming
static rule_working_t *rule_working = NULL;
static int num_rules_working = SIZE_RULESET;

//physical security shunt, override to true during software testing
//if this is true, we can accept new rules
static bool detected_shunt = DETECTED_SHUNT;

//booleans for rx/tx
volatile bool message_received_north = false;
volatile bool message_received_south = false;
volatile bool message_transmitted_north = false;
volatile bool message_transmitted_south = false;

rule_t fake_rule =
{
    .prio = 0xff,
    .mask = 0xff,
    .filter = 0xff,
    .xform = 0xff,
    .idoperand = 0xff,
    .dtoperand = 0xff
};

rule_t control_rule =
{
    .prio = 0x01ff,
    .mask = 0x02ff,
    .filter = 0x03ff,
    .xform = 0x04ff,
    .idoperand = 0x05ff,
    .dtoperand = 0x06ff
};

rule_working_t working_test = {
    .prio = 0x99,
    .bitfield_completed = 0,
    .mask_xform = {
        .mask = 0xff,
        .xform = 0xF0
    },
    .filter_dtoperand_01 = {
        .filter = 0x04030201,
        .dtoperand01 = 0x0201
    },
    .dt_operand_02 = {
        .dtoperand02 = {0x0807, 0x0605, 0x0403}
    },
    .id_operand_hmac_01 = {
        .idoperand = 0x04030201
    }
};

Union64 data_frame_test;

/* Call backs */
void can_out_callback_north_rx(U8 handle, U8 event){
    //message has been received, move data from hsb mob to local mob
    north_rx_msg[0].can_msg->data.u64 = can_get_mob_data(CAN_CH_NORTH, handle).u64;
    north_rx_msg[0].can_msg->id = can_get_mob_id(CAN_CH_NORTH, handle);
    north_rx_msg[0].dlc = can_get_mob_dlc(CAN_CH_NORTH, handle);
    north_rx_msg[0].status = event;
    
    //print what we got
    #if DBG_CAN_MSG
    print_dbg("\n\rReceived can message on NORTH line:\n\r");
    print_dbg_ulong(north_rx_msg[0].can_msg->data.u64);
    PRINT_NEWLINE
    #endif
    //release mob in hsb
    can_mob_free(CAN_CH_NORTH, handle);
    //set ready to evaluate message
    //TODO: state machine call
    message_received_north = true;
}
void can_out_callback_north_tx(U8 handle, U8 event){
    //TODO
    //stub
    #if 0
    print_dbg("\r\nNorth_CAN_msg\n\r");
    print_dbg_ulong(north_tx_msg[0].handle);
    print_dbg_ulong(north_tx_msg[0].can_msg->data.u64);
    #endif
    // Transmission Only
    can_mob_free(CAN_CH_NORTH,handle);
    message_transmitted_north = true;
}
void can_out_callback_south_rx(U8 handle, U8 event){
    //TODO
    //stub
    handle = CANIF_mob_get_mob_rxok(1) ;
    if (handle != 0x20)
    {
        CANIF_mob_clear_rxok_status(1,handle);
        CANIF_mob_clear_status(1,handle); //   and reset MOb status
    }
    event = CAN_STATUS_COMPLETED;
    
    //message has been received, move data from hsb mob to local mob
    south_rx_msg01.can_msg->data.u64 = can_get_mob_data(CAN_CH_SOUTH, handle).u64;
    south_rx_msg01.can_msg->id = can_get_mob_id(CAN_CH_SOUTH, handle);
    south_rx_msg01.dlc = can_get_mob_dlc(CAN_CH_SOUTH, handle);
    south_rx_msg01.status = event;
    
    //print what we got
    #if DBG_CAN_MSG
    print_dbg("\n\rReceived can message on SOUTH line:\n\r");
    //print_dbg_ulong(south_rx_msg01.can_msg->data.u64);
    PRINT_NEWLINE
    print_can_message(south_rx_msg01.can_msg);
    print_can_message(south_rx_msg02.can_msg);
    #endif
    //release mob in hsb
    can_mob_free(CAN_CH_SOUTH, handle);
    //set ready to evaluate message
    //TODO: state machine call
    message_received_south = true;
}
void can_out_callback_south_tx(U8 handle, U8 event){
    //TODO
    //stub
    handle = CANIF_mob_get_mob_txok(1) ;
    if (handle != 0x20)
    {
        CANIF_mob_clear_txok_status(1,handle);
        CANIF_mob_clear_status(1,handle); //   and reset MOb status
    }
    event = CAN_STATUS_COMPLETED;
    #if 1
    print_can_message(south_tx_msg[0].can_msg);
    #endif
    // Transmission Only
    can_mob_free(CAN_CH_SOUTH,handle);
    message_transmitted_south = true;
}
void can_prepare_data_to_send_north(void){
    //TODO
    //stub
    message_transmitted_north = false;
    //Init channel north
    can_init(CAN_CH_NORTH,
    ((U32)&CAN_MOB_SOUTH_RX_NORTH_TX[0]),
    CANIF_CHANNEL_MODE_NORMAL,
    can_out_callback_north_tx);
    //Allocate mob for TX
    north_tx_msg[0].handle = can_mob_alloc(CAN_CH_NORTH);
    
    //Check no mob available
    //if(south_tx_msg[0].handle==CAN_CMD_REFUSED){
    //
    //}
    //--example has conversion of data to meet adc standard from dsp lib.. not sure if necessary
    
    
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
    message_received_north = false;
    //Init channel north
    can_init(CAN_CH_NORTH,
    ((U32)&CAN_MOB_NORTH_RX_SOUTH_TX[0]),
    CANIF_CHANNEL_MODE_NORMAL,
    can_out_callback_north_rx);
    //Allocate mob for TX
    north_rx_msg[0].handle = can_mob_alloc(CAN_CH_NORTH);
    
    //Check no mob available
    //if(north_rx_msg[0].handle==CAN_CMD_REFUSED){
    //
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
    message_transmitted_south = false;
    //Init channel south
    can_init(CAN_CH_SOUTH,
    ((uint32_t)&CAN_MOB_NORTH_RX_SOUTH_TX),
    CANIF_CHANNEL_MODE_NORMAL,
    can_out_callback_south_tx);
    
    INTC_register_interrupt(&can_out_callback_south_rx, AVR32_CANIF_RXOK_IRQ_1, CAN1_INT_RX_LEVEL);
    INTC_register_interrupt(&can_out_callback_south_tx, AVR32_CANIF_TXOK_IRQ_1, CAN1_INT_TX_LEVEL);
    //CANIF_enable_interrupt(1);
    
    //Allocate mob for TX
    south_tx_msg[0].handle = can_mob_alloc(CAN_CH_SOUTH);
    south_rx_msg01.handle = can_mob_alloc(CAN_CH_SOUTH);
    south_rx_msg02.handle = can_mob_alloc(CAN_CH_SOUTH);
    
    //Check no mob available
    //if(south_tx_msg[0].handle==CAN_CMD_REFUSED){
    //
    //}
    //--example has conversion of data to meet adc standard from dsp lib.. not sure if necessary
    /* Check return if no mob are available */
    if (south_tx_msg[0].handle==CAN_CMD_REFUSED) {
        while(1);
    }

    can_tx(CAN_CH_SOUTH,
    south_tx_msg[0].handle,
    south_tx_msg[0].dlc,
    south_tx_msg[0].req_type,
    south_tx_msg[0].can_msg);
    
    can_rx(CAN_CH_SOUTH,
    south_rx_msg01.handle,
    south_rx_msg01.req_type,
    south_rx_msg01.can_msg);
    
    can_rx(CAN_CH_SOUTH,
    south_rx_msg02.handle,
    south_rx_msg02.req_type,
    south_rx_msg02.can_msg);
    
    //or ??
    //while(north_tx_msg[0].handle==CAN_CMD_REFUSED);
}

void can_prepare_data_to_receive_south(void){
    //TODO
    //stub//Init channel north
    message_received_south = false;
    
    can_init(CAN_CH_SOUTH,
    ((uint32_t)&CAN_MOB_SOUTH_RX_NORTH_TX),
    CANIF_CHANNEL_MODE_NORMAL,
    can_out_callback_south_rx);
    //Allocate mob for TX
    south_rx_msg01.handle = can_mob_alloc(CAN_CH_SOUTH);
    south_rx_msg02.handle = can_mob_alloc(CAN_CH_SOUTH);
    
    INTC_register_interrupt(&can_out_callback_south_rx, AVR32_CANIF_RXOK_IRQ_1, CAN1_INT_RX_LEVEL);
    INTC_register_interrupt(&can_out_callback_south_tx, AVR32_CANIF_TXOK_IRQ_1, CAN1_INT_TX_LEVEL);
    
    //Check no mob available
    //if(south_rx_msg01.handle==CAN_CMD_REFUSED || south_rx_msg02.handle==CAN_CMD_REFUSED){
    //while(1);
    //}
    //--example has conversion of data to meet adc standard from dsp lib.. not sure if necessary
    
    can_rx(CAN_CH_SOUTH,
    south_rx_msg01.handle,
    south_rx_msg01.req_type,
    south_rx_msg01.can_msg);
    
    can_rx(CAN_CH_SOUTH,
    south_rx_msg02.handle,
    south_rx_msg02.req_type,
    south_rx_msg02.can_msg);
    //or ??
    //while(north_tx_msg[0].handle==CAN_CMD_REFUSED);
}

void run_test_loop(void) {
    //function scratch area, will be rewritten as needed by the current test we are running
    //not great practice, used for rapid proto
    if (message_received_north == true)
    {
        can_prepare_data_to_receive_north();
        #if DBG_ON
        print_dbg("\n\rPrepared to receive north...\n\r");
        #endif
    }
    if (message_received_south == true)
    {
        can_prepare_data_to_receive_south();
        #if DBG_ON
        print_dbg("\n\rPrepared to receive south...\n\r");
        #endif
    }
    
    
    if (message_transmitted_north == true)
    {
        can_prepare_data_to_send_north();
        #if DBG_ON
        print_dbg("\n\rPrepared to send north...\n\r");
        #endif
    }
    if (message_transmitted_south == true)
    {
        can_prepare_data_to_send_south();
        #if DBG_ON
        print_dbg("\n\rPrepared to send south...\n\r");
        #endif
    }
    #if 0
    print_dbg_ulong((unsigned long)can_get_mob_id(CAN_CH_SOUTH, south_rx_msg01.handle));
    print_dbg("\n\r");
    print_dbg_ulong((unsigned long)can_get_mob_id(CAN_CH_SOUTH, south_rx_msg02.handle));
    print_dbg("\n\r");
    #endif
    //print_dbg_ulong(south_rx_msg[0].can_msg->data.u64);
    
    //can_prepare_data_to_send_north();
    //can_prepare_data_to_send_south();
    //can_prepare_data_to_send_north();
    delay_ms(1000);
    
}

void init(void) {
    /* Insert system clock initialization code here (sysclk_init()). */
    sysclk_init();
    
    board_init();
    
    //set flash wait state according to cpu
    flashc_set_flash_waitstate_and_readmode(sysclk_get_cpu_hz());
    
    //init debug printing for usart
    init_dbg_rs232(sysclk_get_pba_hz());
    //init_dbg_rs232(sysclk_get_cpu_hz());
    
    print_dbg("\r======INITIALIZED======\n\r");
    
    #if DBG_CLKS
    /* test return of clocks */
    print_dbg("\n\rMain Clock Freq\n\r");
    print_dbg_ulong(sysclk_get_main_hz());
    print_dbg("\n\rCPU Freq\n\r");
    print_dbg_ulong(sysclk_get_cpu_hz());
    #endif
}

void init_can(void) {
    /* Setup generic clock for CAN */
    /* Remember to calibrate this correctly to our external osc*/
    scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
    SCIF_GCCTRL_PBCCLOCK,
    AVR32_SCIF_GC_DIV_CLOCK,
    CANIF_OSC_DIV);

    #if DBG_CLKS
    print_dbg("\n\rGeneric clock setup\n\r");
    #endif

    /* Enable generic clock */
    scif_gc_enable(AVR32_SCIF_GCLK_CANIF);
    
    #if DBG_CLKS
    print_dbg("\n\rGeneric clock enabled\n\r");
    print_dbg_ulong(sysclk_get_peripheral_bus_hz(AVR32_CANIF_ADDRESS));
    #endif
    
    /* Disable all interrupts. */
    Disable_global_interrupt();

    /* Initialize interrupt vectors. */
    INTC_init_interrupts();
    
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
    
    /* Enable all interrupts. */
    Enable_global_interrupt();
}

#define member_size(type, member) sizeof(((type *)0)->member)

int main (void)
{
    //setup
    init();
    init_can();
    
    #if 0
    
    can_prepare_data_to_receive_south();
    //can_prepare_data_to_send_south();
    while (1)
    {
        run_test_loop();
    }
    
    #endif
    
    int size_rule = sizeof(rule_t);
    
    unsigned char payload_test = "040000FFFF01000000AAC10706050403020100FFFF0000000000010000";
    int size_payload = sizeof(payload_test);
    
    int size_key = sizeof(hmac_key);
    int size_hmac_single_8 = member_size(rule_prep_04_t, hmac);
    int size_prio = member_size(rule_t, prio);
    int size_hmac_array = member_size(rule_prep_05_t, hmac[0]) + member_size(rule_prep_05_t, hmac[1]) + member_size(rule_prep_05_t, hmac[2]);
    int size_rule_prep_frame = sizeof(rule_prep_05_t);
    
    int test_sha = sha2_self_test(0);
    
    bool test_new_rule = test_new_rule_creation();
    
    
    
    delay_ms(1000);
    
    //Special Case: New Rule Acquisition
    //Intercept CAN frame carrying New Rule payload
    //determine that mask and filter combination meets requirements to match New Rule Frame to New Rule Creation Rule (stored in flash)
    //Verify hardware shunt connect. Discard frame on fail.
    //Interpret frame (according to format)
    //According to Prio field, either:
    //If this is the first frame for this PREP_RULE received:
    //allocate memory for new working set for rule construction
    //copy frame data to working set
    //increment bitfield tracker for frame received
    //If this frame corresponds to a PREP_RULE construction in progress:
    //copy frame data to working set
    //increment bitfield tracker for frame received
    //If this frame is the STORE_RULE frame, evaluate against working set:
    //sequence value received must be incremented past stored value; if rule storage succeeds, increment stored value up
    //bitfield check for all necessary frames received to build rule
    //HMAC validation
    //use stored key to decrypt signature
    //signature payload should match rule data payload
    //Validation SUCCESS:
    //Copy relevant components of working set to Rule structure, save structure to flash
    //clear working set for rule
    //Validation FAIL:
    //clear working set memory for rule in progress
    //END (shunt disconnect detected):
    //clear working set memory for all rules in progress
    
    //
    //state: receiving rules --
    
    //wait for end while debugging
    
    sleep_mode_start();
    
    #if 0
    while(true){
        delay_ms(1000);
    }
    #endif

    
}
