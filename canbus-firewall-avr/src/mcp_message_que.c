/*
 * mcp_message_que.c
 *
 * Created: 11/12/2015 11:46:10 AM
 *  Author: smiller6
 */ 

#include "pdca_interface.h"
#include "mcp_message_que.h"

volatile struct MCP_message_t mcp_message_que[MCP_QUE_SIZE] = {
		{
		0
		}
	};

volatile pdca_channel_options_t PDCA_options_mcp_spi_msg_rx = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_rx,
	.size = PDCA_SIZE_INST,
	.r_addr = NULL,
	.r_size = MCP_CAN_MSG_SIZE,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_msg_tx = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_tx,
	.size = PDCA_SIZE_INST,
	.r_addr = NULL,
	.r_size = MCP_CAN_MSG_SIZE,
};

uint32_t rx_ptr_count = 0;
uint32_t proc_ptr_count = 0;
uint32_t tx_ptr_count = 0;

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
	#if DBG_RX_PTR
	rx_ptr_count++;
	PRINT_NEWLINE()
	print_dbg_hex(rx_ptr_count);
	#endif
}

void wipe_msg_id(volatile struct MCP_message_t **proc)
{
	// wipe the message in question, it will not be transmitted
	memset((void *)(*proc), 0, sizeof(struct MCP_message_t));
}

extern void init_mcp_message_que(void)
{
	que_ptr_rx = &mcp_message_que[0];
	que_ptr_proc = &mcp_message_que[0];
	que_ptr_tx = &mcp_message_que[0];
	
	PROC_status.proc_pending_count = 0;
	
	TX_status.tx_pending_count = 0;
}
