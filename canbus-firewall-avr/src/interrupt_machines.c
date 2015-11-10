/*
 * interrupt_machines.c
 *
 * Created: 11/5/2015 1:44:39 PM
 *  Author: smiller6
 */ 
#include "interrupt_machines.h"

volatile bool pdca_test_transfer_complete = false;

volatile struct MCP_status mcp_status = {
	.mcp_state = START,
	.status_byte_north = 0x00,
	.status_byte_south = 0x00,
	.error_byte_north = 0x00,
	.error_byte_south = 0x00,
	.jobs = 0x00000000,
	.attention = 0x00,
	.PDCA_busy = 0x00,
	};

// PDCA channel settings, uses Atmel convention struct
volatile pdca_channel_options_t PDCA_options_mcp_spi_msg_rx = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_MSG,
	.r_addr = NULL,
	.r_size = 0,
	};
	
volatile pdca_channel_options_t PDCA_options_mcp_spi_msg_tx = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_MSG,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_rx_single = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_SINGLE,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_tx_single = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_SINGLE,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_OPTIONS_rx_test = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = 0,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_OPTIONS_tx_test = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = 0,
	.r_addr = NULL,
	.r_size = 0,
};

// External interrupts set for low level trigger. Asynch mode allows wake on interrupt
void init_eic_options(void)
{
	// Enable level-triggered interrupt.
	eic_options[0].eic_mode   = EIC_MODE_LEVEL_TRIGGERED;
	// Interrupt will trigger on low-level.
	eic_options[0].eic_level  = EIC_LEVEL_LOW_LEVEL;
	// For Wake Up mode, initialize in asynchronous mode
	eic_options[0].eic_async  = EIC_ASYNCH_MODE;
	// Choose External Interrupt Controller Line
	eic_options[0].eic_line   = EXT_INT_IVI_LINE;
	
	
	eic_options[1].eic_mode   = EIC_MODE_LEVEL_TRIGGERED;
	// 	// Interrupt will trigger on low-level.
	eic_options[1].eic_level  = EIC_LEVEL_LOW_LEVEL;
	// 	// For Wake Up mode, initialize in asynchronous mode
	eic_options[1].eic_async  = EIC_ASYNCH_MODE;
	// 	// Choose External Interrupt Controller Line
	eic_options[1].eic_line   = EXT_INT_CAR_LINE;
}

// temp interrupt handler used for basic testing of mcp interrupt
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void mcp_interrupt_handler_north(void)
{
	volatile bool line01 = false;
	volatile bool line02 = false;
	line01 = gpio_get_pin_value(IVI_INT_PIN);
	line02 = gpio_get_pin_value(CAR_INT_PIN);
	//test clear mcp int flags for now
	
	//cheat, clear mcp flags slow
	mcp_set_register(MCP_DEV_NORTH, MCP_ADD_CANINTF, 0x00);
	
	//PDCA on interrupt test
	
		rx_instruction_test[0] = MCP_INST_READ_RX_0;
		// update pdca_options
		PDCA_OPTIONS_tx_test.addr = &rx_instruction_test;
		int test_size = sizeof(rx_instruction_test);
		PDCA_OPTIONS_tx_test.size = sizeof(rx_instruction_test);
		
		PDCA_OPTIONS_rx_test.addr = &rx_msg_test;
		test_size = sizeof(rx_msg_test);
		PDCA_OPTIONS_rx_test.size = sizeof(rx_msg_test);		
		
		pdca_init_channel(PDCA_CHANNEL_SPI_TX, &PDCA_OPTIONS_tx_test);
		pdca_init_channel(PDCA_CHANNEL_SPI_RX, &PDCA_OPTIONS_rx_test);
		
		//register interrupt for rx complete...
		/*INTC_register_interrupt(&pdca_transfer_complete_int_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);*/
		INTC_register_interrupt(&pdca_rx_transfer_complete_int_handler, AVR32_PDCA_IRQ_1, AVR32_INTC_INT1);;
		
		//pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);
		pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_RX);
		//
		
		//chip select mcp
		mcp_select(MCP_DEV_NORTH);
		
		pdca_enable(PDCA_CHANNEL_SPI_TX);
		pdca_enable(PDCA_CHANNEL_SPI_RX);
	
	// ask for mcp int status
	
