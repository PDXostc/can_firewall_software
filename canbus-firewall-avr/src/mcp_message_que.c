/*
 * mcp_message_que.c
 *
 * Created: 11/12/2015 11:46:10 AM
 *  Author: smiller6
 */ 

/* We are trying an experiment here, to always have the SPI transaction settings
 * Receive to where ever the rx_ptr is, and transmit whatever is pointed to by
 * the tx_ptr. This means that extra care should be taken that the PDCA is not set
 * for reception when we want to upload a transmit message, or the rx_ptr location
 * would be overwritten with dummy bytes. Currently, our implementation for transmit
 * and receive using PDCA protects against this case, only enabling rx when we want
 * to receive a message.
 */

#include "pdca_interface.h"
#include "mcp_message_que.h"

volatile struct MCP_message_t mcp_message_que[MCP_QUE_SIZE] = {0};

pdca_channel_options_t PDCA_options_mcp_spi_msg_rx = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_rx,
	.size = PDCA_SIZE_INST,
	.r_addr = NULL,
	.r_size = MCP_CAN_MSG_SIZE,
};

pdca_channel_options_t PDCA_options_mcp_spi_msg_tx = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_tx,
	.size = PDCA_SIZE_INST,
	.r_addr = NULL,
	.r_size = MCP_CAN_MSG_SIZE,
};

uint32_t rx_ptr_count = 0;

//advance pointer, given pointer address. checks for bounds of que and resets position if necessary
void que_advance_ptr(volatile struct MCP_message_t **ptr)
{
	if (*ptr >= &mcp_message_que[MCP_QUE_SIZE - 1])
	{
		*ptr = &mcp_message_que[0];
	}
	else
	{
		*ptr = *ptr + 1;
	}
	//test pointer counting
	rx_ptr_count++;
	#if DBG_RX_PTR
	PRINT_NEWLINE()
	print_dbg_hex(rx_ptr_count);
	#endif
}

extern void init_message_que(void)
{
	que_ptr_rx = &mcp_message_que[0];
	que_ptr_proc = &mcp_message_que[0];
	que_ptr_tx = &mcp_message_que[0];
}
