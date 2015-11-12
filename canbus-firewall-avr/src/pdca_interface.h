/*
 * pdca_interface.h
 *
 * Created: 11/10/2015 8:36:31 AM
 *  Author: smiller6
 */ 


#ifndef PDCA_INTERFACE_H_
#define PDCA_INTERFACE_H_

#include <asf.h>

/************************************************************************/
/* PDCA                                                                 */
/************************************************************************/
#define PDCA_ID_SPI_RX				AVR32_PDCA_PID_SPI0_RX
#define PDCA_ID_SPI_TX				AVR32_PDCA_PID_SPI0_TX
#define PDCA_TRANSFER_SIZE_BYTE		AVR32_PDCA_BYTE
#define PDCA_CHANNEL_SPI_TX			0
#define PDCA_CHANNEL_SPI_RX			1

// PDCA sizes for buffers. These inform the size of the PDCA transfer.
// Sizes must account for desired size, plus size of padding resulting from dummy
// bytes to prompt rx or tx of multiple bytes on the SPI bus. For instance, a
// transaction to receive a CAN msg using the PDCA should be the size of the CAN
// msg, plus the size of the Receive Instruction sent. In our case, the MCP
// supports a single byte for the receive instruction, meaning that our received
// CAN msg will have a single byte of dummy padding at the beginning of its array.
// Please note that any array sized to hold this message should also account for
// this dummy byte.
//
// Expected size of instruction
#define PDCA_SIZE_INST		1
// Expected size of CAN msg
#define PDCA_SIZE_MSG		13
// Size of status byte
#define PDCA_SIZE_STATUS	1
// Size of error register byte
#define PDCA_SIZE_ERROR		1



// Combined sizes for full transaction
#define PDCA_SIZE_TRANS_MSG					(PDCA_SIZE_INST	+ PDCA_SIZE_MSG)
#define PDCA_SIZE_TRANS_SINGLE_INST			(PDCA_SIZE_INST + 1)
#define PDCA_SIZE_TRANS_WRITE_SINGLE_REG	(3) // write + addr + value
#define PDCA_SIZE_TRANS_STATUS				(PDCA_SIZE_INST + PDCA_SIZE_STATUS)
#define PDCA_SIZE_TRANS_ERROR				(PDCA_SIZE_INST + PDCA_SIZE_ERROR)
#define PDCA_SIZE_TRANS_BIT_MODIFY			(4) // inst + addr + mask + value
#define PDCA_SIZE_TRANS_TIMING				(5) // write + addr + CNF3 + CNF2 +CNF1

// temp storage for single instruction / response bytes. PDCA will place raw SPI
// transfer results here, mcp state machine should immediately copy relevant byte(s)
// to corresponding status byte when called back.
// Storage is left large in rare case that many registers need to be downloaded
#define PDCA_TEMP_TRANSFER_BUFFER_SIZE		16

volatile uint8_t PDCA_temporary_receive[PDCA_TEMP_TRANSFER_BUFFER_SIZE];
volatile uint8_t PDCA_temporary_instruction_tx[PDCA_SIZE_TRANS_SINGLE_INST];
volatile uint8_t PDCA_temporary_instruction_rx[PDCA_SIZE_TRANS_SINGLE_INST];
volatile uint8_t PDCA_temporary_bit_modify_tx[PDCA_SIZE_TRANS_BIT_MODIFY];
volatile uint8_t PDCA_temporary_write_single[PDCA_SIZE_TRANS_WRITE_SINGLE_REG];

struct PDCA_status_t {
	uint8_t PDCA_busy;
	};

volatile struct PDCA_status_t pdca_status;

// PDCA test
// create rx / tx temp buffers, delete when testing complete
uint8_t rx_instruction_test[14];
uint8_t rx_msg_test[14];
extern volatile bool pdca_test_transfer_complete;

extern pdca_channel_options_t PDCA_options_mcp_spi_msg_rx;
extern pdca_channel_options_t PDCA_options_mcp_spi_msg_tx;
extern pdca_channel_options_t PDCA_options_mcp_spi_rx_single;
extern pdca_channel_options_t PDCA_options_mcp_spi_tx_single;
extern pdca_channel_options_t PDCA_options_mcp_spi_tx_write_single_register;
extern pdca_channel_options_t PDCA_options_mcp_spi_tx_write_single_instruction;
extern pdca_channel_options_t PDCA_options_mcp_spi_tx_bit_modify;
extern pdca_channel_options_t PDCA_options_mcp_spi_tx_configure_timings;
extern pdca_channel_options_t PDCA_options_mcp_spi_rx_get_status_north;
extern pdca_channel_options_t PDCA_options_mcp_spi_rx_get_status_south;
extern pdca_channel_options_t PDCA_OPTIONS_rx_test;
extern pdca_channel_options_t PDCA_OPTIONS_tx_test;



#endif /* PDCA_INTERFACE_H_ */