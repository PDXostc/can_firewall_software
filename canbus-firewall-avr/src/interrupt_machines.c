/*
 * interrupt_machines.c
 *
 * Created: 11/5/2015 1:44:39 PM
 *  Author: smiller6
 */ 
#include "interrupt_machines.h"

volatile bool pdca_test_transfer_complete = false;

volatile struct MCP_state_machine_t mcp_stm = {
	.mcp_state = START,
	};

volatile struct MCP_status_t mcp_status = {
	.status_byte_north = 0x00,
	.status_byte_south = 0x00,
	.error_byte_north = 0x00,
	.error_byte_south = 0x00,
	.jobs = 0x00000000,
	.attention = 0x00,
	};

// External interrupts set for low level trigger. Asynch mode allows wake on interrupt
uint8_t init_eic_options(void)
{
	// Enable level-triggered interrupt.
	//eic_options[0].eic_mode = EIC_MODE_LEVEL_TRIGGERED;
	// Enable edge-triggered interrupt.
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
	mcp_stm.mcp_state = START;
	
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
	// cpu_irq_disable_level(0); // proc handler level
	// cpu_irq_disable_level(1); // mcp machine and pdca handler level
	// cpu_irq_disable_level(2); // mcp eic pin handler level
	
	//Disable_global_interrupt();
	
	//acknowledge interrupt:
	volatile uint32_t int_ack = AVR32_EIC.isr;
	
	// test disable at this time, will enable after jobs are finished
	// eic_disable_interrupt_line(&AVR32_EIC, EXT_INT_IVI_LINE);
	
// 	volatile bool line01 = false;
// 	volatile bool line02 = false;
// 	line01 = gpio_get_pin_value(IVI_INT_PIN);
// 	line02 = gpio_get_pin_value(CAR_INT_PIN);
	//test clear mcp int flags for now
	
	//cheat, clear mcp flags slow
	//mcp_set_register(MCP_DEV_NORTH, MCP_ADD_CANINTF, 0x00);
	
	//PDCA on interrupt test
	
// 		rx_instruction_test[0] = MCP_INST_READ_RX_0;
// 		// update pdca_options
// 		PDCA_OPTIONS_tx_test.addr = &rx_instruction_test;
// 		int test_size = sizeof(rx_instruction_test);
// 		PDCA_OPTIONS_tx_test.size = sizeof(rx_instruction_test);
// 		
// 		PDCA_OPTIONS_rx_test.addr = &rx_msg_test;
// 		test_size = sizeof(rx_msg_test);
// 		PDCA_OPTIONS_rx_test.size = sizeof(rx_msg_test);		
// 		
// 		pdca_init_channel(PDCA_CHANNEL_SPI_TX, &PDCA_OPTIONS_tx_test);
// 		pdca_init_channel(PDCA_CHANNEL_SPI_RX, &PDCA_OPTIONS_rx_test);
// 		
// 		//register interrupt for rx complete...
// 		/*INTC_register_interrupt(&pdca_transfer_complete_int_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);*/
// 		INTC_register_interrupt(&pdca_rx_transfer_complete_int_handler, AVR32_PDCA_IRQ_1, AVR32_INTC_INT1);;
// 		
// 		//pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);
// 		pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_RX);
// 		//
// 		
// 		//chip select mcp
// 		mcp_select(MCP_DEV_NORTH);
// 		
// 		pdca_enable(PDCA_CHANNEL_SPI_TX);
// 		pdca_enable(PDCA_CHANNEL_SPI_RX);

	#if DBG_INT
	print_dbg("\n\rCalled North INT and Attention should be set...");
	#endif
	
	//set attention required and activate the interrupt pin for the MCP state machine
	//mcp_status.attention |= MCP_DIR_NORTH;
	// 
	// attention required is the same as needing a status update
	SET_MCP_JOB(mcp_status.jobs, JOB_GET_STATUS_NORTH);
	mcp_machine_int_set();
	
	// clear external interrupt line
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_IVI_LINE);
	
	
	// cpu_irq_enable_level(2); // mcp eic pin handler level
	// cpu_irq_enable_level(1); // mcp machine and pdca handler level
	// cpu_irq_enable_level(0); // proc handler level
	
	//Enable_global_interrupt();
	#if	DBG_INT
	print_dbg("\n\rNorth Pin Interrupt Exiting");
	#endif
}

// temp interrupt handler used for basic testing of mcp interrupt
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void mcp_interrupt_handler_south(void)
{
	#if DBG_TIME_INT
	set_timestamp("EIC_INT_south", Get_sys_count());
	#endif
	// cpu_irq_disable_level(0); // proc handler level
	// cpu_irq_disable_level(1); // mcp machine and pdca handler level
	// cpu_irq_disable_level(2); // mcp eic pin handler level
	
	//acknowledge interrupt:
	volatile uint32_t int_ack = AVR32_EIC.isr;
	
// 	volatile bool line01 = false;
// 	volatile bool line02 = false;
// 	line01 = gpio_get_pin_value(IVI_INT_PIN);
// 	line02 = gpio_get_pin_value(CAR_INT_PIN);
	
	//test clear mcp int flags for now
	//mcp_set_register(MCP_DEV_SOUTH, MCP_ADD_CANINTF, 0x00);
	// clear external interrupt line
	// ask for mcp int status
	
	//mcp_print_status(MCP_DEV_SOUTH);
	// test: display status:
// 	#if 0
// 	
// 	mcp_print_status(MCP_DEV_SOUTH);
// 	print_dbg("\n\rCanSTAT REgister");
// 	mcp_print_registers(MCP_DEV_SOUTH, MCP_ADD_CANSTAT, 1);
// 	print_dbg("\n\rCANINTE Register");
// 	mcp_print_registers(MCP_DEV_SOUTH, MCP_ADD_CANINTE, 1);
// 	print_dbg("\n\rCANINTF Register");
// 	mcp_print_registers(MCP_DEV_SOUTH, MCP_ADD_CANINTF, 1);
// 	#endif
	
	#if DBG_INT
	print_dbg("\n\rCalled South INT and Attention should be set...");
	#endif
	
	// attention required is the same as needing a status update
	SET_MCP_JOB(mcp_status.jobs, JOB_GET_STATUS_SOUTH);
	mcp_machine_int_set();
	
	// clear external interrupt line
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_CAR_LINE);
	
	// cpu_irq_enable_level(0); // proc handler level
	// cpu_irq_enable_level(1); // mcp machine and pdca handler level
	// cpu_irq_enable_level(2); // mcp eic pin handler level
}

// Processing jobs interrupt handler
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void proc_int_handler(void)
{
	//cpu_irq_disable_level(0); // proc handler level
	
	// acknowledge interrupt: (PA pins are gpio port 0)
	volatile uint32_t int_ack = AVR32_GPIO.port[PROC_INT_PIN/32].ifrc;
	
	#if DBG_INT
	//test
	set_led(LED_02, LED_ON);
	print_dbg("\n\rProc_int_handler_called!");
	set_led(LED_02, LED_OFF);
	#endif
	// test sequence
	gpio_set_pin_high(PROC_INT_PIN);
	gpio_clear_pin_interrupt_flag(PROC_INT_PIN);
	
	//cpu_irq_enable_level(0); // proc handler level	
}

