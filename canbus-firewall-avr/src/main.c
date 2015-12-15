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
#include "mcp_message_que.h"
#include "timestamp.h"

uint32_t clk_main, clk_cpu, clk_periph, clk_busa, clk_busb;

//SRAM Allocation for loaded filter rulesets
static rule_t can_ruleset_southbound[SIZE_RULESET];
static rule_t can_ruleset_northbound[SIZE_RULESET];

#define member_size(type, member) sizeof(((type *)0)->member)

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

// TEST function: load rules with worst case, where only a single rule applies at the end of storage
void load_rules_manual_test_single(rule_t *applicable_rule)
{
	// write 0xfff rule (inapplicable) to all slots
	for (int i = 0; i < SIZE_RULESET; i++)
	{
		can_ruleset_northbound[i] = rule_test_block;
		can_ruleset_southbound[i] = rule_test_block;
	}
	
	// set the specific rules we want, one for each at end of their respective sets
	can_ruleset_northbound[SIZE_RULESET - 1] = *applicable_rule;
	can_ruleset_southbound[SIZE_RULESET - 1] = *applicable_rule;
}

static void init_rules(void)
{
	//rules in flash are stored together.
	//Northbound[0 : SIZE_RULESET-1]
	//Southbound[SIZE_RULESET : (SIZERULESET*2)-1]
	load_ruleset(&flash_can_ruleset[0], can_ruleset_northbound, SIZE_RULESET);
	load_ruleset(&flash_can_ruleset[SIZE_RULESET], can_ruleset_southbound, SIZE_RULESET);
	
	/************************************************************************/
	/* test rules manually                                                  */
	/************************************************************************/
	//load_rules_manual_test_single(&rule_test_inside_range_xform_data_and);
	
	// TODO: remember to parse ruleset boundaries for MCP filter programming...
}

/* Process function.
 * Incoming message id is translated and compared against relevant rule set.
 * An evaluation / recommendation is returned for how the message is to be treated.
 * Message is then either ingested as a new rule (special case), filtered or discarded.
*/
static inline void process(volatile struct MCP_message_t **rx, volatile struct MCP_message_t **proc, rule_t* ruleset, volatile struct MCP_message_t *que)
{
	//check for each proc ptr to not equal the location we will copy to
	//process message pointed to
	//advance
	
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
		
		//U32 msg_id;
		// Uses temporary structure for storing the id of the message currently being evaluated.
		// This of course assumes that only one message is being fully processed at a time
		translate_id_mcp_to_U32((*proc)->msg, &Eval_temp.id);
		
		
		// evaluation function will compare the 32b id with the applicable ruleset.
		// if a match is found, the NEW or FILTER case will be returned, along with a
		// pointer to the relevant rule to be used for processing. Note that NEW
		// is the special case for building an incoming new rule, and requires 
		// intensive processing.
		enum Eval_t eval = evaluate_msg_id(Eval_temp.id, ruleset, &rule_match);
		
		switch(eval) {
			case NEW:
			if(test_loopback() == true){
				// data stored in MCP format must be moved to a 64b number for processing
				translate_data_mcp_to_U64((*proc)->msg, &Eval_temp.data.u64);
				// provide translated data to rule handler
				handle_new_rule_data(&Eval_temp.data);
				
			}
			break;
			
			case FILTER:
			//check for transform and rule conditions
			// switch on xform (once for each half byte)
			//apply rule to message, que for transmit
			//
			
			//operate on id, mask and shift to isolate upper half byte
			xform = (rule_match->xform & 0xF0) >> 4;
			
			// check for PASS condition early out
			if(xform != XFORM_PASS)
			{
				success = operate_transform_id(&Eval_temp.id, &rule_match->idoperand, xform);
				//copy altered id back to message
				translate_id_U32_to_mcp((*proc)->msg, &Eval_temp.id);
			} else
			{
				// id passed
				success = 0;
			}
			
			if (success != 0)
			{
				// message not successfully filtered, should be zeroed
				wipe_msg_id(&(*proc));
				break;
			}
			
			//operate on data, mask to isolate lower half byte
			xform = rule_match->xform & 0x0F;
			if (xform != XFORM_PASS)
			{
				// copy data to 64b number for operation.
				translate_data_mcp_to_U64((*proc)->msg, &Eval_temp.data.u64);
				
				// operate on number according to rule.
				success = operate_transform_u64(&Eval_temp.data.u64, &rule_match->dtoperand, xform);
				
				// copy number back to MCP format
				translate_data_U64_to_mcp((*proc)->msg, &Eval_temp.data.u64);
			}
			else
			{
				success = 0;
			}
			
			if (success != 0)
			{
				// data not successfully operated, message should be zeroed
				wipe_msg_id(&(*proc));
				break;
			}
			break;
			
			case DISCARD:
			default:
			// No rule found that applies to message, should be zeroed
			wipe_msg_id(&(*proc));
			break;
		}
	}
	
	// if processing pointer is not in the same position as the receive pointer, advance it
	if (*proc != *rx)
	{
		que_advance_ptr(proc);
	}
	
	// decrement the number of pending messages in need of processing
	PROC_status.proc_pending_count--;
	
	// increment the number of messages in need of transmission evaluation
	TX_status.tx_pending_count++;
	
	// set job for mcp to evaluate message for transmission
	SET_MCP_JOB(mcp_status.jobs, JOB_TX_PENDING);
	
	// if the last message pending appears to have been processed, clear the pin to switch off the loop
	if (PROC_status.proc_pending_count < 1)
	{
		proc_int_clear();
	}
	
}

