/*
 * interrupt_machines.c
 *
 * Created: 11/5/2015 1:44:39 PM
 *  Author: smiller6
 */ 
#include "interrupt_machines.h"

volatile bool pdca_test_transfer_complete = false;

volatile struct MCP_status_t mcp_status = {
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
pdca_channel_options_t PDCA_options_mcp_spi_msg_rx = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_MSG,
	.r_addr = NULL,
	.r_size = 0,
	};
	
pdca_channel_options_t PDCA_options_mcp_spi_msg_tx = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_MSG,
	.r_addr = NULL,
	.r_size = 0,
};

pdca_channel_options_t PDCA_options_mcp_spi_rx_single = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_SINGLE_INST,
	.r_addr = NULL,
	.r_size = 0,
};

pdca_channel_options_t PDCA_options_mcp_spi_tx_single = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_SINGLE_INST,
	.r_addr = NULL,
	.r_size = 0,
};

pdca_channel_options_t PDCA_options_mcp_spi_tx_write_single = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_write_single,
	.size = PDCA_SIZE_TRANS_WRITE_SINGLE,
	.r_addr = NULL,
	.r_size = 0,
};

pdca_channel_options_t PDCA_options_mcp_spi_tx_bit_modify = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_bit_modify_tx,
	.size = PDCA_SIZE_TRANS_BIT_MODIFY,
	.r_addr = NULL,
	.r_size = 0,
};

pdca_channel_options_t PDCA_options_mcp_spi_tx_configure_timings = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_TIMING,
	.r_addr = NULL,
	.r_size = 0,
};

pdca_channel_options_t PDCA_OPTIONS_rx_test = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = 0,
	.r_addr = NULL,
	.r_size = 0,
};

pdca_channel_options_t PDCA_OPTIONS_tx_test = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = 0,
	.r_addr = NULL,
	.r_size = 0,
};

// External interrupts set for low level trigger. Asynch mode allows wake on interrupt
uint8_t init_eic_options(void)
{
	// Enable level-triggered interrupt.
	eic_options[0].eic_mode   = EIC_MODE_EDGE_TRIGGERED;
	// Interrupt will trigger on low-level.
	//eic_options[0].eic_level  = EIC_LEVEL_LOW_LEVEL;
	// Interrupt triggered on falling edge
	eic_options[0].eic_edge = EIC_EDGE_FALLING_EDGE;
	// For Wake Up mode, initialize in asynchronous mode
	eic_options[0].eic_async  = EIC_ASYNCH_MODE;
	// Choose External Interrupt Controller Line
	eic_options[0].eic_line   = EXT_INT_IVI_LINE;
	
	
	eic_options[1].eic_mode   = EIC_MODE_EDGE_TRIGGERED;
	// 	// Interrupt will trigger on low-level.
	//eic_options[1].eic_level  = EIC_LEVEL_LOW_LEVEL;
	// Interrupt triggered on falling edge
	eic_options[1].eic_edge = EIC_EDGE_FALLING_EDGE;
	// 	// For Wake Up mode, initialize in asynchronous mode
	eic_options[1].eic_async  = EIC_ASYNCH_MODE;
	// 	// Choose External Interrupt Controller Line
	eic_options[1].eic_line   = EXT_INT_CAR_LINE;
	
	//for now,
	return 0x00;
}