// MCP state machine interrupt handler
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void mcp_machine_int_handler(void)
{
	#if DBG_TIME_MCP
	set_timestamp("MCPINTERRUPT", Get_sys_count());
	#endif
	// MCP state machine runs at INT level 1, so does PDCA
	// normally masking of interrupts would be handled for us, but our function calls
	// and execution mean that we will have to manually mask interrupts at this level
	// and below, then reenable them when exiting this handler
	// 
	// cpu_irq_disable_level(0); // proc handler level
	// cpu_irq_disable_level(1); // mcp machine and pdca handler level
	//Disable_global_interrupt();
	
	#if DBG_INT
	//test
	set_led(LED_01, LED_ON);
	print_dbg("\n\rMCP_machine_int_handler_called!");
	set_led(LED_01, LED_OFF);
	#endif
	
	// acknowledge interrupt: (GPIO pins PA are on port 0)
	volatile uint32_t int_ack = AVR32_GPIO.port[MCP_MACHINE_INT_PIN/32].ifrc;
	
	mcp_machine_int_clear();
	//Disable_global_interrupt();
	
	
	// run state machine
	// if logic permits, loop will exit and set the pin high to wait for next interruption
	// example cases:
	//		waiting for PDCA transfer complete, 
	//		waiting for pending tx
	//		waiting for mcp external interrupt attention flag
	//		
	run_mcp_state_machine();
	
// 	gpio_set_pin_high(MCP_MACHINE_INT_PIN);
// 	gpio_clear_pin_interrupt_flag(MCP_MACHINE_INT_PIN);
// 	
	
	// cpu_irq_enable_level(1); // mcp machine and pdca handler level
	// cpu_irq_enable_level(0); // proc handler level
	//Enable_global_interrupt();
#if DBG_TIME_MCP
	set_timestamp("MCPexit", Get_sys_count());
#endif
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void pdca_rx_transfer_complete_int_handler(void)
{
	#if DBG_CS_PDCA
	//test! select deselect for time stamping
	if (pdca_status.PDCA_busy == MCP_DIR_NORTH)
	{
		mcp_deselect(MCP_DEV_NORTH);
		mcp_select(MCP_DEV_NORTH);
		mcp_deselect(MCP_DEV_NORTH);
	}
	else if (pdca_status.PDCA_busy == MCP_DIR_SOUTH)
	{
		mcp_deselect(MCP_DEV_SOUTH);
		mcp_select(MCP_DEV_SOUTH);
		mcp_deselect(MCP_DEV_SOUTH);
	}
	#endif
	
	#if DBG_TIME_PDCA
	set_timestamp("pdcarxin", Get_sys_count());
	#endif
	// cpu_irq_disable_level(0); // proc handler level
	// cpu_irq_disable_level(1); // mcp machine and pdca handler level
	
	//acknowledge interrupt: 
	volatile uint32_t int_ack = AVR32_PDCA.channel[PDCA_CHANNEL_SPI_TX].isr;
	
	// Disable pdca interrupts now that transfer is complete
// 	volatile avr32_pdca_channel_t *pdca = pdca_get_handler(PDCA_CHANNEL_SPI_RX);
// 	pdca->idr = 0x07;

	// if error:
// 	if ((AVR32_PDCA.channel[PDCA_CHANNEL_SPI_TX].isr & AVR32_PDCA_TERR_MASK) == AVR32_PDCA_TERR_MASK)
// 	{
// 		// get the error address...
// 		volatile uint32_t err_add = AVR32_PDCA.channel[PDCA_CHANNEL_SPI_TX].mar;
// 		err_add;
// 	}
	
	pdca_disable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);
	pdca_disable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_RX);

	pdca_disable(PDCA_CHANNEL_SPI_TX);
	pdca_disable(PDCA_CHANNEL_SPI_RX);
	
	// deselect mcp based on which we were busy with, assumes that the busy flag
	// has been set by the mcp machine which also started this job
	if (pdca_status.PDCA_busy == MCP_DIR_NORTH)
	{
		#if DBG_CS_PDCA
		mcp_deselect(MCP_DEV_NORTH);
		mcp_select(MCP_DEV_NORTH);
		#endif
		mcp_deselect(MCP_DEV_NORTH);		
	} 
	else if (pdca_status.PDCA_busy == MCP_DIR_SOUTH)
	{
		#if DBG_CS_PDCA
		mcp_deselect(MCP_DEV_SOUTH);
		mcp_select(MCP_DEV_SOUTH);
		#endif
		mcp_deselect(MCP_DEV_SOUTH);
	}
	
	// set not busy
	pdca_status.PDCA_busy = 0;
	
	//set the mcp interrupt
	mcp_machine_int_set();
	
	// cpu_irq_enable_level(0); // proc handler level
	// cpu_irq_enable_level(1); // mcp machine and pdca handler level
	#if DBG_TIME_PDCA
	set_timestamp("pdcarxout", Get_sys_count());
	#endif
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
void pdca_tx_transfer_complete_int_handler(void)
{
	#if DBG_CS_PDCA
	//test! select deselect for time stamping
	if (pdca_status.PDCA_busy == MCP_DIR_NORTH)
	{
		mcp_deselect(MCP_DEV_NORTH);
		delay_us(164);
		mcp_select(MCP_DEV_NORTH);
		delay_us(164);
		mcp_deselect(MCP_DEV_NORTH);
	}
	else if (pdca_status.PDCA_busy == MCP_DIR_SOUTH)
	{
		mcp_deselect(MCP_DEV_SOUTH);
		delay_us(164);
		mcp_select(MCP_DEV_SOUTH);
		delay_us(164);
		mcp_deselect(MCP_DEV_SOUTH);
	}
	#endif
	#if DBG_TIME_PDCA
	set_timestamp("pdcatxin", Get_sys_count());
	#endif
	// cpu_irq_disable_level(0); // proc handler level
	// cpu_irq_disable_level(1); // mcp machine and pdca handler level
	
	//acknowledge interrupt:
	volatile uint32_t int_ack = AVR32_PDCA.channel[PDCA_CHANNEL_SPI_TX].isr;
	
	// if error:
// 	if ((AVR32_PDCA.channel[PDCA_CHANNEL_SPI_TX].isr & AVR32_PDCA_TERR_MASK) == AVR32_PDCA_TERR_MASK)
// 	{
// 		// get the error address...
// 		volatile uint32_t err_add = AVR32_PDCA.channel[PDCA_CHANNEL_SPI_TX].mar;
// 		err_add;
// 	}
	
	// Disable pdca interrupts now that transfer is complete
// 	volatile avr32_pdca_channel_t *pdca = pdca_get_handler(PDCA_CHANNEL_SPI_TX);
// 	pdca->idr = 0x07;

	pdca_disable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);

	pdca_disable(PDCA_CHANNEL_SPI_TX);
	
	// deselect mcp based on which we were busy with, assumes that the busy flag
	// has been set by the mcp machine which also started this job
	if (pdca_status.PDCA_busy == MCP_DIR_NORTH)
	{
		#if DBG_CS_PDCA
		mcp_deselect(MCP_DEV_NORTH);
		delay_us(164);
		mcp_select(MCP_DEV_NORTH);
		delay_us(164);
		#endif
		mcp_deselect(MCP_DEV_NORTH);
	}
	else if (pdca_status.PDCA_busy == MCP_DIR_SOUTH)
	{
		#if DBG_CS_PDCA
		mcp_deselect(MCP_DEV_SOUTH);
		delay_us(164);
		mcp_select(MCP_DEV_SOUTH);
		delay_us(164);
		#endif
		mcp_deselect(MCP_DEV_SOUTH);
	}
	
	// set not busy
	pdca_status.PDCA_busy = 0;
	
	//set the mcp interrupt
	mcp_machine_int_set();
	
	// cpu_irq_enable_level(0); // proc handler level
	// cpu_irq_enable_level(1); // mcp machine and pdca handler level
	#if DBG_TIME_PDCA
	set_timestamp("pdcatxout", Get_sys_count());
	#endif
}

