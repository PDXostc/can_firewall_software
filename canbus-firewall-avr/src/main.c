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
#include "rules_test.h"
#include "filter.h"
#include "sleep.h"
#include "polarssl/sha2.h"
#include "led.h"
#include "loopback.h"
#include "mcp.h"
#include "interrupt_machines.h"

uint32_t clk_main, clk_cpu, clk_periph, clk_busa, clk_busb;

#define CAN_MSG_QUE_SIZE    16
#define CAN_MSG_QUE_NORTH_RX_SOUTH_TX_CHANNEL   0
#define CAN_MSG_QUE_SOUTH_RX_NORTH_TX_CHANNEL   1

#define DLC_LENGTH_STANDARD		8
#define DLC_LENGTH		DLC_LENGTH_STANDARD

// CAN MOB allocation into HSB RAM
#if defined (__GNUC__) && defined (__AVR32__)
volatile can_msg_t CAN_MOB_NORTH_RX_SOUTH_TX[NB_MOB_CHANNEL] __attribute__ ((__section__(".hsb_ram_loc")));
volatile can_msg_t CAN_MOB_SOUTH_RX_NORTH_TX[NB_MOB_CHANNEL] __attribute__ ((__section__(".hsb_ram_loc")));
volatile can_mob_t can_msg_que_north_rx_south_tx[CAN_MSG_QUE_SIZE] __attribute__ ((__section__(".hsb_ram_loc")));
volatile can_mob_t can_msg_que_south_rx_north_tx[CAN_MSG_QUE_SIZE] __attribute__ ((__section__(".hsb_ram_loc")));
#elif defined (__ICCAVR32__)
volatile __no_init can_msg_t CAN_MOB_NORTH_RX_SOUTH_TX[NB_MOB_CHANNEL] @0xA0000000;
volatile __no_init can_msg_t CAN_MOB_SOUTH_RX_NORTH_TX[NB_MOB_CHANNEL] @0xA0000000;
volatile __no_init can_mob_t can_msg_que_north_rx_south_tx[CAN_MSG_QUE_SIZE] @0xA0000000;
volatile __no_init can_mob_t can_msg_que_south_rx_north_tx[CAN_MSG_QUE_SIZE] @0xA0000000;
#endif

//SRAM Allocation for loaded filter rulesets
static rule_t can_ruleset_north_rx_south_tx[SIZE_RULESET];
static rule_t can_ruleset_south_rx_north_tx[SIZE_RULESET];

//ptrs to que, initialize to beginning
volatile can_mob_t *rx_s =   &can_msg_que_south_rx_north_tx[0];
volatile can_mob_t *proc_s = &can_msg_que_south_rx_north_tx[0];
volatile can_mob_t *tx_n =   &can_msg_que_south_rx_north_tx[0];

volatile can_mob_t *rx_n =   &can_msg_que_north_rx_south_tx[0];
volatile can_mob_t *proc_n = &can_msg_que_north_rx_south_tx[0];
volatile can_mob_t *tx_s =   &can_msg_que_north_rx_south_tx[0];

#define member_size(type, member) sizeof(((type *)0)->member)

//test of our rx_config struct
struct RX_config rx_config_default = {
	._RXM0 = 0x00000000,
	._RXF0 = 0x00000000,
	._RXF1 = 0x00000000,
	._RX0_EID = 0x00,
	._RXM1 = 0x00000000,
	._RXF2 = 0x00000000,
	._RXF3 = 0x00000000,
	._RXF4 = 0x00000000,
	._RXF5 = 0x00000000,
	._RX1_EID = (MCP_MASK_RXM1_EID |\
	MCP_MASK_RXF2_EID |\
	MCP_MASK_RXF3_EID |\
	MCP_MASK_RXF4_EID |\
	MCP_MASK_RXF5_EID),
	._RXB0_BUKT = MCP_VAL_BUKT_ROLLOVER_EN,
	._MCP_VAL_RX0_CTRL = MCP_VAL_RXM_STD_EXT,
	._MCP_VAL_RX1_CTRL = MCP_VAL_RXM_STD_EXT
};

struct RX_config rx_config_test_01 = {
	._RXM0 = 0x7FF,
	._RXF0 = 0x7FF,
	._RXF1 = 0x0A5,
	._RX0_EID = 0x00,
	._RXM1 = 0x1FFFFFFF,
	._RXF2 = 0x1FFFFFFF,
	._RXF3 = 0x1A5A5A5A,
	._RXF4 = 0x00000000,
	._RXF5 = 0x00000000,
	._RX1_EID = (MCP_MASK_RXM1_EID |\
	MCP_MASK_RXF2_EID |\
	MCP_MASK_RXF3_EID |\
	MCP_MASK_RXF4_EID |\
	MCP_MASK_RXF5_EID),
	//._RXB0_BUKT = MCP_VAL_BUKT_ROLLOVER_EN,
	._MCP_VAL_RX0_CTRL = MCP_VAL_RXM_STD_ONLY,
	._MCP_VAL_RX1_CTRL = MCP_VAL_RXM_EXT_ONLY
};

/* Utility wrapper for deleting an Atmel CAN message object
*/
static inline void wipe_mob(volatile can_mob_t **mob)
{
	memset((void *)(*mob), 0, sizeof(can_mob_t));
}

static void print_array_uint8(uint8_t *arr, int length)
{
	PRINT_NEWLINE()
	print_dbg("Array Values: ");
	for (int i = 0; i < length; i++)
	{
		print_dbg_char_hex(arr[i]);
		print_dbg(" | ");
	}
}