/* Main filter loop; designed to be a linear pipeline that we will try to get done as quickly as possible */
static inline void run_firewall(void)
{
	while (!gpio_local_get_pin_value(PROC_INT_PIN))
	{
		#if DBG_TEST_THROUGHPUT_PROC
		test_set_received_message_for_transmit();
		#endif
		
		#if DBG_PROC
		print_dbg("Processing loop. Proc_pending_count: ");
		print_dbg_char_hex(PROC_status.proc_pending_count);
		#endif
		
		/************************************************************************/
		/* Evaluate and Filter                                                  */
		/************************************************************************/
		
		//process using applicable ruleset according to direction
		if(que_ptr_proc->direction == MCP_DIR_NORTH)
		{
			// direction is northbound, use south receive north transmit
			process(&que_ptr_rx, &que_ptr_proc, can_ruleset_northbound, mcp_message_que);
		}
		else if (que_ptr_proc->direction == MCP_DIR_SOUTH)
		{
			// direction is southbound, use north receive south transmit
			process(&que_ptr_rx, &que_ptr_proc, can_ruleset_southbound, mcp_message_que);
		}
		
		//call mcp interrupt, now that a message is ready
		mcp_machine_int_set();
	}
}

#if DBG_TRACE
#if (defined __GNUC__)
__attribute__((aligned(4)))
#elif (defined __ICCAVR32__)
#pragma data_alignment=4
#endif
volatile char __tracebuffer__[8192];

volatile int __tracebuffersize__ = sizeof(__tracebuffer__);

void InitTraceBuffer()
{
	int index = 0;
	for(index =0; index<8192; index++)
	{
		__tracebuffer__[index];
		__tracebuffersize__;
	}
}
#endif

int main (void)
{
	#if DBG_TRACE
	InitTraceBuffer();
	#endif
	
	//disable wdt if running
	wdt_disable();
	
	//setup
	init();
	init_rules();
	
	// 	init_led_gpio_ports();
	set_led(LED_01, LED_ON);
	set_led(LED_02, LED_ON);
	set_led(LED_01, LED_OFF);
	set_led(LED_02, LED_OFF);
	
	#if DBG_LED_USE_LED_LOOPBACK
		if (test_loopback() == true)
		{
			set_led(LED_02, LED_ON);
		} 
		else
		{
			set_led(LED_01, LED_ON);
		}
	#endif
	
	// INIT MCP MODULE
	init_mcp_module();
	
	// INIT MESSAGING QUE
	init_mcp_message_que();

	// INIT ALL INTERRUPTS AND INTERRUPT DRIVEN STATE MACHINES
	// This will also initiate the jobs for the MCP start and program
	init_interrupt_machines();
	
#if DBG_MSG_QUE
	// testing messages in que...
	//arrays to send
	volatile uint8_t test_arr_dec_01[13] = {0xff, 0xaa, 0xa5, 0x5a, 8, 8, 7, 6, 5, 4, 3, 2, 1};
	volatile uint8_t test_arr_dec_02[13] = {0xf0, 0xaa, 0xa5, 0x5a, 8, 8, 7, 6, 5, 4, 3, 2, 1};
	volatile uint8_t test_arr_dec_03[13] = {0xfa, 0xaa, 0xa5, 0x5a, 8, 8, 7, 6, 5, 4, 3, 2, 1};
	volatile uint8_t test_arr_inc[13] = {0xff, 0xaa, 0xa5, 0x5a, 8, 1, 2, 3, 4, 5, 6, 7, 8};
	volatile uint8_t test_mcp_format_inc_std_ff[13] = {0xff, 0xe0, 0x00, 0x00, 3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	volatile uint8_t test_mcp_format_inc_ext_ff[13] = {0xff, 0xe0, 0x00, 0x00, 3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	
	
	//create some test junk in que
	mcp_message_que[0].direction = MCP_DIR_NORTH;
	mcp_message_que[1].direction = MCP_DIR_SOUTH;
	mcp_message_que[2].direction = MCP_DIR_NORTH;
	mcp_message_que[3].direction = MCP_DIR_SOUTH;
	// mcp_message_que[2].direction = MCP_DIR_NORTH;
	// mcp_message_que[3].direction = MCP_DIR_NORTH;
	for (int i = 0; i < 13; i++)
	{
		mcp_message_que[0].msg[i] = test_arr_dec_01[i];
		mcp_message_que[1].msg[i] = test_arr_inc[i];
		mcp_message_que[2].msg[i] = test_mcp_format_inc_std_ff[i];
		mcp_message_que[3].msg[i] = test_mcp_format_inc_ext_ff[i];
		// mcp_message_que[2].msg[i] = test_arr_dec_03[i];
	}

	//make sure rx pointer starts out well ahead
	que_ptr_rx = &mcp_message_que[4];
	
	//proc pointer to 3. should not transmit because proc pointer is here
	que_ptr_proc = &mcp_message_que[4];
	
	// set tx increment to num jobs we should try to do, also, we see if tx will overrun proc when it should not
	TX_status.tx_pending_count = 4;
	
	// set tx pending job
	SET_MCP_JOB(mcp_status.jobs, JOB_TX_PENDING);
	
#endif // if DBG_MSG_QUE
	
	/* SETUP AND INITS COMPLETE. ENABLE ALL INTERRUPTS */
	set_timestamp("start", Get_sys_count());
	
	Enable_global_interrupt();
	
	// GO!
	
	// Main loop should attempt to be idle when not running interrupt driven job,
	// or filtering
	while (1)
	{
		run_firewall();

		sleep_mode_start();
	}
}
