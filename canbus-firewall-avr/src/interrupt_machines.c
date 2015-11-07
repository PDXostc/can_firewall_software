/*
 * interrupt_machines.c
 *
 * Created: 11/5/2015 1:44:39 PM
 *  Author: smiller6
 */ 
#include "interrupt_machines.h"

volatile bool pdca_test_transfer_complete = false;

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
	mcp_set_register(MCP_NORTH, MCP_ADD_CANINTF, 0x00);
	
	//PDCA on interrupt test
	
		rx_instruction_test[0] = MCP_INST_READ_RX_0;
		// update pdca_options
		PDCA_OPTIONS_tx_test.addr = &rx_instruction_test;
		PDCA_OPTIONS_tx_test.size = sizeof(rx_instruction_test);
		
		PDCA_OPTIONS_rx_test.addr = &rx_msg_test;
		PDCA_OPTIONS_rx_test.size = sizeof(rx_msg_test);
		
		
		pdca_init_channel(PDCA_CHANNEL_SPI_TX, &PDCA_OPTIONS_tx_test);
		pdca_init_channel(PDCA_CHANNEL_SPI_RX, &PDCA_OPTIONS_rx_test);
		
		//register interrupt for rx complete...
		/*INTC_register_interrupt(&pdca_transfer_complete_int_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);*/
		INTC_register_interrupt(&pdca_transfer_complete_int_handler, AVR32_PDCA_IRQ_1, AVR32_INTC_INT1);;
		
		//pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_TX);
		pdca_enable_interrupt_transfer_complete(PDCA_CHANNEL_SPI_RX);
		//
		
		//chip select mcp
		mcp_select(MCP_NORTH);
		
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
	mcp_set_register(MCP_SOUTH, MCP_ADD_CANINTF, 0x00);
	// clear external interrupt line
	// ask for mcp int status
	
	mcp_print_status(MCP_SOUTH);
	// test: display status:
	#if DBG_MCP
	
	mcp_print_status(MCP_SOUTH);
	print_dbg("\n\rCanSTAT REgister");
	mcp_print_registers(MCP_SOUTH, MCP_ADD_CANSTAT, 1);
	print_dbg("\n\rCANINTE Register");
	mcp_print_registers(MCP_SOUTH, MCP_ADD_CANINTE, 1);
	print_dbg("\n\rCANINTF Register");
	mcp_print_registers(MCP_SOUTH, MCP_ADD_CANINTF, 1);
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
	
	gpio_set_pin_high(MCP_MACHINE_INT_PIN);
	gpio_clear_pin_interrupt_flag(MCP_MACHINE_INT_PIN);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void pdca_transfer_complete_int_handler(void)
{
	//handle transfer complete
	pdca_test_transfer_complete = true;
	//volatile avr32_pdca_channel_t *pdca = pdca_get_handler(PDCA_CHANNEL_SPI_TX);
	volatile avr32_pdca_channel_t *pdca = pdca_get_handler(PDCA_CHANNEL_SPI_RX);
	pdca->idr = 0x07;

	pdca_disable(PDCA_CHANNEL_SPI_TX);
	pdca_disable(PDCA_CHANNEL_SPI_RX);
	mcp_deselect(MCP_NORTH);
}