// Initialize the MCP status structure
uint8_t init_mcp_status(void)
{
	mcp_status.mcp_state = START;
	
	uint8_t result = MCP_RETURN_FAIL;
	uint8_t cnf1, cnf2, cnf3;
	
	switch(MCP_DEFAULT_CAN_RATE_NORTH)
	{
		case MCP_VAL_CAN_500kbps_CLOCK_16Mhz:
		cnf1 = MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF1;
		cnf2 = MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF2;
		cnf3 = MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF3;
		
		result = MCP_RETURN_SUCCESS;
		break;
		
		case MCP_VAL_CAN_1mbps_CLOCK_16Mhz:
		cnf1 = MCP_VAL_CAN_1mbps_CLOCK_16Mhz_CNF1;
		cnf2 = MCP_VAL_CAN_1mbps_CLOCK_16Mhz_CNF2;
		cnf3 = MCP_VAL_CAN_1mbps_CLOCK_16Mhz_CNF3;
		
		result = MCP_RETURN_SUCCESS;
		break;
		
		case MCP_VAL_CAN_125kbps_CLOCK_16Mhz:
		cnf1 = MCP_VAL_CAN_125kbps_CLOCK_16Mhz_CNF1;
		cnf2 = MCP_VAL_CAN_125kbps_CLOCK_16Mhz_CNF2;
		cnf3 = MCP_VAL_CAN_125kbps_CLOCK_16Mhz_CNF3;
		
		result = MCP_RETURN_SUCCESS;
		break;
		
	}
	//found settings match, set MCP bit timings
	if (result == MCP_RETURN_SUCCESS)
	{
		mcp_status.timings_north[0] = MCP_INST_WRITE;
		mcp_status.timings_north[1] = MCP_ADD_CNF3;
		mcp_status.timings_north[2] = cnf3;
		mcp_status.timings_north[3] = cnf2;
		mcp_status.timings_north[4] = cnf1;
	}
	
	switch(MCP_DEFAULT_CAN_RATE_SOUTH)
	{
		case MCP_VAL_CAN_500kbps_CLOCK_16Mhz:
		cnf1 = MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF1;
		cnf2 = MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF2;
		cnf3 = MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF3;
		
		result = MCP_RETURN_SUCCESS;
		break;
		
		case MCP_VAL_CAN_1mbps_CLOCK_16Mhz:
		cnf1 = MCP_VAL_CAN_1mbps_CLOCK_16Mhz_CNF1;
		cnf2 = MCP_VAL_CAN_1mbps_CLOCK_16Mhz_CNF2;
		cnf3 = MCP_VAL_CAN_1mbps_CLOCK_16Mhz_CNF3;
		
		result = MCP_RETURN_SUCCESS;
		break;
		
		case MCP_VAL_CAN_125kbps_CLOCK_16Mhz:
		cnf1 = MCP_VAL_CAN_125kbps_CLOCK_16Mhz_CNF1;
		cnf2 = MCP_VAL_CAN_125kbps_CLOCK_16Mhz_CNF2;
		cnf3 = MCP_VAL_CAN_125kbps_CLOCK_16Mhz_CNF3;
		
		result = MCP_RETURN_SUCCESS;
		break;
		
	}
	
	//found settings match, set MCP bit timings
	if (result == MCP_RETURN_SUCCESS)
	{
		mcp_status.timings_south[0] = MCP_INST_WRITE;
		mcp_status.timings_south[1] = MCP_ADD_CNF3;
		mcp_status.timings_south[2] = cnf3;
		mcp_status.timings_south[3] = cnf2;
		mcp_status.timings_south[4] = cnf1;
	}
	
	return result;
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
	//mcp_set_register(MCP_DEV_NORTH, MCP_ADD_CANINTF, 0x00);
	
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
	Disable_global_interrupt();
	// run state machine
	// if logic permits, loop will exit and set the pin high to wait for next interruption
	// example cases:
	//		waiting for PDCA transfer complete, 
	//		waiting for pending tx
	//		waiting for mcp external interrupt attention flag
	//		
	run_mcp_state_machine(&mcp_status);
	
// 	gpio_set_pin_high(MCP_MACHINE_INT_PIN);
// 	gpio_clear_pin_interrupt_flag(MCP_MACHINE_INT_PIN);
// 	
	nop();
	mcp_machine_int_clear();
	Enable_global_interrupt();
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void pdca_rx_transfer_complete_int_handler(void)
{
	//handle transfer complete
	//pdca_test_transfer_complete = true;
	
	
	// Disable pdca interrupts now that transfer is complete
// 	volatile avr32_pdca_channel_t *pdca = pdca_get_handler(PDCA_CHANNEL_SPI_RX);
// 	pdca->idr = 0x07;

	pdca_disable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);
	pdca_disable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_RX);

	pdca_disable(PDCA_CHANNEL_SPI_TX);
	pdca_disable(PDCA_CHANNEL_SPI_RX);
	
	// deselect mcp based on which we were busy with, assumes that the busy flag
	// has been set by the mcp machine which also started this job
	if (mcp_status.PDCA_busy == MCP_DIR_NORTH)
	{
		mcp_deselect(MCP_DEV_NORTH);		
	} 
	else if (mcp_status.PDCA_busy == MCP_DIR_SOUTH)
	{
		mcp_deselect(MCP_DEV_SOUTH);
	}
	
	// set not busy
	mcp_status.PDCA_busy = 0;
	
	//set the mcp interrupt
	mcp_machine_int_set();
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void pdca_tx_transfer_complete_int_handler(void)
{
	//handle transfer complete
	//pdca_test_transfer_complete = true;
	
	
	// Disable pdca interrupts now that transfer is complete
// 	volatile avr32_pdca_channel_t *pdca = pdca_get_handler(PDCA_CHANNEL_SPI_TX);
// 	pdca->idr = 0x07;

	pdca_disable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);

	pdca_disable(PDCA_CHANNEL_SPI_TX);
	
	// deselect mcp based on which we were busy with, assumes that the busy flag
	// has been set by the mcp machine which also started this job
	if (mcp_status.PDCA_busy == MCP_DIR_NORTH)
	{
		mcp_deselect(MCP_DEV_NORTH);
	}
	else if (mcp_status.PDCA_busy == MCP_DIR_SOUTH)
	{
		mcp_deselect(MCP_DEV_SOUTH);
	}
	
	// set not busy
	mcp_status.PDCA_busy = 0;
	
	//set the mcp interrupt
	mcp_machine_int_set();
}