// 	#if DBG_MCP
// 	// test: display status:
// 	mcp_print_status(MCP_NORTH);
// 	
// 	mcp_print_status(MCP_NORTH);
// 	print_dbg("\n\rCanSTAT REgister");
// 	mcp_print_registers(MCP_NORTH, MCP_ADD_CANSTAT, 1);
// 	print_dbg("\n\rCANINTE Register");
// 	mcp_print_registers(MCP_NORTH, MCP_ADD_CANINTE, 1);
// 	print_dbg("\n\rCANINTF Register");
// 	mcp_print_registers(MCP_NORTH, MCP_ADD_CANINTF, 1);
// 	#endif
// 	
// 	mcp_set_register(MCP_NORTH, MCP_ADD_CANINTF, 0x00);
	
	// analyze interrupt status byte and set flags...
	
	// if we choose to deal with mcp interrupts here, download/upload and
	// reset (bit modify) mcp interrupt flag registers
	//
	// clear external interrupt line
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_IVI_LINE);
}

// temp interrupt handler used for basic testing of mcp interrupt
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void mcp_interrupt_handler_south(void)
{
	volatile bool line01 = false;
	volatile bool line02 = false;
	line01 = gpio_get_pin_value(IVI_INT_PIN);
	line02 = gpio_get_pin_value(CAR_INT_PIN);
	
	//test clear mcp int flags for now
	mcp_set_register(MCP_DEV_SOUTH, MCP_ADD_CANINTF, 0x00);
	// clear external interrupt line
	// ask for mcp int status
	
	mcp_print_status(MCP_DEV_SOUTH);
	// test: display status:
	#if DBG_MCP
	
	mcp_print_status(MCP_DEV_SOUTH);
	print_dbg("\n\rCanSTAT REgister");
	mcp_print_registers(MCP_DEV_SOUTH, MCP_ADD_CANSTAT, 1);
	print_dbg("\n\rCANINTE Register");
	mcp_print_registers(MCP_DEV_SOUTH, MCP_ADD_CANINTE, 1);
	print_dbg("\n\rCANINTF Register");
	mcp_print_registers(MCP_DEV_SOUTH, MCP_ADD_CANINTF, 1);
	#endif
	
	// analyze interrupt status byte and set flags...
	// if we choose to deal with mcp interrupts here, download/upload and
	// reset (bit modify) mcp interrupt flag registers
	//
	// clear external interrupt line
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_CAR_LINE);
}

// Processing jobs interrupt handler
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void proc_int_handler(void)
{
	#if DBG_INT
	//test
	set_led(LED_02, LED_ON);
	print_dbg("\n\rProc_int_handler_called!");
	set_led(LED_02, LED_OFF);
	#endif
	// test sequence
	gpio_set_pin_high(PROC_INT_PIN);
	gpio_clear_pin_interrupt_flag(PROC_INT_PIN);
	
}