// Set up a job for receiving response data over SPI
// Address must be set in options before calling, and buffer to send must be primed
// with instruction. Sending buffer must include dummy information to facilitate
// response byte(s) sent by MCP over SPI
//
void PDCA_set_job_rx(const struct spi_device *device,
pdca_channel_options_t *options_tx,
pdca_channel_options_t *options_rx,
uint8_t pdca_busy_flag
)
{
	// test, see if selection before init channel shows significant delay
		// select the device associated with this job
		mcp_select(device);
	
	// set pdca busy
	pdca_status.PDCA_busy = pdca_busy_flag;
	
	//initialize channels with provided options
	pdca_init_channel(PDCA_CHANNEL_SPI_TX, options_tx);
	pdca_init_channel(PDCA_CHANNEL_SPI_RX, options_rx);
	
	// register the interrupt for receive transfer complete. IRQ 1 corresponds to the SPI_RX channel,
	// INT level 1 is used so that interrupt resides on same level as the MCP state machine, able
	// to interrupt the Main loop or Processing handler
	// INTC_register_interrupt(&pdca_rx_transfer_complete_int_handler, AVR32_PDCA_IRQ_1, INT_LEVEL_PDCA);
	
	// enable the interrupt for receive transfer complete
	pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_RX);
	
	// enable the transfer error interrupt, if memory address appears invalid
	// pdca_enable_interrupt_transfer_error(PDCA_CHANNEL_SPI_RX);
	
	// select the device associated with this job
	// mcp_select(device);
	
	//enable both channels for transfer
	pdca_enable(PDCA_CHANNEL_SPI_TX);
	pdca_enable(PDCA_CHANNEL_SPI_RX);
}


// Set up a job for transmitting data over SPI
// Address must be set in options before calling, sending buffer must contain a valid
// instruction. This function does not care about the received data; therefore it
// calls back on transmit complete and does not init the PDCA_SPI_RX channel
//
void PDCA_set_job_tx(const struct spi_device *device,
pdca_channel_options_t *options_tx,
uint8_t pdca_busy_flag
)
{
	// test, see if selection prior shows significant delay
		// select the device associated with this job
		mcp_select(device);
	// set pdca busy
	pdca_status.PDCA_busy = pdca_busy_flag;
	
	// initialize tx channel with options
	pdca_init_channel(PDCA_CHANNEL_SPI_TX, options_tx);
	
	// register the interrupt for transmit complete
	// INTC_register_interrupt(&pdca_tx_transfer_complete_int_handler, AVR32_PDCA_IRQ_0, INT_LEVEL_PDCA);
	
	// enable interrupt for transmission complete
	pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);
	
	// select the device associated with this job
	// mcp_select(device);
	
	//enable the transmit channel
	pdca_enable(PDCA_CHANNEL_SPI_TX);
}