// Set up a job for receiving response data over SPI
// Address must be set in options before calling, and buffer to send must be primed
// with instruction. Sending buffer must include dummy information to facilitate 
// response byte(s) sent by MCP over SPI
// 
void PDCA_set_job_rx(struct spi_device *device, 
						 pdca_channel_options_t *options_tx,
						 pdca_channel_options_t *options_rx,
						 uint8_t pdca_busy_flag
						 )
	{
		
		// set pdca busy
		mcp_status.PDCA_busy = pdca_busy_flag;
		
		//initialize channels with provided options
		pdca_init_channel(PDCA_CHANNEL_SPI_TX, options_tx);
		pdca_init_channel(PDCA_CHANNEL_SPI_RX, options_rx);
		
		// register the interrupt for receive transfer complete. IRQ 1 corresponds to the SPI_RX channel,
		// INT level 1 is used so that interrupt resides on same level as the MCP state machine, able 
		// to interrupt the Main loop or Processing handler
		INTC_register_interrupt(&pdca_rx_transfer_complete_int_handler, AVR32_PDCA_IRQ_1, INT_LEVEL_PDCA);
		
		// enable the interrupt for receive transfer complete
		pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_RX);
		
		// select the device associated with this job
		mcp_select(device);
		
		//enable both channels for transfer
		pdca_enable(PDCA_CHANNEL_SPI_TX);
		pdca_enable(PDCA_CHANNEL_SPI_RX);
	}


// Set up a job for transmitting data over SPI
// Address must be set in options before calling, sending buffer must contain a valid
// instruction. This function does not care about the received data; therefore it 
// calls back on transmit complete and does not init the PDCA_SPI_RX channel
// 
void PDCA_set_job_tx(struct spi_device *device,
					 pdca_channel_options_t *options_tx,
					 uint8_t pdca_busy_flag
					 )
	{
		// set pdca busy
		mcp_status.PDCA_busy = pdca_busy_flag;
		
		// initialize tx channel with options
		pdca_init_channel(PDCA_CHANNEL_SPI_TX, options_tx);
		
		// register the interrupt for transmit complete
		INTC_register_interrupt(&pdca_tx_transfer_complete_int_handler, AVR32_PDCA_IRQ_0, INT_LEVEL_PDCA);
		
		// enable interrupt for transmission complete
		pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);
		
		// select the device associated with this job
		mcp_select(device);
		
		//enable the transmit channel
		pdca_enable(PDCA_CHANNEL_SPI_TX);
	}

