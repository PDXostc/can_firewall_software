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

#define CAN_MSG_QUE_SIZE    16
#define CAN_MSG_QUE_NORTH_RX_SOUTH_TX_CHANNEL   0
#define CAN_MSG_QUE_SOUTH_RX_NORTH_TX_CHANNEL   1

// CAN MOB allocation into HSB RAM
#if defined (__GNUC__) && defined (__AVR32__)
volatile can_msg_t CAN_MOB_NORTH_RX_SOUTH_TX[NB_MOB_CHANNEL] __attribute__ ((__section__(".hsb_ram_loc")));
volatile can_msg_t CAN_MOB_SOUTH_RX_NORTH_TX[NB_MOB_CHANNEL] __attribute__ ((__section__(".hsb_ram_loc")));
volatile can_mob_t can_msg_que_north_rx_south_tx[CAN_MSG_QUE_SIZE] __attribute__ ((__section__(".hsb_ram_loc")));
volatile can_mob_t can_msg_que_south_rx_north_tx[CAN_MSG_QUE_SIZE] __attribute__ ((__section__(".hsb_ram_loc")));
#elif defined (__ICCAVR32__)
volatile __no_init can_msg_t CAN_MOB_NORTH_RX_SOUTH_TX[NB_MOB_CHANNEL] @0xA0000000;
volatile __no_init can_msg_t CAN_MOB_SOUTH_RX_NORTH_TX[NB_MOB_CHANNEL] @0xA0000000;
#endif

//Single channel rx/tx state switcher
enum State_Channel_t{
	WAIT, RX_INIT, RX, TX
};

enum State_Channel_t State_Channel = RX_INIT;

//SRAM Allocation for loaded filter rulesets
static rule_t can_ruleset_north_rx_south_tx[SIZE_RULESET];
static rule_t can_ruleset_south_rx_north_tx[SIZE_RULESET];

//physical security shunt, override to true during software testing
//if this is true, we can accept new rules
static bool detected_shunt = DETECTED_SHUNT;

//booleans for rx/tx
volatile bool message_received_north = false;
volatile bool message_received_south = false;
volatile bool message_transmitted_north = false;
volatile bool message_transmitted_south = false;

//ptrs to que, initialize to beginning
volatile can_mob_t *rx_s =   &can_msg_que_south_rx_north_tx[0];
volatile can_mob_t *proc_s = &can_msg_que_south_rx_north_tx[0];
volatile can_mob_t *tx_n =   &can_msg_que_south_rx_north_tx[0];

volatile can_mob_t *rx_n =   &can_msg_que_north_rx_south_tx[0];
volatile can_mob_t *proc_n = &can_msg_que_north_rx_south_tx[0];
/*volatile can_mob_t *tx_s =   &can_msg_que_north_rx_south_tx[0];*/

//single channel test, transmit lives on south..
volatile can_mob_t *tx_s =   &can_msg_que_south_rx_north_tx[0];

const enum Eval_t {
	DISCARD, NEW, FILTER
} Eval_t;

/* Call backs */
static void can_out_callback_north_rx(U8 handle, U8 event){
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
	message_received_north = true;
}

static void can_out_callback_south_tx(U8 handle, U8 event){
	//TODO
	//stub
	//     handle = CANIF_mob_get_mob_txok(1) ;
	//     if (handle != 0x20)
	//     {
	//         CANIF_mob_clear_txok_status(1,handle);
	//         CANIF_mob_clear_status(1,handle); //   and reset MOb status
	//     }
	//event = CAN_STATUS_COMPLETED;
	#if DBG_CAN_MSG
	print_dbg("\n\rTransmitted CAN Msg\n\r");
	print_can_message(tx_s->can_msg);
	#endif
	// Transmission Only
	can_mob_free(CAN_CH_SOUTH,handle);
	message_transmitted_south = true;
}