// MCP state machine interrupt handler
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void mcp_machine_int_handler(void)
{
	#if DBG_INT
	//test
	set_led(LED_01, LED_ON);
	print_dbg("\n\rMCP_machine_int_handler_called!");
	set_led(LED_01, LED_OFF);
	#endif
	
	// run state machine
	// if logic permits, loop will exit and set the pin high to wait for next interruption
	// example cases:
	//		waiting for PDCA transfer complete, 
	//		waiting for pending tx
	//		waiting for mcp external interrupt attention flag
	//		
	run_mcp_state_machine(&mcp_status);
	
	gpio_set_pin_high(MCP_MACHINE_INT_PIN);
	gpio_clear_pin_interrupt_flag(MCP_MACHINE_INT_PIN);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void pdca_rx_transfer_complete_int_handler(void)
{
	//handle transfer complete
	//pdca_test_transfer_complete = true;
	
	
	// Disable pdca interrupts now that transfer is complete
	volatile avr32_pdca_channel_t *pdca = pdca_get_handler(PDCA_CHANNEL_SPI_RX);
	pdca->idr = 0x07;

	pdca_disable(PDCA_CHANNEL_SPI_TX);
	pdca_disable(PDCA_CHANNEL_SPI_RX);
	
	// deselect mcp based on which we were busy with, assumes that the busy flag
	// has been set by the mcp machine which also started this job
	if (mcp_status.PDCA_busy == MCP_DIR_NORTH)
	{
		mcp_deselect(MCP_DEV_NORTH);		
	} 
	else
	{
		mcp_deselect(MCP_DEV_SOUTH);
	}
	
	// set not busy
	mcp_status.PDCA_busy = 0;
}

void init_interrupt_machines(void)
{
	/************************************************************************/
	/* Interrupts                                                           */
	/************************************************************************/
	
	Disable_global_interrupt();
	
	INTC_init_interrupts();
	
	// Setup Pin interrupts for MCP state machine and processing jobs
	gpio_configure_pin(MCP_MACHINE_INT_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(PROC_INT_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	/* For GPIO IRQ, the formula should be:
	 * (gpio_irq0 + gpio pin number/ eight )
	 * so PA05 = 0 and PA21 = 2...
	 */
	
	//MCP machine should run at two int levels above main
	INTC_register_interrupt(&mcp_machine_int_handler, AVR32_GPIO_IRQ_0, AVR32_INTC_INT1);
	//Proc should run at first int level
	INTC_register_interrupt(&proc_int_handler, AVR32_GPIO_IRQ_2, AVR32_INTC_INT0);
	
	gpio_enable_pin_interrupt(MCP_MACHINE_INT_PIN, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(PROC_INT_PIN, GPIO_FALLING_EDGE);	
	
	/************************************************************************/
	/* Setup Interrupts for MCP Using EIC                                   */
	/************************************************************************/	
	
	mcp_set_register(MCP_DEV_NORTH, MCP_ADD_CANINTE, MCP_VAL_INT_RX_TX_ENABLE);
	mcp_set_register(MCP_DEV_NORTH, MCP_ADD_CANINTF, 0x00);
	mcp_set_register(MCP_DEV_SOUTH, MCP_ADD_CANINTE, MCP_VAL_INT_RX_TX_ENABLE);
	mcp_set_register(MCP_DEV_SOUTH, MCP_ADD_CANINTF, 0x00);
	
	init_eic_options();	
	
	INTC_register_interrupt(&mcp_interrupt_handler_north, EXT_INT_IVI_IRQ, AVR32_INTC_INT2);
	INTC_register_interrupt(&mcp_interrupt_handler_south, EXT_INT_CAR_IRQ, AVR32_INTC_INT2);
	
	eic_init(&AVR32_EIC, eic_options, EXT_INT_NUM_LINES);
	
	eic_enable_line(&AVR32_EIC, EXT_INT_IVI_LINE);
	eic_enable_interrupt_line(&AVR32_EIC, EXT_INT_IVI_LINE);
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_IVI_LINE);
	
	eic_enable_line(&AVR32_EIC, EXT_INT_CAR_LINE);
	eic_enable_interrupt_line(&AVR32_EIC, EXT_INT_CAR_LINE);
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_CAR_LINE);
	
	/************************************************************************/
	/* Configure State Machine and Structs for First Run                    */
	/************************************************************************/
	
	// Set jobs to reset and program mcp chips
	//mcp_status.jobs = (JOB_RESET_NORTH | JOB_RESET_SOUTH | JOB_CONFIGURE_NORTH | JOB_CONFIGURE_SOUTH);
	mcp_stm_set_job(&mcp_status, (JOB_RESET_NORTH | JOB_RESET_SOUTH | JOB_CONFIGURE_NORTH | JOB_CONFIGURE_SOUTH));
	
}
/* MCP state machine
 * The MCP state machine should be run inside an interrupt handler, triggering 
 * a state execution sequence.
 * The basic role of the MCP state machine is to fall through states leading to
 * the execution of tasks necessary to complete jobs for evaluating and affecting
 * the MCP CAN controllers. 
 * Completion of these tasks depends on operating the PDCA, or peripheral direct
 * memory access controller on the Atmel chip. The machine should set an SPI transaction
 * in motion with the PDCA, then exit the loop and wait for a "callback" interrupt
 * when the transfer is complete.
 * It is expected that while the machine is triggered from a single interrupt,
 * the source of such an interrupt can be from multiple reasons, commonly:
 * -completion of a PDCA transfer
 * -external interrupt from an MCP device, notifying received or transferred message, or even an error
 * -processing job complete, triggering the need for evaluation of a message by the tx pointer in que
 * 
 * The machine cannot perform any meaningful operation without the SPI bus, so if the 
 * PDCA is busy using the bus, the state machine should probably exit. The PDCA
 * has the responsibility of clearing the busy flag and triggering the machine interrupt.
 */
void run_mcp_state_machine(struct MCP_status *status)
{
	// run the machine when this is called, if any state needs to exit the state loop
	// for a reason other than PDCA being busy, this gives the opportunity to break
	// out. This should always reset to true when this function is called by interrupt.
	volatile bool run_machine = true;
	
	// if the PDCA is free, run the machine
	while (status->PDCA_busy == 0 && run_machine == true)
	{
		//run machine
		switch (status->mcp_state)
		{
		case START:
		/* START case should be the default state. Other states should return here
		 * when tasks complete. START should send the machine on to initiate required
		 * jobs.
		 */
			// goto jobs if pending
			if (status->jobs > 0)
			{
				mcp_stm_set_state(status, JOB_START);
			} 
			// no jobs pending, but interupt was thrown, check for attention required status
			else if (status->attention > 0)
			{
				if ((status->attention & MCP_DIR_NORTH) == MCP_DIR_NORTH)
				{
					mcp_stm_set_job(status, JOB_GET_STATUS_NORTH);
				}
				
				if ((status->attention & MCP_DIR_SOUTH) == MCP_DIR_SOUTH)
				{
					mcp_stm_set_job(status, JOB_GET_STATUS_SOUTH);
				}
				
				mcp_stm_set_state(status, JOB_START);
			}
			break;
			
		case JOB_START:
		/* JOB_START is the main brancing off point for executing pending tasks.
		 * Pending jobs flags are checked and executed in order of priority. Jobs
		 * are exclusive; the state machine can have multiple jobs pending, but 
		 * will only execute one at a time and must finish execution before another
		 * can be started.
		 */			
			if ((status->jobs & JOB_RESET_NORTH) == JOB_RESET_NORTH) {
				mcp_stm_set_state(status, RESET_NORTH);
			}		
			else if ((status->jobs & JOB_RESET_SOUTH) == JOB_RESET_SOUTH) {
				mcp_stm_set_state(status, RESET_SOUTH);
			}
			else if ((status->jobs & JOB_CONFIGURE_NORTH) == JOB_CONFIGURE_NORTH) {
				mcp_stm_set_state(status, ENTER_CONFIG_MODE_NORTH);
			}
			else if ((status->jobs & JOB_CONFIGURE_SOUTH) == JOB_CONFIGURE_SOUTH) {
				mcp_stm_set_state(status, ENTER_CONGIG_MODE_SOUTH);
			}
			else if ((status->jobs & JOB_GET_STATUS_NORTH) == JOB_GET_STATUS_NORTH) {
				mcp_stm_set_state(status, GET_STATUS_NORTH);
			}
			else if ((status->jobs & JOB_GET_STATUS_SOUTH) == JOB_GET_STATUS_SOUTH) {
				mcp_stm_set_state(status, GET_STATUS_SOUTH);
			}
			else if ((status->jobs & JOB_GET_ERROR_REG_NORTH) == JOB_GET_ERROR_REG_NORTH) {
				mcp_stm_set_state(status, GET_ERROR_REG_NORTH);
			}
			else if ((status->jobs & JOB_GET_ERROR_REG_SOUTH) == JOB_GET_ERROR_REG_SOUTH) {
				mcp_stm_set_state(status, GET_ERROR_REG_SOUTH);
			}
			else if ((status->jobs & JOB_TX_PENDING) == JOB_TX_PENDING) {
				mcp_stm_set_state(status, TX_PENDING);
			}
			else if ((status->jobs & JOB_RX_0_NORTH) == JOB_RX_0_NORTH) {
				mcp_stm_set_state(status, READ_RX_0_NORTH);
			}
			else if ((status->jobs & JOB_RX_0_SOUTH) == JOB_RX_0_SOUTH) {
				mcp_stm_set_state(status, READ_RX_0_SOUTH);
			}
			else if ((status->jobs & JOB_RX_1_NORTH) == JOB_RX_1_NORTH) {
				mcp_stm_set_state(status, READ_RX_1_NORTH);
			}
			else if ((status->jobs & JOB_RX_1_SOUTH) == JOB_RX_1_SOUTH) {
				mcp_stm_set_state(status, READ_RX_1_SOUTH);
			}
			else if (status->jobs == JOB_NO_JOBS) {
				mcp_stm_set_state(status, NO_JOBS);
			}
			break;
		case NO_JOBS:
		case RESET_NORTH:
		case RESET_SOUTH:
		case ENTER_CONFIG_MODE_NORTH:
		case ENTER_CONGIG_MODE_SOUTH:
		case CONFIGURE_BIT_TIMINGS_NORTH:
		case CONFIGURE_BIT_TIMINGS_SOUTH:
		case CONFIGURE_RX_0_NORTH:
		case CONFIGURE_RX_1_NORTH:
		case CONFIGURE_RX_0_SOUTH:
		case CONFIGURE_RX_1_SOUTH:
		case CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_NORTH:
		case CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_SOUTH:
		case ENTER_NORMAL_MODE_NORTH:
		case ENTER_NORMAL_MODE_SOUTH:
		case GET_STATUS_NORTH:
		case GET_STATUS_SOUTH:
		case GET_STATUS_NORTH_CALLBACK:
		case GET_STATUS_SOUTH_CALLBACK:
		case GET_ERROR_REG_NORTH:
		case GET_ERROR_REG_SOUTH:
		case GET_ERROR_REG_NORTH_CALLBACK:
		case GET_ERROR_REG_SOUTH_CALLBACK:
		case TX_PENDING:
		case EVALUATE_TX_POINTER:
		case LOAD_TXB_0_NORTH:
		case LOAD_TXB_1_NORTH:
		case LOAD_TXB_2_NORTH:
		case LOAD_TXB_0_NORTH_CALLBACK:
		case LOAD_TXB_1_NORTH_CALLBACK:
		case LOAD_TXB_2_NORTH_CALLBACK:
		case LOAD_TXB_0_SOUTH:
		case LOAD_TXB_1_SOUTH:
		case LOAD_TXB_2_SOUTH:
		case LOAD_TXB_0_SOUTH_CALLBACK:
		case LOAD_TXB_1_SOUTH_CALLBACK:
		case LOAD_TXB_2_SOUTH_CALLBACK:
		case READ_RX_0_NORTH:
		case READ_RX_1_NORTH:
		case READ_RX_0_NORTH_CALLBACK:
		case READ_RX_1_NORTH_CALLBACK:
		case READ_RX_0_SOUTH:
		case READ_RX_1_SOUTH:
		case READ_RX_0_SOUTH_CALLBACK:
		case READ_RX_1_SOUTH_CALLBACK:
		default:
			break;
		}
	}
	//else get out and wait
	
}