void init_interrupt_machines(void)
{
	//init the mcp status
	uint8_t success  = init_mcp_status();
	
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
	INTC_register_interrupt(&mcp_machine_int_handler, AVR32_GPIO_IRQ_0, INT_LEVEL_MCP_MACHINE);
	//Proc should run at first int level
	INTC_register_interrupt(&proc_int_handler, AVR32_GPIO_IRQ_2, INT_LEVEL_PROC);
	
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
	
	INTC_register_interrupt(&mcp_interrupt_handler_north, EXT_INT_IVI_IRQ, INT_LEVEL_MCP_EIC);
	INTC_register_interrupt(&mcp_interrupt_handler_south, EXT_INT_CAR_IRQ, INT_LEVEL_MCP_EIC);
	
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
	
	//trigger mcp state machine for first run
	//set the mcp interrupt
	mcp_machine_int_set();
	
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
void run_mcp_state_machine(volatile struct MCP_status_t *status)
{
	// run the machine when this is called, if any state needs to exit the state loop
	// for a reason other than PDCA being busy, this gives the opportunity to break
	// out. This should always reset to true when this function is called by interrupt.
	volatile bool run_machine = true;
	#if DBG_MCP_STATE
	print_dbg("\n\r...Run state machine called...");
	#endif
	
	// if the PDCA is free, run the machine
	while ((status->PDCA_busy == 0) && (run_machine == true))
	{
		#if DBG_MCP_STATE
		print_dbg("\n\r...Entered while loop of machine...");
		#endif
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
			else {
				// no jobs or attention required, cease the machine
				run_machine = false;
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
				mcp_stm_set_state(status, ENTER_CONFIG_MODE_SOUTH);
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
		/* NO_JOBS is the fall through case for job initiation. If there are no jobs
		 * the machine should go back to the start.
		 */
			mcp_stm_set_state(status, START);
			
			break;
			
		case RESET_NORTH:
		/* RESET_ case requires that either:
		 *		the pin to drive the reset on an MCP chip is held low, or
		 *		the RESET command is sent over SPI, as a PDCA job
		 *	It is probably faster to wait a few cycles holding the pin, but conceivable
		 *	that in the future a reset case required at run time could benefit from
		 *	sceduling the reset as a PDCA job.
		 */
			mcp_reset_pin(MCP_DEV_NORTH);
			
			//if using pin method, set job complete in this state
			mcp_stm_unset_job(status, JOB_RESET_NORTH);
			
			mcp_stm_set_state(status, START);
			
			break;
			
		case RESET_SOUTH:
		
			mcp_reset_pin(MCP_DEV_SOUTH);
		
			//if using pin method, set job complete in this state
			mcp_stm_unset_job(status, JOB_RESET_SOUTH);
		
			mcp_stm_set_state(status, START);
		
			break;
		
		case ENTER_CONFIG_MODE_NORTH:
		/* ENTER_CONFIG_MODE_ is the first state in the mcp configuration job sequence.
		 * Multiple registers will be configured in this sequence, requiring multiple
		 * PDCA jobs. The callback for each state of the sequence should simply be
		 * the next state in the sequence, as no data needs to be received for a
		 * configuration job.
		 */
			// set up for configuration mode instruction:
			// uses MCP bit modify instruction
			// this could be prepared in advance instead
			PDCA_temporary_bit_modify_tx[0] = MCP_INST_BIT_MODIFY;
			PDCA_temporary_bit_modify_tx[1] = MCP_ADD_CANCTRL;
			PDCA_temporary_bit_modify_tx[2] = MCP_MASK_CTRL_MODE;
			PDCA_temporary_bit_modify_tx[3] = MCP_VAL_MODE_CONFIG;
			
			// set PDCA to send configuration information over SPI
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_bit_modify, MCP_DIR_NORTH);
			
			// next state when called back will be to configure the mcp can timing
			mcp_stm_set_state(status, CONFIGURE_BIT_TIMINGS_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rEnter Config Mode North Job, Next is Configure Bit Timings North");
			#endif
			
			break;
			
		case ENTER_CONFIG_MODE_SOUTH:
			// set up for configuration mode instruction:
			// uses MCP bit modify instruction
			// this could be prepared in advance instead
			PDCA_temporary_bit_modify_tx[0] = MCP_INST_BIT_MODIFY;
			PDCA_temporary_bit_modify_tx[1] = MCP_ADD_CANCTRL;
			PDCA_temporary_bit_modify_tx[2] = MCP_MASK_CTRL_MODE;
			PDCA_temporary_bit_modify_tx[3] = MCP_VAL_MODE_CONFIG;
			
			// set PDCA to send configuration information over SPI
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_bit_modify, MCP_DIR_SOUTH);
			
			// next state when called back will be to configure the mcp can timing
			mcp_stm_set_state(status, CONFIGURE_BIT_TIMINGS_SOUTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rEnter Config Mode South Job, Next is Configure Bit Timings South");
			#endif
			
			break;
			
		case CONFIGURE_BIT_TIMINGS_NORTH:
		/* CONFIGURE_BIT_TIMINGS_ uses the preconfigured bit timings set up in the 
		 * MCP status structure. A job is scheduled with the PDCA for a consecutive
		 * write of registers on the MCP corresponding to CNF3, CNF2, CNF1
		 */
			// must set up the options with the desired timing information buffer. 
			// should contain instruction, address and consecutive configuration
			// information
			PDCA_options_mcp_spi_tx_configure_timings.addr = &status->timings_north;
			
			// set job with provided settings
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_configure_timings, MCP_DIR_NORTH);
			
			// next state will be to configure the first receive buffer
			mcp_stm_set_state(status, CONFIGURE_RX_0_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rConfigure Bit Timings North Job, Next is Configure RX0 North");
			#endif
			
			break;
			
		case CONFIGURE_BIT_TIMINGS_SOUTH:
			// must set up the options with the desired timing information buffer.
			// should contain instruction, address and consecutive configuration
			// information
			PDCA_options_mcp_spi_tx_configure_timings.addr = &status->timings_south;
			
			// set job with provided settings
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_configure_timings, MCP_DIR_SOUTH);
			
			// next state will be to configure the first receive buffer
			mcp_stm_set_state(status, CONFIGURE_RX_0_SOUTH);
			
			break;
			
		case CONFIGURE_RX_0_NORTH:
		/* CONFIGURE_RX is part of the start up and programming sequence. Configuration
		 * of the MCP receive buffers can only be done in Configuration Mode;
		 */
			//TODO: Use PDCA for proper job, requiring more states to be inserted
			// in the machine
			//FOR NOW: Configuring the Receive buffers is a start up action, during
			// which the MCP devices are not active. The Micro has no other tasks
			// at this time, so it is acceptable to use the slower waiting interface
			// for the MCPs
			// 
			mcp_configure_rx_0(MCP_DEV_NORTH, status->rx_config_north);
			
			// next state configures the second receive buffer
			mcp_stm_set_state(status, CONFIGURE_RX_1_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rConfigure_RX0 North Job, Next is Configure RX1 North");
			#endif
			
			break;
			
		case CONFIGURE_RX_1_NORTH:
			mcp_configure_rx_1(MCP_DEV_NORTH, status->rx_config_north);
			
			// next state is to configure the ready to send pins on MCP
			mcp_stm_set_state(status, CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rConfigure_RX1 North Job, Next is Configure Ready To Send Pins");
			#endif
			
			break;
			
		case CONFIGURE_RX_0_SOUTH:
			//TODO: Use PDCA for proper job, requiring more states to be inserted
			// in the machine
			//FOR NOW: Configuring the Receive buffers is a start up action, during
			// which the MCP devices are not active. The Micro has no other tasks
			// at this time, so it is acceptable to use the slower waiting interface
			// for the MCPs
			//
			mcp_configure_rx_0(MCP_DEV_SOUTH, status->rx_config_south);
		
			// next state configures the second receive buffer
			mcp_stm_set_state(status, CONFIGURE_RX_1_SOUTH);
		
			break;
		case CONFIGURE_RX_1_SOUTH:
			mcp_configure_rx_1(MCP_DEV_SOUTH, status->rx_config_south);
					
			// next state is to configure the ready to send pins on MCP
			mcp_stm_set_state(status, CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_SOUTH);
					
			break;
			
		case CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_NORTH:
		/* CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_ is a simple but necessary operation
		 * in the programming phase of initializing MCP controllers. The RTS pins
		 * in our case should be set to digital output so that the transmit buffer 
		 * conditions are not controlled by the exterior pins on the MCP
		 */
			PDCA_temporary_write_single[0] = MCP_INST_WRITE;
			PDCA_temporary_write_single[1] = MCP_ADD_TXRTSCTRL;
			PDCA_temporary_write_single[2] = MCP_VAL_TXRTS_DIGITAL_INPUT;
			
			// do PDCA job
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_write_single, MCP_DIR_NORTH);
			
			// next state should be configuration complete, set mcp to normal operation mode
			mcp_stm_set_state(status, ENTER_NORMAL_MODE_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rConfigure Ready To Send Pins North Job, Next is Enter Normal Mode North");
			#endif
			
			break;
			
		case CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_SOUTH:
			PDCA_temporary_write_single[0] = MCP_INST_WRITE;
			PDCA_temporary_write_single[1] = MCP_ADD_TXRTSCTRL;
			PDCA_temporary_write_single[2] = MCP_VAL_TXRTS_DIGITAL_INPUT;
			
			// do PDCA job
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_write_single, MCP_DIR_SOUTH);
			
			// next state should be configuration complete, set mcp to normal operation mode
			mcp_stm_set_state(status, ENTER_NORMAL_MODE_SOUTH);
			
			break;
			
		case ENTER_NORMAL_MODE_NORTH:
		/* ENTER_NORMAL_MODE_ should be the final step after all configurations 
		 * have been made to the MCP device. Uses a bit modify command to set the
		 * CTRL registers on the MCP.
		 */
			PDCA_temporary_bit_modify_tx[0] = MCP_INST_BIT_MODIFY;
			PDCA_temporary_bit_modify_tx[1] = MCP_ADD_CANCTRL;
			PDCA_temporary_bit_modify_tx[2] = MCP_MASK_CTRL_MODE;
			PDCA_temporary_bit_modify_tx[3] = MCP_VAL_MODE_NORMAL;
			
			// set PDCA job
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_bit_modify, MCP_DIR_NORTH);
						
			// go to start state after normal mode has been entered
			mcp_stm_set_state(status, ENTER_NORMAL_MODE_NORTH_CALLBACK);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rEnter Normal Mode North Job, Next is Enter Normal Mode North CALLBACK");
			#endif
			
			break;
			
		case ENTER_NORMAL_MODE_SOUTH:
			PDCA_temporary_bit_modify_tx[0] = MCP_INST_BIT_MODIFY;
			PDCA_temporary_bit_modify_tx[1] = MCP_ADD_CANCTRL;
			PDCA_temporary_bit_modify_tx[2] = MCP_MASK_CTRL_MODE;
			PDCA_temporary_bit_modify_tx[3] = MCP_VAL_MODE_NORMAL;
			
			// set PDCA job
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_bit_modify, MCP_DIR_SOUTH);
			
			// go to start state after normal mode has been entered
			mcp_stm_set_state(status, ENTER_NORMAL_MODE_SOUTH_CALLBACK);
			
			break;
		
		case ENTER_NORMAL_MODE_NORTH_CALLBACK:
		/* ENTER_NORMAL_MODE_x_CALLBACK indicates the configuration job is complete
		 */
			UNSET_JOB(status->jobs, JOB_CONFIGURE_NORTH);
			
			mcp_stm_set_state(status, START);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rEnter Normal Mode North CALLBACK Job, Next is Back to START");
			#endif
			
			break;
			
		case ENTER_NORMAL_MODE_SOUTH_CALLBACK:
		/* ENTER_NORMAL_MODE_x_CALLBACK indicates the configuration job is complete
		 */
			UNSET_JOB(status->jobs, JOB_CONFIGURE_SOUTH);
			
			mcp_stm_set_state(status, START);
			
			break;
			
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
	#if DBG_MCP_STATE
	print_dbg("\n\rLast State on State Loop Exit: ");
	print_dbg_ulong((unsigned long)mcp_status.mcp_state);
	#endif
}