static void can_prepare_data_to_receive_north(void){
	//TODO
	//stub
	message_received_north = false;
	//Init channel north
	can_init(CAN_CH_NORTH,
	((U32)&CAN_MOB_NORTH_RX_SOUTH_TX),
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
//
static void can_prepare_data_to_send_south(void){
	//TODO
	//stub
	message_transmitted_south = false;
	//Init channel south
	can_init(CAN_CH_SOUTH,
	((uint32_t)&CAN_MOB_NORTH_RX_SOUTH_TX),
	CANIF_CHANNEL_MODE_NORMAL,
	can_out_callback_south_tx);

	//INTC_register_interrupt(&can_out_callback_south_rx, AVR32_CANIF_RXOK_IRQ_1, CAN1_INT_RX_LEVEL);
	//INTC_register_interrupt(&can_out_callback_south_tx, AVR32_CANIF_TXOK_IRQ_1, CAN1_INT_TX_LEVEL);

	//Allocate mob for TX
	tx_s->handle = can_mob_alloc(CAN_CH_SOUTH);
	/* Check return if no mob are available */
	while (tx_s->handle==CAN_CMD_REFUSED) {
		
	}

	can_tx(CAN_CH_SOUTH,
	tx_s->handle,
	tx_s->dlc,
	tx_s->req_type,
	tx_s->can_msg);

	//increment
	//Disable_global_interrupt();
	//advance ptr
	if(tx_s >= &can_msg_que_south_rx_north_tx[CAN_MSG_QUE_SIZE - 1])
	{
		tx_s = &can_msg_que_south_rx_north_tx[0];
		} else {
		tx_s = tx_s + 1;
	}
	
	State_Channel = RX_INIT;
	
	//Enable_global_interrupt();

}

static void can_out_callback_north_tx(U8 handle, U8 event){
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

static void can_prepare_data_to_send_north(void){
	//TODO
	//stub
	message_transmitted_north = false;
	//Init channel north
	can_init(CAN_CH_NORTH,
	((U32)&CAN_MOB_SOUTH_RX_NORTH_TX),
	CANIF_CHANNEL_MODE_NORMAL,
	can_out_callback_north_tx);

	north_tx_msg[0].handle = can_mob_alloc(CAN_CH_NORTH);
	/* Check return if no mob are available */
	if (north_tx_msg[0].handle==CAN_CMD_REFUSED) {
		while(1);
	}

	can_tx(CAN_CH_NORTH,
	north_tx_msg[0].handle,
	north_tx_msg[0].dlc,
	north_tx_msg[0].req_type,
	north_tx_msg[0].can_msg);
	
	//or ??
	//while(tx_n->handle==CAN_CMD_REFUSED);
}

static void can_out_callback_south_rx(U8 handle, U8 event){
	
	//Disable_global_interrupt();
	//inlining for now...
	//copy to location of desired rx ptr in queue
	rx_s->can_msg->data.u64 = can_get_mob_data(CAN_CH_SOUTH, handle).u64;
	rx_s->can_msg->id = can_get_mob_id(CAN_CH_SOUTH, handle);
	rx_s->dlc = can_get_mob_dlc(CAN_CH_SOUTH, handle);
	
	//print what we got
	#if DBG_CAN_MSG
	print_dbg("\n\rReceived can message on SOUTH line:\n\r");
	//print_dbg_ulong(south_rx_msg01.can_msg->data.u64);
	PRINT_NEWLINE
	print_can_message(rx_s->can_msg);
	//print_can_message(south_rx_msg02.can_msg);
	#endif
	
	//advance ptr
	if(rx_s >= &can_msg_que_south_rx_north_tx[CAN_MSG_QUE_SIZE - 1])
	{
		rx_s = &can_msg_que_south_rx_north_tx[0];
		} else {
		rx_s = rx_s + 1;
	}

	//release mob in hsb
	can_mob_free(CAN_CH_SOUTH, handle);
	//set ready to evaluate message
	//TODO: state machine call
	message_received_south = true;
	
	State_Channel = RX;
	
	//Enable_global_interrupt();
}

static void can_prepare_data_to_receive_south(void){
	//TODO
	//stub//Init channel north
	message_received_south = false;
	
	can_init(CAN_CH_SOUTH,
	((uint32_t)&CAN_MOB_SOUTH_RX_NORTH_TX),
	CANIF_CHANNEL_MODE_NORMAL,
	can_out_callback_south_rx);
	//Allocate mob for TX
	rx_s->handle = can_mob_alloc(CAN_CH_SOUTH);
	//south_rx_msg02.handle = can_mob_alloc(CAN_CH_SOUTH);
	
	//INTC_register_interrupt(&can_out_callback_south_rx, AVR32_CANIF_RXOK_IRQ_1, CAN1_INT_RX_LEVEL);
	//INTC_register_interrupt(&can_out_callback_south_tx, AVR32_CANIF_TXOK_IRQ_1, CAN1_INT_TX_LEVEL);
	
	//Check no mob available
	//if(south_rx_msg01.handle==CAN_CMD_REFUSED || south_rx_msg02.handle==CAN_CMD_REFUSED){
	//while(1);
	//}
	
	can_rx(CAN_CH_SOUTH,
	rx_s->handle,
	rx_s->req_type,
	&msg_pass_all);
	
	#if DBG_CAN_MSG
	print_dbg("\n\rCAN Init Receive Ready...");
	#endif
	
	State_Channel = WAIT;
}

static void can_prepare_next_receive_south(void)
{
	//Disable_global_interrupt();
	message_received_south = false;
	
	rx_s->handle = can_mob_alloc(CAN_CH_SOUTH);
	
	INTC_register_interrupt((void *)&can_out_callback_south_rx, AVR32_CANIF_RXOK_IRQ_1, CAN1_INT_RX_LEVEL);
	
	can_rx(CAN_CH_SOUTH,
	rx_s->handle,
	rx_s->req_type,
	rx_s->can_msg);
	
	#if DBG_CAN_MSG
	print_dbg("\n\rCAN Receive Ready...");
	#endif
	//Enable_global_interrupt();
}

#define member_size(type, member) sizeof(((type *)0)->member)

static inline enum Eval_t evaluate(volatile can_mob_t *msg, rule_t *ruleset, rule_t *out_rule){
	//note: does not handle extended CAN yet
	
	//if shunt connected, check against new rule case
	if(detected_shunt == true)
	{
		if(msg->can_msg->id == msg_new_rule.id){
			return NEW;
		}
	}
	
	int i = 0;
	while(i != SIZE_RULESET){
		//look for match
		//test of values:
		// 		int and_msg = msg->can_msg->id & ruleset[i].mask;
		// 		int filter = ruleset[i].filter;
		
		if((msg->can_msg->id & ruleset[i].mask) == ruleset[i].filter)
		{
			//match found, set out_rule and return evaluation case
			out_rule = &ruleset[i];
			return FILTER;
		}
		
		i += 1;
	}
	
	//got here without any match
	return DISCARD;
}

static inline void process(volatile can_mob_t **rx, volatile can_mob_t **proc, rule_t* ruleset, volatile can_mob_t *que)
{
	//check for each proc ptr to not equal the location we will copy to
	//process message pointed to
	//advance
	
	//south
	if (*proc == *rx)
	{
		return;
	}
	else
	{
		//evaluate against rules and handle accordingly
		//supply message and rule ptr, receive ptr to applicable rule
		//check rule
		rule_t *rule_match = NULL;
		enum Eval_t eval = evaluate(*proc, ruleset, rule_match);
		switch(eval) {
			case NEW:
			if(detected_shunt){
				//does not check for success
				handle_new_rule_data(&(*proc)->can_msg->data);
			}
			State_Channel = RX_INIT;
			break;
			
			case FILTER:
			//check for transform and rule conditions
			//apply rule to message, que for transmit
			
			
			
			//test of single channel que for transmit:
			
			State_Channel = TX;
			
			break;
			
			case DISCARD:
			default:
			//delete what is here
			memset((void *)(*proc), 0, sizeof(can_mob_t));
			State_Channel = RX_INIT;
			break;
		}
	}
	
	//Disable_global_interrupt();
	
	//advance ptr
	if(*proc >= &que[CAN_MSG_QUE_SIZE - 1])
	{
		*proc = &que[0];
		} else {
		*proc = *proc + 1;
	}
	
	//Enable_global_interrupt();
}

static inline void transmit(volatile can_mob_t **proc, volatile can_mob_t **tx, volatile can_mob_t *que, int tx_direction)
{
	if (*tx == *proc)
	{
		return;
	}
	else
	{
		//transmit, if applicable
		//remember to set dlc
		
		//tx test:
		//don't tx 00. anything else it finds is assumed to have been processed
		
		if ((*tx)->can_msg->id > 0)
		{
			if (tx_direction == 0)
			{
				//can_prepare_data_to_send_north();
			}
			else if(tx_direction == 1)
			{
				can_prepare_data_to_send_south();
			}
		}
		
	}
	//     //increment
	//     //Disable_global_interrupt();
	//     //advance ptr
	//     if(*tx >= &que[CAN_MSG_QUE_SIZE - 1])
	//     {
	//         *tx = &que[0];
	//         } else {
	//         *tx = *tx + 1;
	//     }
	//
	//     Enable_global_interrupt();
}

static void init(void) {
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	
	board_init();
	
	//set flash wait state according to cpu
	flashc_set_flash_waitstate_and_readmode(sysclk_get_cpu_hz());
	
	//init debug printing for usart
	init_dbg_rs232(sysclk_get_pba_hz());
	
	print_dbg("\r======INITIALIZED======\n\r");
	
	#if DBG_CLKS
	/* test return of clocks */
	print_dbg("\n\rMain Clock Freq\n\r");
	print_dbg_ulong(sysclk_get_main_hz());
	print_dbg("\n\rCPU Freq\n\r");
	print_dbg_ulong(sysclk_get_cpu_hz());
	print_dbg("\n\rPBA Freq\n\r");
	print_dbg_ulong(sysclk_get_pba_hz());
	#endif
}

static void init_can(void) {
	/* Setup generic clock for CAN */
	/* Remember to calibrate this correctly to our external osc*/
	int setup_gclk;
	#if 1
	setup_gclk = scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
	SCIF_GCCTRL_PLL0,
	AVR32_SCIF_GC_USES_PLL0,
	4);
	#elif 0
	setup_gclk= scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
	SCIF_GCCTRL_PBCCLOCK,
	AVR32_SCIF_GC_DIV_CLOCK,
	CANIF_OSC_DIV);
	#elif 0
	setup_gclk = scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
	SCIF_GCCTRL_OSC0,
	AVR32_SCIF_GC_NO_DIV_CLOCK,
	0);
	#endif

	#if DBG_CLKS
	if (setup_gclk ==0)
	{
		print_dbg("\n\rGeneric clock setup\n\r");
		}else {
		print_dbg("\n\rGeneric clock NOT SETUP\n\r");
	}
	
	#endif

	/* Enable generic clock */
	int enable_gclk = scif_gc_enable(AVR32_SCIF_GCLK_CANIF);
	#if DBG_CLKS
	if(enable_gclk == 0)
	{
		print_dbg("\n\rGeneric Clock Enabled");
	}
	#endif
	
	#if DBG_CLKS
	//print_dbg("\n\rGeneric clock enabled\n\r");
	print_dbg_ulong(sysclk_get_peripheral_bus_hz((const volatile void *)AVR32_CANIF_ADDRESS));
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

static void init_rules(void)
{
	//rules in flash are stored together.
	//Northbound[0 : SIZE_RULESET-1]
	//Southbound[SIZE_RULESET : (SIZERULESET*2)-1]
	load_ruleset(&flash_can_ruleset[0], can_ruleset_south_rx_north_tx, SIZE_RULESET);
	load_ruleset(&flash_can_ruleset[SIZE_RULESET], can_ruleset_north_rx_south_tx, SIZE_RULESET);
}

static inline void run_test_loop(void) {
	//function scratch area, will be rewritten as needed by the current test we are running
	//not great practice, used for rapid proto
	//     if (message_received_north == true)
	//     {
	// //        can_prepare_data_to_receive_north();
	//         #if DBG_ON
	//         print_dbg("\n\rPrepared to receive north...\n\r");
	//         #endif
	//     }
	if (message_received_south == true)
	{
		can_prepare_data_to_receive_south();
		//can_prepare_next_receive_south();
		#if DBG_ON
		print_dbg("\n\rPrepared to receive south...\n\r");
		#endif
	}
	
	
	//     if (message_transmitted_north == true)
	//     {
	//         can_prepare_data_to_send_north();
	//         #if DBG_ON
	//         print_dbg("\n\rPrepared to send north...\n\r");
	//         #endif
	//     }
	//
	//     if (message_transmitted_south == true)
	//     {
	//        can_prepare_data_to_send_south();
	//         #if DBG_ON
	//         print_dbg("\n\rPrepared to send south...\n\r");
	//         #endif
	//     }
	#if 0
	print_dbg_ulong((unsigned long)can_get_mob_id(CAN_CH_SOUTH, south_rx_msg01.handle));
	print_dbg("\n\r");
	print_dbg_ulong((unsigned long)can_get_mob_id(CAN_CH_SOUTH, south_rx_msg02.handle));
	print_dbg("\n\r");
	#endif
	
}

/* Main filter loop; designed to be a linear pipeline that we will try to get done as quickly as possible */
static inline void run_firewall(void)
{
	//maintain and move proc_ ptrs
	process(&rx_s, &proc_s, can_ruleset_south_rx_north_tx, can_msg_que_south_rx_north_tx);
	process(&rx_n, &proc_n, can_ruleset_north_rx_south_tx, can_msg_que_north_rx_south_tx);
	//maintain and move tx_ ptrs
	//     transmit(&proc_s, &tx_n, can_msg_que_south_rx_north_tx);
	//     transmit(&proc_n, &tx_s, can_msg_que_north_rx_south_tx);
}

/* Single channel usage of firewall */
static inline void run_firewall_single_channel(void)
{
	//Disable_global_interrupt();
	//first iteration just flips over when rx or tx
	switch(State_Channel)
	{
		case RX_INIT:
		can_prepare_data_to_receive_south();
		//State_Channel = RX;
		break;
		
		case RX:
		process(&rx_s, &proc_s, can_ruleset_south_rx_north_tx, can_msg_que_south_rx_north_tx);
		//State_Channel = TX;
		break;
		
		case TX:
		transmit(&proc_s, &tx_s, can_msg_que_south_rx_north_tx, CAN_CH_SOUTH);
		break;
		
		case WAIT:
		//wait for callback
		break;
	}
	//Enable_global_interrupt();
}

int main (void)
{
	//setup
	init();
	init_can();
	init_rules();

	//set rules in ruleset for testing
	//can_ruleset_south_rx_north_tx[0] = test_pass;
	for (int i = 0; i < SIZE_RULESET-1; i++)
	{
		can_ruleset_south_rx_north_tx[i] = test_block;
	}
	can_ruleset_south_rx_north_tx[15] = test_pass;

	//bool test_new_rule = test_new_rule_creation();
	//int size_can_msg = sizeof(can_msg_t);
	#if 1
	
	//can_prepare_data_to_receive_south();

	while (1)
	{
		//run_test_loop();
		//run_firewall();
		run_firewall_single_channel();
	}
	
	#endif
	
	delay_ms(1000);
	
	//wait for end while debugging
	
	sleep_mode_start();
	
	#if 0
	while(true){
		delay_ms(1000);
	}
	#endif

	
}