void init_interrupt_machines(void)
{
	//init the mcp status
	/*uint8_t success  = */init_mcp_status();
	
	/************************************************************************/
	/* Interrupts                                                           */
	/************************************************************************/
	
	Disable_global_interrupt();
	
	INTC_init_interrupts();
	
	// Setup Pin interrupts for MCP state machine and processing jobs
	gpio_configure_pin(MCP_MACHINE_INT_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(PROC_INT_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	// testing using local interface for pins instead
	gpio_local_init();
	// gpio_local_enable_pin_output_driver(MCP_MACHINE_INT_PIN);
	// gpio_local_set_gpio_pin(MCP_MACHINE_INT_PIN);
	
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
	
	
	// register the interrupt for receive transfer complete. IRQ 1 corresponds to the SPI_RX channel,
	// INT level 1 is used so that interrupt resides on same level as the MCP state machine, able
	// to interrupt the Main loop or Processing handler
	INTC_register_interrupt(&pdca_rx_transfer_complete_int_handler, AVR32_PDCA_IRQ_1, INT_LEVEL_PDCA);
	
	// register the interrupt for transmit complete
	INTC_register_interrupt(&pdca_tx_transfer_complete_int_handler, AVR32_PDCA_IRQ_0, INT_LEVEL_PDCA);
	
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

bool local_test_once = true;
int local_tx_count = 0;

#define TEST_ONCE true

// temp test routine of queing a received message for transmission for throughput
void test_set_received_message_for_transmit(void)
{
	//have to keep proc ahead of tx, for now we cheat and set it == rx
	que_ptr_proc = que_ptr_rx;
	#if TEST_ONCE
	if ((local_test_once == true) && local_tx_count > 1)
	{
		return;
	}
	local_tx_count++;
	#endif
	//set job pending and inc tx pending count
	TX_status.tx_pending_count++;
	SET_MCP_JOB(mcp_status.jobs, JOB_TX_PENDING);
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
void run_mcp_state_machine(void)
{
	// run the machine when this is called, if any state needs to exit the state loop
	// for a reason other than PDCA being busy, this gives the opportunity to break
	// out. This should always reset to true when this function is called by interrupt.
	volatile bool run_machine = true;
	#if DBG_MCP_STATE
	print_dbg("\n\r...Run state machine called...");
	#endif
	
	#if DBG_TIME_MCP
	set_timestamp("MCP_start", Get_sys_count());
	#endif
	// if the PDCA is free, run the machine
	while ((pdca_status.PDCA_busy == 0) && (run_machine == true))
	{
		#if DBG_MCP_STATE
		//print_dbg("\n\r...Entered while loop of machine...");
		#endif
		//run machine
		switch (mcp_stm.mcp_state)
		{
		case START:
		/* START case should be the default state. Other states should return here
		 * when tasks complete. START should send the machine on to initiate required
		 * jobs.
		 */
			// goto jobs if pending
			if (mcp_status.jobs > 0)
			{
				mcp_stm_set_state(&mcp_stm, JOB_START);
			} 
			else 
			{
				// check for pin still held low, meaning attention is needed
				if (!gpio_local_get_pin_value(IVI_INT_PIN))
				{
					mcp_stm_set_job(&mcp_status, JOB_GET_STATUS_NORTH);
				}
				
				if (!gpio_local_get_pin_value(CAR_INT_PIN))
				{
					mcp_stm_set_job(&mcp_status, JOB_GET_STATUS_SOUTH);
				}
				
				if(mcp_status.jobs == 0)
				{
					// no jobs or attention required, cease the machine
					run_machine = false;					
				}
			}

			
			break;
			
		case JOB_START:
		/* JOB_START is the main brancing off point for executing pending tasks.
		 * Pending jobs flags are checked and executed in order of priority. Jobs
		 * are exclusive; the state machine can have multiple jobs pending, but 
		 * will only execute one at a time and must finish execution before another
		 * can be started.
		 */			
			if ((mcp_status.jobs & JOB_RESET_NORTH) == JOB_RESET_NORTH) {
				mcp_stm_set_state(&mcp_stm, RESET_NORTH);
			}		
			else if ((mcp_status.jobs & JOB_RESET_SOUTH) == JOB_RESET_SOUTH) {
				mcp_stm_set_state(&mcp_stm, RESET_SOUTH);
			}
			else if ((mcp_status.jobs & JOB_CONFIGURE_NORTH) == JOB_CONFIGURE_NORTH) {
				mcp_stm_set_state(&mcp_stm, ENTER_CONFIG_MODE_NORTH);
			}
			else if ((mcp_status.jobs & JOB_CONFIGURE_SOUTH) == JOB_CONFIGURE_SOUTH) {
				mcp_stm_set_state(&mcp_stm, ENTER_CONFIG_MODE_SOUTH);
			}
			else if ((mcp_status.jobs & JOB_GET_STATUS_NORTH) == JOB_GET_STATUS_NORTH) {
				mcp_stm_set_state(&mcp_stm, GET_STATUS_NORTH);
			}
			else if  ((mcp_status.jobs & JOB_GET_STATUS_SOUTH) == JOB_GET_STATUS_SOUTH) {
				mcp_stm_set_state(&mcp_stm, GET_STATUS_SOUTH);
			}
			else if ((mcp_status.jobs & JOB_GET_ERROR_REG_NORTH) == JOB_GET_ERROR_REG_NORTH) {
				mcp_stm_set_state(&mcp_stm, GET_ERROR_REG_NORTH);
			}
			else if ((mcp_status.jobs & JOB_GET_ERROR_REG_SOUTH) == JOB_GET_ERROR_REG_SOUTH) {
				mcp_stm_set_state(&mcp_stm, GET_ERROR_REG_SOUTH);
			}
			else if ((mcp_status.jobs & JOB_TX_PENDING) == JOB_TX_PENDING) {
				mcp_stm_set_state(&mcp_stm, TX_PENDING);
			}
			else if ((mcp_status.jobs & JOB_RX_0_NORTH) == JOB_RX_0_NORTH) {
				mcp_stm_set_state(&mcp_stm, READ_RX_0_NORTH);
				//temp!!! aborting here because we don't handle this job yet
// 				print_dbg("\n\rABORTING\n\r");
// 				run_machine = false;
			}
			else if ((mcp_status.jobs & JOB_RX_0_SOUTH) == JOB_RX_0_SOUTH) {
				mcp_stm_set_state(&mcp_stm, READ_RX_0_SOUTH);
				//temp!!! aborting here because we don't handle this job yet
// 				print_dbg("\n\rABORTING\n\r");
// 				run_machine = false;
			}
			else if ((mcp_status.jobs & JOB_RX_1_NORTH) == JOB_RX_1_NORTH) {
				mcp_stm_set_state(&mcp_stm, READ_RX_1_NORTH);
				//temp!!! aborting here because we don't handle this job yet
// 				print_dbg("\n\rABORTING\n\r");
// 				run_machine = false;
			}
			else if ((mcp_status.jobs & JOB_RX_1_SOUTH) == JOB_RX_1_SOUTH) {
				mcp_stm_set_state(&mcp_stm, READ_RX_1_SOUTH);
				//temp!!! aborting here because we don't handle this job yet
// 				print_dbg("\n\rABORTING\n\r");
// 				run_machine = false;
			}
			else if (mcp_status.jobs == JOB_NO_JOBS) {
				mcp_stm_set_state(&mcp_stm, NO_JOBS);
			}
			break;
			
		case NO_JOBS:
		/* NO_JOBS is the fall through case for job initiation. If there are no jobs
		 * the machine should go back to the start.
		 */
			mcp_stm_set_state(&mcp_stm, START);
			run_machine = false;
			
			break;
			
		case RESET_NORTH:
		/* RESET_x case requires that either:
		 *		the pin to drive the reset on an MCP chip is held low, or
		 *		the RESET command is sent over SPI, as a PDCA job
		 *	It is probably faster to wait a few cycles holding the pin, but conceivable
		 *	that in the future a reset case required at run time could benefit from
		 *	sceduling the reset as a PDCA job.
		 */
			mcp_reset_pin(MCP_DEV_NORTH);
			
			//if using pin method, set job complete in this state
			mcp_stm_unset_job(&mcp_status, JOB_RESET_NORTH);
			
			mcp_stm_set_state(&mcp_stm, START);
			
			break;
			
		case RESET_SOUTH:
		
			mcp_reset_pin(MCP_DEV_SOUTH);
		
			//if using pin method, set job complete in this state
			mcp_stm_unset_job(&mcp_status, JOB_RESET_SOUTH);
		
			mcp_stm_set_state(&mcp_stm, START);
		
			break;
		
		case ENTER_CONFIG_MODE_NORTH:
		/* ENTER_CONFIG_MODE_x is the first state in the mcp configuration job sequence.
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
			mcp_stm_set_state(&mcp_stm, CONFIGURE_BIT_TIMINGS_NORTH);
			
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
			mcp_stm_set_state(&mcp_stm, CONFIGURE_BIT_TIMINGS_SOUTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rEnter Config Mode South Job, Next is Configure Bit Timings South");
			#endif
			
			break;
			
		case CONFIGURE_BIT_TIMINGS_NORTH:
		/* CONFIGURE_BIT_TIMINGS_x uses the preconfigured bit timings set up in the 
		 * MCP status structure. A job is scheduled with the PDCA for a consecutive
		 * write of registers on the MCP corresponding to CNF3, CNF2, CNF1
		 */
			// must set up the options with the desired timing information buffer. 
			// should contain instruction, address and consecutive configuration
			// information
			PDCA_options_mcp_spi_tx_configure_timings.addr = &mcp_status.timings_north;
			
			// set job with provided settings
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_configure_timings, MCP_DIR_NORTH);
			
			// next state will be to set the interrupt enable in the MCP interrupt enable register
			mcp_stm_set_state(&mcp_stm, SET_MCP_INTERRUPT_ENABLE_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rConfigure Bit Timings North Job, Next is Configure RX0 North");
			#endif
			
			break;
			
		case CONFIGURE_BIT_TIMINGS_SOUTH:
			// must set up the options with the desired timing information buffer.
			// should contain instruction, address and consecutive configuration
			// information
			PDCA_options_mcp_spi_tx_configure_timings.addr = &mcp_status.timings_south;
			
			// set job with provided settings
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_configure_timings, MCP_DIR_SOUTH);
			
			// next state will be to configure the first receive buffer
			mcp_stm_set_state(&mcp_stm, SET_MCP_INTERRUPT_ENABLE_SOUTH);
			
			break;
			
		case SET_MCP_INTERRUPT_ENABLE_NORTH:
		/* SET_MCP_INTERRUPT_ENABLE_x stage in programming job to set the MCP interrupt
		 * enable register to desired settings. Currently, our design only wants
		 * proactive interrupts on new messages received by the MCP. Interrupts for
		 * transmission buffer(s) are not used, as the system will check the last
		 * acquired status (or request new status) of the transmit buffer when a message is ready for
		 * transmission.
		 */
			// load temp buffer with instruction payload
			PDCA_temporary_write_single[0] = MCP_INST_WRITE;
			PDCA_temporary_write_single[1] = MCP_ADD_CANINTE;
			PDCA_temporary_write_single[2] = MCP_DEFAULT_INTERRUPT_VALUE_NORTH;
			
			// set job to send interrupt enable instruction
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_write_single_register, MCP_DIR_NORTH);
			
			// next state should be to configure the receive buffer 0
			mcp_stm_set_state(&mcp_stm, CONFIGURE_RX_0_NORTH);
			break;
			
		case SET_MCP_INTERRUPT_ENABLE_SOUTH:
			// load temp buffer with instruction payload
			PDCA_temporary_write_single[0] = MCP_INST_WRITE;
			PDCA_temporary_write_single[1] = MCP_ADD_CANINTE;
			PDCA_temporary_write_single[2] = MCP_DEFAULT_INTERRUPT_VALUE_SOUTH;
			
			// set job to send interrupt enable instruction
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_write_single_register, MCP_DIR_SOUTH);
			
			// next state should be to configure the receive buffer 0
			mcp_stm_set_state(&mcp_stm, CONFIGURE_RX_0_SOUTH);
			break;
			
		case CONFIGURE_RX_0_NORTH:
		/* CONFIGURE_RX_n_x is part of the start up and programming sequence. Configuration
		 * of the MCP receive buffers can only be done in Configuration Mode;
		 */
			//TODO: Use PDCA for proper job, requiring more states to be inserted
			// in the machine
			//FOR NOW: Configuring the Receive buffers is a start up action, during
			// which the MCP devices are not active. The Micro has no other tasks
			// at this time, so it is acceptable to use the slower waiting interface
			// for the MCPs
			// 
			mcp_configure_rx_0(MCP_DEV_NORTH, mcp_status.rx_config_north);
			
			// next state configures the second receive buffer
			mcp_stm_set_state(&mcp_stm, CONFIGURE_RX_1_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rConfigure_RX0 North Job, Next is Configure RX1 North");
			#endif
			
			break;
			
		case CONFIGURE_RX_1_NORTH:
			mcp_configure_rx_1(MCP_DEV_NORTH, mcp_status.rx_config_north);
			
			// next state is to configure the ready to send pins on MCP
			mcp_stm_set_state(&mcp_stm, CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_NORTH);
			
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
			mcp_configure_rx_0(MCP_DEV_SOUTH, mcp_status.rx_config_south);
		
			// next state configures the second receive buffer
			mcp_stm_set_state(&mcp_stm, CONFIGURE_RX_1_SOUTH);
		
			break;
		case CONFIGURE_RX_1_SOUTH:
			mcp_configure_rx_1(MCP_DEV_SOUTH, mcp_status.rx_config_south);
					
			// next state is to configure the ready to send pins on MCP
			mcp_stm_set_state(&mcp_stm, CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_SOUTH);
					
			break;
			
		case CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_NORTH:
		/* CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_x is a simple but necessary operation
		 * in the programming phase of initializing MCP controllers. The RTS pins
		 * in our case should be set to digital output so that the transmit buffer 
		 * conditions are not controlled by the exterior pins on the MCP
		 */
			PDCA_temporary_write_single[0] = MCP_INST_WRITE;
			PDCA_temporary_write_single[1] = MCP_ADD_TXRTSCTRL;
			PDCA_temporary_write_single[2] = MCP_VAL_TXRTS_DIGITAL_INPUT;
			
			// do PDCA job
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_write_single_register, MCP_DIR_NORTH);
			
			// next state should be configuration complete, set mcp to normal operation mode
			mcp_stm_set_state(&mcp_stm, ENTER_NORMAL_MODE_NORTH);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rConfigure Ready To Send Pins North Job, Next is Enter Normal Mode North");
			#endif
			
			break;
			
		case CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_SOUTH:
			PDCA_temporary_write_single[0] = MCP_INST_WRITE;
			PDCA_temporary_write_single[1] = MCP_ADD_TXRTSCTRL;
			PDCA_temporary_write_single[2] = MCP_VAL_TXRTS_DIGITAL_INPUT;
			
			// do PDCA job
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_write_single_register, MCP_DIR_SOUTH);
			
			// next state should be configuration complete, set mcp to normal operation mode
			mcp_stm_set_state(&mcp_stm, ENTER_NORMAL_MODE_SOUTH);
			
			break;
			
		case ENTER_NORMAL_MODE_NORTH:
		/* ENTER_NORMAL_MODE_x should be the final step after all configurations 
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
			mcp_stm_set_state(&mcp_stm, ENTER_NORMAL_MODE_NORTH_CALLBACK);
			
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
			mcp_stm_set_state(&mcp_stm, ENTER_NORMAL_MODE_SOUTH_CALLBACK);
			
			break;
		
		case ENTER_NORMAL_MODE_NORTH_CALLBACK:
		/* ENTER_NORMAL_MODE_x_CALLBACK indicates the configuration job is complete
		 */
			UNSET_MCP_JOB(mcp_status.jobs, JOB_CONFIGURE_NORTH);
			
			mcp_stm_set_state(&mcp_stm, START);
			
			#if DBG_MCP_STATE
			print_dbg("\n\rEnter Normal Mode North CALLBACK Job, Next is Back to START");
			#endif
			
			break;
			
		case ENTER_NORMAL_MODE_SOUTH_CALLBACK:
		/* ENTER_NORMAL_MODE_x_CALLBACK indicates the configuration job is complete
		 */
			UNSET_MCP_JOB(mcp_status.jobs, JOB_CONFIGURE_SOUTH);
			
			mcp_stm_set_state(&mcp_stm, START);
			
			break;
			
		case GET_STATUS_NORTH:
		/* GET_STATUS_x is a small job to request an updated status byte from the MCP
		 * device. Getting status is usually done when a new attention request has
		 * been received from an external MCP interrupt pin, and no jobs are found
		 * pending. The status byte will be analyzed on callback and used to set
		 * pending jobs.
		 * Get status should clear the attention needed flag for the respective MCP device.
		 * Reasoning: Clearing the attention required now will alllow for the next flag
		 * set by an external interrupt to indicate that a new condition has arisen
		 * while processing current pending jobs.
		 * 
		 * Above statement invalid. Attention should be cleared when we think the 
		 * Interrupt flag on an MCP could result in the pin lowering. Therefore 
		 * the attn flag should be cleared when we’ve used the Read RX Buffer 
		 * instruction to get a CAN msg, which clears the flag on the MCP.
		 * Please remember to clear the attention when on the receive message callback state
		 */
			// clear attention for north
			//mcp_status.attention &= ~MCP_DIR_NORTH;
			MCP_UNSET_ATTN(mcp_status.attention, MCP_DIR_NORTH);
		
			// temporary instruction buffer is used for this purpose, just has to be
			// loaded with the correct instruction byte
			PDCA_temporary_instruction_tx[0] = MCP_INST_READ_STATUS;
			
			//testing reload counter usage to eliminate extra byte
			//PDCA_options_mcp_spi_rx_get_status_north.r_addr = &mcp_status.status_byte_north;
			
			// set job to receive a status byte from the mcp. options for single 
			// instruction sizes should correspond to a single byte in this case
			PDCA_set_job_rx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_write_single_instruction_single_response, &PDCA_options_mcp_spi_rx_get_status_north, MCP_DIR_NORTH);
			
			// go to status callback when byte transferred
			mcp_stm_set_state(&mcp_stm, GET_STATUS_NORTH_CALLBACK);
			
			break;
			
		case GET_STATUS_SOUTH:
		// TODO
			// clear attention for north
			//mcp_status.attention &= ~MCP_DIR_SOUTH;
			
			#if DBG_TIME_STATUS
			set_timestamp("status_south", Get_sys_count());
			#endif
			
			// temporary instruction buffer is used for this purpose, just has to be
			// loaded with the correct instruction byte
			PDCA_temporary_instruction_tx[0] = MCP_INST_READ_STATUS;
			
			// set job to receive a status byte from the mcp. options for single
			// instruction sizes should correspond to a single byte in this case
			PDCA_set_job_rx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_write_single_instruction_single_response, &PDCA_options_mcp_spi_rx_get_status_south, MCP_DIR_SOUTH);
			
			// go to status callback when byte transferred
			mcp_stm_set_state(&mcp_stm, GET_STATUS_SOUTH_CALLBACK);
			
			break;
			
		case GET_STATUS_NORTH_CALLBACK:
		/* GET_STATUS_x_CALLBACK_ expects that the status byte requested for the MCP
		 * device has been received. The status byte is then used to set pending
		 * jobs for the MCP state machine. Practically, we are only setting jobs here
		 * based on the RX buffer mcp_status, 
		 */
			// if status byte has come back 0x00, we likely got here because attention
			// was required, but it may have come from an error condition. This 
			// feature will be enabled in the future
// 			if (mcp_status.status_byte_north == 0x00)
// 			{
// 				SET_JOB(mcp_status.jobs, JOB_GET_ERROR_REG_NORTH);
// 			}
			
			
			//use status byte from north to set specific receive jobs for north
			// set job will or the existing jobs list, so if the job is zeros, 
			// no others will be affected
			if ((mcp_status.status_byte_north & 0x01) == 0x01)
			{
				SET_MCP_JOB(mcp_status.jobs, JOB_RX_0_NORTH);
			}
			if ((mcp_status.status_byte_north & 0x02) == 0x02)
			{
				SET_MCP_JOB(mcp_status.jobs, JOB_RX_1_NORTH);
			}
			
			// set getting of status job complete
			UNSET_MCP_JOB(mcp_status.jobs, JOB_GET_STATUS_NORTH);
			
			// go to start now that additional jobs have been set
			mcp_stm_set_state(&mcp_stm, START);
			
			#if DBG_MCP_STATUS
			// print status byte and jobs that have been set
			print_dbg("\n\r___North Status Received___");
			print_dbg("Status_byte_");
			print_dbg_char_hex(mcp_status.status_byte_north);
			print_dbg("_Jobs_Pending_");
			print_dbg_hex(mcp_status.jobs);
			
				#if DBG_MCP
				// also do a slow print from the actual mcp
				mcp_print_status(MCP_DEV_NORTH);
				#endif
				
			#endif
			
			break;
			
		case GET_STATUS_SOUTH_CALLBACK:	
			#if DBG_TIME_STATUS
			set_timestamp("status_south_b", Get_sys_count());
			#endif
			
			//use status byte from north to set specific receive jobs for north
			// set job will or the existing jobs list, so if the job is zeros,
			// no others will be affected
			if ((mcp_status.status_byte_south & 0x01) == 0x01)
			{
				SET_MCP_JOB(mcp_status.jobs, JOB_RX_0_SOUTH);
			}
			if ((mcp_status.status_byte_south & 0x02) == 0x02)
			{
				SET_MCP_JOB(mcp_status.jobs, JOB_RX_1_SOUTH);
			}
		
			// set getting of status job complete
			UNSET_MCP_JOB(mcp_status.jobs, JOB_GET_STATUS_SOUTH);
		
			// go to start now that additional jobs have been set
			mcp_stm_set_state(&mcp_stm, START);
		
			#if DBG_MCP_STATUS
			// print status byte and jobs that have been set
			print_dbg("\n\r___South Status Received___");
			print_dbg("Status_byte_");
			print_dbg_char_hex(mcp_status.status_byte_south);
			print_dbg("_Jobs_Pending_");
			print_dbg_hex(mcp_status.jobs);
		
				#if DBG_MCP
				// also do a slow print from the actual mcp
				mcp_print_status(MCP_DEV_SOUTH);
				#endif
		
			#endif
			break;
			
		case GET_ERROR_REG_NORTH:
		//TODO			
		case GET_ERROR_REG_SOUTH:
		// TODO
		case GET_ERROR_REG_NORTH_CALLBACK:
		// TODO
		case GET_ERROR_REG_SOUTH_CALLBACK:
		// TODO
			break;
			
		case TX_PENDING:
		/* TX_PENDING is the initial step for the transmission job. Any additional
		 * prep should go here. Transmission jobs have priority over receive jobs.
		 * The transmission job is set when the Processing
		 * job completes and increments a count of the messages in the que that
		 * it has evaluated and filtered. When the transmission job has finished
		 * uploading a message to an MCP device, the transmission count will be 
		 * decremented; if cound evaluates to zero, the job is considered to be
		 * finished.
		 * Proceed to evaluating the contents of the transmission
		 * pointer.
		 */
			// check that the TX is not equal to the proc or rx before eval stage
			if((que_ptr_tx != que_ptr_proc) && (que_ptr_tx != que_ptr_rx))
			{
				mcp_stm_set_state(&mcp_stm, EVALUATE_TX_POINTER);
			}
			else if(que_ptr_tx == que_ptr_proc)// job was not cleared correctly, or tx was advanced too far...
												// for now we should clear the job to allow others to proceed
			{
				UNSET_MCP_JOB(mcp_status.jobs, JOB_TX_PENDING);
				TX_status.tx_pending_count = 0;
				mcp_stm_set_state(&mcp_stm, START);
			}
			
			break;
			
		case EVALUATE_TX_POINTER:
		/* EVALUATE_TX_POINTER
		 * There is a single TX pointer moving through the message que, following
		 * after the Processing pointer. After Processing, a message is scheduled
		 * to be evaluated for transmission. This stage looks at the contents of
		 * the message the TX pointer indicates. If zeros are found for the ID, 
		 * the message is considered to have been wiped/discarded, and the TX pointer
		 * will be advanced without transmission.
		 * If the message ID is nonzero, it is considered to have passed the filter
		 * process and will be retransmitted.
		 * This evaluation stage must check for a last known free TX buffer on the 
		 * respective MCP device. If one is found, the next stage will be the Load
		 * state for that buffer. If one is not found, a Get Status job will be set,
		 * so that any TX buffers that have become free since the last status update
		 * will be known.
		 * NOTE: The message format is assumed to be the MCP 13 byte array format,
		 * so checking the id should involve bytes [0:1] in the msg (SIDH and SIDL)
		 * NOTE: The check for a free buffer is done by looking at the status byte.
		 * The relevant bits are 2, 4, 6, corresponding to the TXREQ bit in the TXBnCTRL
		 * register, signifying that a transmission is pending. If these bits are set,
		 * a transmission is pending and we cannot write to the TX buffer. If clear,
		 * the buffer is empty.
		 */
			// check nonzero msg id, else move on
			if ((que_ptr_tx->msg[0] > 0x00) || (que_ptr_tx->msg[1] > 0x00))
			{
				// the message has a direction. please remember that this setting
				// indicates the direction the message is bound for, ie if received
				// from the north, the message is headed south, and vice versa.
				// check for free TX buffers in status byte for the MCP device in this direction
				if (que_ptr_tx->direction == MCP_DIR_NORTH)
				{
					// set to upload for whichever is free. if none found we need
					// a refreshed status
					if( ((mcp_status.status_byte_north >> 2) & 0x01) == 0x00 )
					{
						// first buffer not pending, choose for upload
						mcp_stm_set_state(&mcp_stm, LOAD_TXB_0_NORTH);
					} 
					else if (((mcp_status.status_byte_north >> 4) & 0x01) == 0x00 )
					{
						// second buffer not pending, choose for upload
						mcp_stm_set_state(&mcp_stm, LOAD_TXB_1_NORTH);
					}  
					else if (((mcp_status.status_byte_north >> 6) & 0x01) == 0x00 )
					{
						// second buffer not pending, choose for upload
						mcp_stm_set_state(&mcp_stm, LOAD_TXB_2_NORTH);
					} 
					else 
					{
						// no open buffers found, set a get status job and return to start
						SET_MCP_JOB(mcp_status.jobs, JOB_GET_STATUS_NORTH);
						mcp_stm_set_state(&mcp_stm, START);
					}
				} // else check south direction
				else if (que_ptr_tx->direction == MCP_DIR_SOUTH)
				{
					// set to upload for whichever is free. if none found we need
					// a refreshed status
					if( ((mcp_status.status_byte_north >> 2) & 0x01) == 0x00 )
					{
						// first buffer not pending, choose for upload
						mcp_stm_set_state(&mcp_stm, LOAD_TXB_0_SOUTH);
					}
					else if (((mcp_status.status_byte_north >> 4) & 0x01) == 0x00 )
					{
						// second buffer not pending, choose for upload
						mcp_stm_set_state(&mcp_stm, LOAD_TXB_1_SOUTH);
					}
					else if (((mcp_status.status_byte_north >> 6) & 0x01) == 0x00 )
					{
						// second buffer not pending, choose for upload
						mcp_stm_set_state(&mcp_stm, LOAD_TXB_2_SOUTH);
					}					
					else 
					{
						// no open buffers found, set a get status job and return to start
						SET_MCP_JOB(mcp_status.jobs, JOB_GET_STATUS_SOUTH);
						mcp_stm_set_state(&mcp_stm, START);
					}
				}
			} // id zero, move pointer and decrement 
			else
			{
				mcp_stm_set_state(&mcp_stm, ADVANCE_TX);
			}
			
			break;
			
		case LOAD_TXB_0_NORTH:
		/* LOADTXB_n_x
		 * This stage in the transmission job is simply to set the correct parameters
		 * in the options to be handed to the PDCA, and set the job to upload the
		 * CAN message to the repsective TX buffer using PDCA over SPI.
		 */
			// set instruction to load TXB0, in the temp instruction array
			PDCA_temporary_instruction_tx[0] = MCP_INST_LOAD_TX_0;
			// set address of tx pointer message for sending
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			
			// set job to upload message to MCP device
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_msg_tx, MCP_DIR_NORTH);
			
			// next state is callback when upload is complete
			mcp_stm_set_state(&mcp_stm, LOAD_TXB_0_NORTH_CALLBACK);
			
			break;
			
		case LOAD_TXB_1_NORTH:
			// set instruction to load TXB0, in the temp instruction array
			PDCA_temporary_instruction_tx[0] = MCP_INST_LOAD_TX_1;
			// set address of tx pointer message for sending
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
		
			// set job to upload message to MCP device
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_msg_tx, MCP_DIR_NORTH);
		
			// next state is callback when upload is complete
			mcp_stm_set_state(&mcp_stm, LOAD_TXB_1_NORTH_CALLBACK);
		
			break;
			
		case LOAD_TXB_2_NORTH:
			// set instruction to load TXB0, in the temp instruction array
			PDCA_temporary_instruction_tx[0] = MCP_INST_LOAD_TX_2;
			// set address of tx pointer message for sending
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			
			// set job to upload message to MCP device
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_msg_tx, MCP_DIR_NORTH);
			
			// next state is callback when upload is complete
			mcp_stm_set_state(&mcp_stm, LOAD_TXB_2_NORTH_CALLBACK);
			
			break;
		case LOAD_TXB_0_NORTH_CALLBACK:
		/* LOAD_TXB_n_x_CALLBACK
		 * The CAN message has been uploaded to an MCP device. This stage should
		 * update the status byte that this buffer will be pending transmission,
		 * and set a job to issue a request to send instruction for the pending TX
		 * buffer. The next state will be TX pointer advancement and job decrement.
		 */
			// update north status bit showing request for tx should be pending
			mcp_status.status_byte_north |= (1 << 2);
			//mcp_status.status_byte_north = 0xFF;
			
			#if	DBG_MCP_CAN_TX
			// show status byte updated after alteration
			PRINT_NEWLINE()
			print_dbg("MCP status byte North Updated");
			print_dbg_char_hex(mcp_status.status_byte_north);
			#endif
			
			// set request to send instruction in temporary array
			PDCA_temporary_instruction_tx[0] = MCP_INST_RTS_TXB0;
			
			// set job to send instruction
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_write_single_instruction, MCP_DIR_NORTH);
			
			// next state is Advance TX pointer
			mcp_stm_set_state(&mcp_stm, ADVANCE_TX);
			
			break;
			
		case LOAD_TXB_1_NORTH_CALLBACK:
			// update north status bit showing request for tx should be pending
			mcp_status.status_byte_north |= (1 << 4);
			
			#if	DBG_MCP_CAN_TX
			// show status byte updated after alteration
			PRINT_NEWLINE()
			print_dbg("MCP status byte North Updated");
			print_dbg_char_hex(mcp_status.status_byte_north);
			#endif
			
			// set request to send instruction in temporary array
			PDCA_temporary_instruction_tx[0] = MCP_INST_RTS_TXB1;
			
			// set job to send instruction
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_write_single_instruction, MCP_DIR_NORTH);
			
			// next state is Advance TX pointer
			mcp_stm_set_state(&mcp_stm, ADVANCE_TX);
			
			break;
			
		case LOAD_TXB_2_NORTH_CALLBACK:
			// update north status bit showing request for tx should be pending
			mcp_status.status_byte_north |= (1 << 6);
			
			#if	DBG_MCP_CAN_TX
			// show status byte updated after alteration
			PRINT_NEWLINE()
			print_dbg("MCP status byte North Updated");
			print_dbg_char_hex(mcp_status.status_byte_north);
			#endif
			
			// set request to send instruction in temporary array
			PDCA_temporary_instruction_tx[0] = MCP_INST_RTS_TXB2;
			
			// set job to send instruction
			PDCA_set_job_tx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_tx_write_single_instruction, MCP_DIR_NORTH);
			
			// next state is Advance TX pointer
			mcp_stm_set_state(&mcp_stm, ADVANCE_TX);
			
			break;
			
		case LOAD_TXB_0_SOUTH:
			// set instruction to load TXB0, in the temp instruction array
			PDCA_temporary_instruction_tx[0] = MCP_INST_LOAD_TX_0;
			// set address of tx pointer message for sending
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			
			// set job to upload message to MCP device
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_msg_tx, MCP_DIR_SOUTH);
			
			// next state is callback when upload is complete
			mcp_stm_set_state(&mcp_stm, LOAD_TXB_0_SOUTH_CALLBACK);
			
			break;
			
		case LOAD_TXB_1_SOUTH:
			// set instruction to load TXB0, in the temp instruction array
			PDCA_temporary_instruction_tx[0] = MCP_INST_LOAD_TX_1;
			// set address of tx pointer message for sending
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			
			// set job to upload message to MCP device
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_msg_tx, MCP_DIR_SOUTH);
			
			// next state is callback when upload is complete
			mcp_stm_set_state(&mcp_stm, LOAD_TXB_1_SOUTH_CALLBACK);
			
			break;
			
		case LOAD_TXB_2_SOUTH:
			// set instruction to load TXB0, in the temp instruction array
			PDCA_temporary_instruction_tx[0] = MCP_INST_LOAD_TX_2;
			// set address of tx pointer message for sending
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			
			// set job to upload message to MCP device
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_msg_tx, MCP_DIR_SOUTH);
			
			// next state is callback when upload is complete
			mcp_stm_set_state(&mcp_stm, LOAD_TXB_2_SOUTH_CALLBACK);
			
			break;
			
		case LOAD_TXB_0_SOUTH_CALLBACK:
			// update north status bit showing request for tx should be pending
			mcp_status.status_byte_south |= (1 << 2);
			
			// set request to send instruction in temporary array
			PDCA_temporary_instruction_tx[0] = MCP_INST_RTS_TXB0;
			
			// set job to send instruction
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_write_single_instruction, MCP_DIR_SOUTH);
			
			// next state is Advance TX pointer
			mcp_stm_set_state(&mcp_stm, ADVANCE_TX);
			
			break;
			
		case LOAD_TXB_1_SOUTH_CALLBACK:
			// update north status bit showing request for tx should be pending
			mcp_status.status_byte_south |= (1 << 4);
			
			// set request to send instruction in temporary array
			PDCA_temporary_instruction_tx[0] = MCP_INST_RTS_TXB1;
			
			// set job to send instruction
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_write_single_instruction, MCP_DIR_SOUTH);
			
			// next state is Advance TX pointer
			mcp_stm_set_state(&mcp_stm, ADVANCE_TX);
			
			break;
		case LOAD_TXB_2_SOUTH_CALLBACK:
			// update north status bit showing request for tx should be pending
			mcp_status.status_byte_south |= (1 << 6);
			
			// set request to send instruction in temporary array
			PDCA_temporary_instruction_tx[0] = MCP_INST_RTS_TXB2;
			
			// set job to send instruction
			PDCA_set_job_tx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_tx_write_single_instruction, MCP_DIR_SOUTH);
			
			// next state is Advance TX pointer
			mcp_stm_set_state(&mcp_stm, ADVANCE_TX);
			
			break;
			
		case ADVANCE_TX:
		/* ADVANCE_TX
		 * At this stage, the TX pointer has been evaluated, and should be 
		 * advanced, if able. Also decrement and resolve pending tx job(s)
		 */
			//check pointer not equal to proceeding Process pointer, if so, advance
			if ((que_ptr_tx != que_ptr_proc) && (que_ptr_tx != que_ptr_rx))
			{
				que_advance_ptr(&que_ptr_tx);
			}
			
			// decrement pending transmission count
			if (TX_status.tx_pending_count > 0)
			{
				TX_status.tx_pending_count--;
			}
			
			// if the tx pending count is zero, we think we've finished with all
			// of the pending transmission jobs for now
			if (TX_status.tx_pending_count == 0)
			{
				UNSET_MCP_JOB(mcp_status.jobs, JOB_TX_PENDING);
			}
			
			// next state should be return to start
			mcp_stm_set_state(&mcp_stm, START);
			
			break;
			
		case READ_RX_0_NORTH:
		/* READ_RX_n_x is the first step in the job for receiving a CAN message 
		 * from an MCP RX buffer. The options for the PDCA transfer should be in place
		 * by this time, but because the pointers are not constants, their addresses
		 * must be provided at this time.
		 */
			// set message direction on the rx_ptr location. Please note that this
			// is the direction the message is headed, ie Northbound/Southbound.
			// Therefore messages received North are set to South direction of travel.
			// 
			set_que_ptr_direction(que_ptr_rx, MCP_DIR_SOUTH);
			
			// set instruction to read rx buffer 0 on mcp
			PDCA_temporary_instruction_tx[0] = MCP_INST_READ_RX_0;
			
			// provide addresses for messages in que
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			PDCA_options_mcp_spi_msg_rx.r_addr = &que_ptr_rx->msg;
			
			// set pdca job, no addresses need to be set, as the options are pointed
			// at the que locations by default. In this receive case, it does not
			// matter what is in the tx buffer, the MCP will ignore the contents and 
			// transmit its own RX buffer contents only
			PDCA_set_job_rx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_msg_tx, &PDCA_options_mcp_spi_msg_rx, MCP_DIR_NORTH);
			
			mcp_stm_set_state(&mcp_stm, READ_RX_0_NORTH_CALLBACK);
			
			break;
			
		case READ_RX_1_NORTH:
			// set message direction on the rx_ptr location. Please note that this
			// is the direction the message is headed, ie Northbound/Southbound.
			// Therefore messages received North are set to South direction of travel.
			//
			set_que_ptr_direction(que_ptr_rx, MCP_DIR_SOUTH);
			
			// set instruction to read rx buffer 0 on mcp
			PDCA_temporary_instruction_tx[0] = MCP_INST_READ_RX_1;
			
			// provide addresses for messages in que
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			PDCA_options_mcp_spi_msg_rx.r_addr = &que_ptr_rx->msg;
			
			// set pdca job, no addresses need to be set, as the options are pointed
			// at the que locations by default. In this receive case, it does not
			// matter what is in the tx buffer, the MCP will ignore the contents and
			// transmit its own RX buffer contents only
			PDCA_set_job_rx(MCP_DEV_NORTH, &PDCA_options_mcp_spi_msg_tx, &PDCA_options_mcp_spi_msg_rx, MCP_DIR_NORTH);
			
			mcp_stm_set_state(&mcp_stm, READ_RX_1_NORTH_CALLBACK);
			
			break;
			
		case READ_RX_0_NORTH_CALLBACK:
		/* READ_RX_n_x_CALLBACK when a message has been received to the que via
		 * PDCA SPI transfer, this state should advance the rx_ptr and set the job
		 * complete for reading the message from the MCP.
		 */
			#if DBG_MCP_STATE //&& DBG_MCP_CAN_RX
			//print the msg received
			print_dbg("\n\r__MSG_RECEIVED__");
			print_array_uint8(que_ptr_rx->msg, MCP_CAN_MSG_SIZE);
			#endif
		
			que_advance_ptr(&que_ptr_rx);
			
			// set job complete
			mcp_stm_unset_job(&mcp_status, JOB_RX_0_NORTH);
			
			#if DBG_TEST_THROUGHPUT
			test_set_received_message_for_transmit();
			#endif
			
			// next state should be START to give a chance for attention to be checked
			mcp_stm_set_state(&mcp_stm, START);
			
			break;
			
		case READ_RX_1_NORTH_CALLBACK:
			#if DBG_MCP_STATE //&& DBG_MCP_CAN_RX
			//print the msg received
			print_dbg("\n\r__MSG_RECEIVED__");
			print_array_uint8(que_ptr_rx->msg, MCP_CAN_MSG_SIZE);
			#endif
			
			que_advance_ptr(&que_ptr_rx);
			
			// set job complete
			mcp_stm_unset_job(&mcp_status, JOB_RX_1_NORTH);
			
			#if DBG_TEST_THROUGHPUT
			test_set_received_message_for_transmit();
			#endif
			
			// next state should be START to give a chance for attention to be checked
			mcp_stm_set_state(&mcp_stm, START);
			
			break;
			
		case READ_RX_0_SOUTH:
			#if DBG_TIME_MCP
			set_timestamp("Read_0_south", Get_sys_count());
			#endif
			//set northbound direction when receiving from south
			set_que_ptr_direction(que_ptr_rx, MCP_DIR_NORTH);
			
			// set instruction to read rx buffer 0 on mcp
			PDCA_temporary_instruction_tx[0] = MCP_INST_READ_RX_0;
			
			// provide addresses for messages in que
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			PDCA_options_mcp_spi_msg_rx.r_addr = &que_ptr_rx->msg;
			
			// set pdca job, no addresses need to be set, as the options are pointed
			// at the que locations by default. In this receive case, it does not
			// matter what is in the tx buffer, the MCP will ignore the contents and
			// transmit its own RX buffer contents only
			PDCA_set_job_rx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_msg_tx, &PDCA_options_mcp_spi_msg_rx, MCP_DIR_SOUTH);
			
			mcp_stm_set_state(&mcp_stm, READ_RX_0_SOUTH_CALLBACK);
			
			break;
			
		case READ_RX_1_SOUTH:
			#if DBG_TIME_MCP
			set_timestamp("Read_1_south", Get_sys_count());
			#endif
		
			//set northbound direction when receiving from south
			set_que_ptr_direction(que_ptr_rx, MCP_DIR_NORTH);
		
			// set instruction to read rx buffer 0 on mcp
			PDCA_temporary_instruction_tx[0] = MCP_INST_READ_RX_1;
		
			// provide addresses for messages in que
			PDCA_options_mcp_spi_msg_tx.r_addr = &que_ptr_tx->msg;
			PDCA_options_mcp_spi_msg_rx.r_addr = &que_ptr_rx->msg;
		
			// set pdca job, no addresses need to be set, as the options are pointed
			// at the que locations by default. In this receive case, it does not
			// matter what is in the tx buffer, the MCP will ignore the contents and
			// transmit its own RX buffer contents only
			PDCA_set_job_rx(MCP_DEV_SOUTH, &PDCA_options_mcp_spi_msg_tx, &PDCA_options_mcp_spi_msg_rx, MCP_DIR_SOUTH);
		
			mcp_stm_set_state(&mcp_stm, READ_RX_1_SOUTH_CALLBACK);
		
			break;
			
		case READ_RX_0_SOUTH_CALLBACK:
			#if DBG_TIME_MCP
			set_timestamp("Read_0_south_b", Get_sys_count());
			#endif
			#if DBG_MCP_STATE //&& DBG_MCP_CAN_RX
			//print the msg received
			print_dbg("\n\r__MSG_RECEIVED__");
			print_array_uint8(que_ptr_rx->msg, MCP_CAN_MSG_SIZE);
			#endif
		
			que_advance_ptr(&que_ptr_rx);
		
			// set job complete
			mcp_stm_unset_job(&mcp_status, JOB_RX_0_SOUTH);
		
			#if DBG_TEST_THROUGHPUT
			test_set_received_message_for_transmit();
			#endif
		
			// next state should be START to give a chance for attention to be checked
			mcp_stm_set_state(&mcp_stm, START);
		
			break;
			
		case READ_RX_1_SOUTH_CALLBACK:
			#if DBG_TIME_MCP
			set_timestamp("Read_1_south_b", Get_sys_count());
			#endif
			#if DBG_MCP_STATE //&& DBG_MCP_CAN_RX
			//print the msg received
			print_dbg("\n\r__MSG_RECEIVED__");
			print_array_uint8(que_ptr_rx->msg, MCP_CAN_MSG_SIZE);
			#endif
			
			que_advance_ptr(&que_ptr_rx);
			
			// set job complete
			mcp_stm_unset_job(&mcp_status, JOB_RX_1_SOUTH);
			
			#if DBG_TEST_THROUGHPUT
			test_set_received_message_for_transmit();
			#endif
			
			// next state should be START to give a chance for attention to be checked
			mcp_stm_set_state(&mcp_stm, START);
			
			break;
			
		default:
			break;
		}
	}
	//else get out and wait
	#if DBG_MCP_STATE
	print_dbg("\n\rLast State on State Loop Exit: ");
	print_dbg_ulong((unsigned long)mcp_stm.mcp_state);
	#endif
	
	
}