/* Process function to be deprecated. Shows handling of messages based on
* evaluation function included in filter
*/
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
		U32 xform = 0;
		int success = -1;
		
		enum Eval_t eval = evaluate(*proc, ruleset, &rule_match);
		
		switch(eval) {
			case NEW:
			if(test_loopback() == true){
				//does not check for success
				handle_new_rule_data(&(*proc)->can_msg->data);
			}
			break;
			
			case FILTER:
			//check for transform and rule conditions
			// switch on xform (once for each half byte)
			//apply rule to message, que for transmit
			//
			
			//operate on id, mask and shift to isolate upper half byte
			xform = (rule_match->xform & 0xF0) >> 4;
			success = operate_transform_id((*proc)->can_msg, &rule_match->idoperand, xform);
			if (success != 0)
			{
				wipe_mob(&(*proc));
				break;
			}
			
			//operate on data, mask to isolate lower half byte
			xform = rule_match->xform & 0x0F;
			success = operate_transform_u64(&(*proc)->can_msg->data.u64, &rule_match->dtoperand, xform);
			if (success != 0)
			{
				wipe_mob(&(*proc));
				break;
			}
			break;
			
			case DISCARD:
			default:
			//delete what is here
			wipe_mob(&(*proc));
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

/* Transmit function to be deprecated. Shows simple queue output logic.
*
*/
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
				//can_prepare_data_to_send_south();
			}
		}
	}
	
	//increment
	//advance ptr
	if(*tx >= &que[CAN_MSG_QUE_SIZE - 1])
	{
		*tx = &que[0];
		} else {
		*tx = *tx + 1;
	}
	
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

	init_led_gpio_ports();
	
}

static void init_rules(void)
{
	//rules in flash are stored together.
	//Northbound[0 : SIZE_RULESET-1]
	//Southbound[SIZE_RULESET : (SIZERULESET*2)-1]
	load_ruleset(&flash_can_ruleset[0], can_ruleset_south_rx_north_tx, SIZE_RULESET);
	load_ruleset(&flash_can_ruleset[SIZE_RULESET], can_ruleset_north_rx_south_tx, SIZE_RULESET);
	
	// remember to parse ruleset boundaries for MCP filter programming...
}

/* Main filter loop; designed to be a linear pipeline that we will try to get done as quickly as possible */
static inline void run_firewall(void)
{
	#if 0
	//maintain and move proc_ ptrs
	process(&rx_s, &proc_s, can_ruleset_south_rx_north_tx, can_msg_que_south_rx_north_tx);
	process(&rx_n, &proc_n, can_ruleset_north_rx_south_tx, can_msg_que_north_rx_south_tx);
	//maintain and move tx_ ptrs
	transmit(&proc_s, &tx_n, can_msg_que_south_rx_north_tx);
	transmit(&proc_n, &tx_s, can_msg_que_north_rx_south_tx);
	#endif
}

int main (void)
{
	//setup
	init();
	init_rules();
	
	// 	init_led_gpio_ports();
	set_led(LED_01, LED_ON);
	set_led(LED_02, LED_ON);
	set_led(LED_01, LED_OFF);
	set_led(LED_02, LED_OFF);
	
	// INIT MCP MODULE
	init_mcp_module();

	// INIT ALL INTERRUPTS AND INTERRUPT DRIVEN STATE MACHINES
	init_interrupt_machines();
	
	// Prior
	/************************************************************************/
	/* MCP CAN INIT                                                         */
	/************************************************************************/
	
// 	uint8_t init_success = 0xFF;
// 
// 	init_success = mcp_init_can(MCP_DEV_NORTH, MCP_VAL_CAN_1mbps_CLOCK_16Mhz, &rx_config_test_01, MCP_VAL_MODE_NORMAL);
// 	if (init_success != MCP_RETURN_SUCCESS)
// 	{
// 		print_dbg("\n\rInit FAIL NORTH");
// 	}
// 	init_success = mcp_init_can(MCP_DEV_SOUTH, MCP_VAL_CAN_1mbps_CLOCK_16Mhz, &rx_config_test_01, MCP_VAL_MODE_NORMAL);
// 	if (init_success != MCP_RETURN_SUCCESS)
// 	{
// 		print_dbg("\n\rInit FAIL SOUTH");
// 	}
		
	/* SETUP AND INITS COMPLETE. ENABLE ALL INTERRUPTS */
	Enable_global_interrupt();
	
	// GO!
	
	// Main loop should attempt to be idle when not running interrupt driven job
	
	
	/************************************************************************/
	/* old debugging section, delete when done                              */
	/************************************************************************/
#if 0
	nop();
	
	
	
	test_setup_transmit_mcp_can(MCP_NORTH);
	test_setup_transmit_mcp_can(MCP_SOUTH);
	
	uint8_t rx_test[MCP_CAN_MSG_SIZE] = {0};
	
	mcp_print_status(MCP_NORTH);
	mcp_print_status(MCP_SOUTH);
	
	mcp_print_txbnctrl(MCP_NORTH);
	
	mcp_print_error_registers(MCP_NORTH);

	mcp_print_txbnctrl(MCP_SOUTH);
	
	mcp_print_error_registers(MCP_SOUTH);
#endif

	while (1)
	{	
		//mcp_machine_int_set();	
		//run_firewall();
		nop();
	}
	
	delay_ms(1000);
	
	//wait for end while debugging
	
	sleep_mode_start();
}
