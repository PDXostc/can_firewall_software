/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * pdca_interface.c
 *
 */ 

//#include "pdca_interface.h"
#include "interrupt_machines.h"

volatile struct PDCA_status_t pdca_status = {
	.PDCA_busy = 0x00,
	};

// PDCA channel settings, uses Atmel convention struct
volatile pdca_channel_options_t PDCA_options_mcp_spi_rx_single = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_SINGLE_INST,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_tx_single = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_SINGLE_INST,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_tx_write_single_register = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_write_single,
	.size = PDCA_SIZE_TRANS_WRITE_SINGLE_REG,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_tx_write_single_instruction = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_tx,
	.size = PDCA_SIZE_INST,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_tx_write_single_instruction_single_response = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_tx,
	.size = PDCA_SIZE_TRANS_SINGLE_INST,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_tx_bit_modify = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_bit_modify_tx,
	.size = PDCA_SIZE_TRANS_BIT_MODIFY,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_tx_configure_timings = {
	.pid = PDCA_ID_SPI_TX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = NULL,
	.size = PDCA_SIZE_TRANS_TIMING,
	.r_addr = NULL,
	.r_size = 0,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_rx_get_status_north = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_rx,
	.size = 1,
	.r_addr = &mcp_status.status_byte_north,
	.r_size = 1,
};

volatile pdca_channel_options_t PDCA_options_mcp_spi_rx_get_status_south = {
	.pid = PDCA_ID_SPI_RX,
	.transfer_size = PDCA_TRANSFER_SIZE_BYTE,
	.addr = &PDCA_temporary_instruction_rx,
	.size = 1,
	.r_addr = &mcp_status.status_byte_south,
	.r_size = 1,
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

