/*
* mcp.h
*
* Created: 10/14/2015 10:27:41 AM
*  Author: smiller6
*/

#include <asf.h>

#include "conf_debug.h"

#ifndef MCP_H_
#define MCP_H_

#define MCP_SPI_MAX_BAUDRATE		10000000 //10Mhz
#define MCP_SPI_BAUDRATE			9000000	 //9Mhz

#ifndef MCP_SPI
#define MCP_SPI		(&AVR32_SPI0)
#endif

#define RESET_PIN_HOLD_DELAY_US		3
#define RESET_PIN_HOLD_DELAY()	delay_us(RESET_PIN_HOLD_DELAY_US)

#define MCP_CAN_MSG_SIZE		13 // bytes
#define MCP_CAN_MSG_HEADER_SIZE 5
#define MCP_CAN_MSG_DATA_SIZE	8

//following the convention that functions return 0 if successful, >0 for fail states
#define MCP_RETURN_SUCCESS		0
#define MCP_RETURN_FAIL			1

//Atmel SPI modes
//MCP requires mode 0,0 or mode 1,1.
// TODO / NOTE: current testing shows beter using mode 3...need concrete answer for why
//#define MCP_SPI_MODE_0_0		SPI_MODE_1
//#define MCP_SPI_MODE_1_1		SPI_MODE_2
#define MCP_SPI_MODE_1_0		SPI_MODE_3
#define MCP_SPI_MODE		MCP_SPI_MODE_1_0

//MCP SPI Instructions
#define MCP_INST_RESET		0xC0
#define MCP_INST_READ		0x03

#define MCP_INST_READ_RX_0	0x90	//read from rx buffer 0. sequential reading into next address will yield data...
#define MCP_INST_READ_RX_0D	0x92
#define MCP_INST_READ_RX_1	0x94
#define MCP_INST_READ_RX_1D	0x96

#define MCP_INST_WRITE		0x02

#define MCP_INST_LOAD_TX_0	0x40	//starts at TXBnSIDH, sequential writing will fill buffer with message
#define MCP_INST_LOAD_TX_1	0x42
#define MCP_INST_LOAD_TX_2	0x44

#define MCP_INST_RTS_TXB0	0x81
#define MCP_INST_RTS_TXB1	0x82
#define MCP_INST_RTS_TXB2	0x83
#define MCP_INST_RTS_ALL	0x87

#define MCP_INST_READ_STATUS	0xA0

#define MCP_INST_READ_RX_STATUS	0xB0

#define MCP_INST_BIT_MODIFY		0x05

#define MCP_MASK_CTRL_MODE	0xE0

/************************************************************************/
/* MCP Register Addresses                                               */
/************************************************************************/

//0x00 - 0x0F
#define MCP_ADD_RXF0SIDH	0x00
#define MCP_ADD_RXF0SIDL	0x01
#define MCP_ADD_RXF0EID8	0x02
#define MCP_ADD_RXF0EID0	0x03
#define MCP_ADD_RXF1SIDH	0x04
#define MCP_ADD_RXF1SIDL	0x05
#define MCP_ADD_RXF1EID8	0x06
#define MCP_ADD_RXF1EID0	0x07
#define MCP_ADD_RXF2SIDH	0x08
#define MCP_ADD_RXF2SIDL	0x09
#define MCP_ADD_RXF2EID8	0x0A
#define MCP_ADD_RXF2EID0	0x0B
#define MCP_ADD_BFPCTRL		0x0C
#define MCP_ADD_TXRTSCTRL	0x0D
#define MCP_ADD_CANSTAT		0x0E
#define MCP_ADD_CANCTRL		0x0F
//0x10 - 0x1F
#define MCP_ADD_RXF3SIDH	0x10
#define MCP_ADD_RXF3SIDL	0x11
#define MCP_ADD_RXF3EID8	0x12
#define MCP_ADD_RXF3EID0	0x13
#define MCP_ADD_RXF4SIDH	0x14
#define MCP_ADD_RXF4SIDL	0x15
#define MCP_ADD_RXF4EID8	0x16
#define MCP_ADD_RXF4EID0	0x17
#define MCP_ADD_RXF5SIDH	0x18
#define MCP_ADD_RXF5SIDL	0x19
#define MCP_ADD_RXF5EID8	0x1A
#define MCP_ADD_RXF5EID0	0x1B
#define MCP_ADD_TEC			0x1C
#define MCP_ADD_REC			0x1D
//0x20 - 0x2F
#define MCP_ADD_RXM0SIDH	0x20
#define MCP_ADD_RXM0SIDL	0x21
#define MCP_ADD_RXM0EID8	0x22
#define MCP_ADD_RXM0EID0	0x23
#define MCP_ADD_RXM1SIDH	0x24
#define MCP_ADD_RXM1SIDL	0x25
#define MCP_ADD_RXM1EID8	0x26
#define MCP_ADD_RXM1EID0	0x27
#define MCP_ADD_CNF3		0x28
#define MCP_ADD_CNF2		0x29
#define MCP_ADD_CNF1		0x2A
#define MCP_ADD_CANINTE		0x2B
#define MCP_ADD_CANINTF		0x2C
#define MCP_ADD_EFLG		0x2D
//0x30 - 0x3F
#define MCP_ADD_TXB0CTRL	0x30
#define MCP_ADD_TXB0SIDH    0x31
#define MCP_ADD_TXB0SIDL	0X32
#define MCP_ADD_TXB0EID8	0x33
#define MCP_ADD_TXB0EID0	0x34
#define MCP_ADD_TXB0DLC		0x35
#define MCP_ADD_TXB0D0		0x36
#define MCP_ADD_TXB0D1		0x37
#define MCP_ADD_TXB0D2		0x38
#define MCP_ADD_TXB0D3		0x39
#define MCP_ADD_TXB0D4		0x3A
#define MCP_ADD_TXB0D5		0x3B
#define MCP_ADD_TXB0D6		0x3C
#define MCP_ADD_TXB0D7		0x3D
//0x40 - 0x4F
#define MCP_ADD_TXB1CTRL	0x40
#define MCP_ADD_TXB1SIDH    0x41
#define MCP_ADD_TXB1SIDL	0X42
#define MCP_ADD_TXB1EID8	0x43
#define MCP_ADD_TXB1EID0	0x44
#define MCP_ADD_TXB1DLC		0x45
#define MCP_ADD_TXB1D0		0x46
#define MCP_ADD_TXB1D1		0x47
#define MCP_ADD_TXB1D2		0x48
#define MCP_ADD_TXB1D3		0x49
#define MCP_ADD_TXB1D4		0x4A
#define MCP_ADD_TXB1D5		0x4B
#define MCP_ADD_TXB1D6		0x4C
#define MCP_ADD_TXB1D7		0x4D
//0x50 - 0x5F
#define MCP_ADD_TXB2CTRL	0x50
#define MCP_ADD_TXB2SIDH    0x51
#define MCP_ADD_TXB2SIDL	0X52
#define MCP_ADD_TXB2EID8	0x53
#define MCP_ADD_TXB2EID0	0x54
#define MCP_ADD_TXB2DLC		0x55
#define MCP_ADD_TXB2D0		0x56
#define MCP_ADD_TXB2D1		0x57
#define MCP_ADD_TXB2D2		0x58
#define MCP_ADD_TXB2D3		0x59
#define MCP_ADD_TXB2D4		0x5A
#define MCP_ADD_TXB2D5		0x5B
#define MCP_ADD_TXB2D6		0x5C
#define MCP_ADD_TXB2D7		0x5D
//0x60 - 0x6F
#define MCP_ADD_RXB0CTRL	0x60
#define MCP_ADD_RXB0SIDH    0x61
#define MCP_ADD_RXB0SIDL	0X62
#define MCP_ADD_RXB0EID8	0x63
#define MCP_ADD_RXB0EID0	0x64
#define MCP_ADD_RXB0DLC		0x65
#define MCP_ADD_RXB0D0		0x66
#define MCP_ADD_RXB0D1		0x67
#define MCP_ADD_RXB0D2		0x68
#define MCP_ADD_RXB0D3		0x69
#define MCP_ADD_RXB0D4		0x6A
#define MCP_ADD_RXB0D5		0x6B
#define MCP_ADD_RXB0D6		0x6C
#define MCP_ADD_RXB0D7		0x6D
//0x70 - 0x7F
#define MCP_ADD_RXB1CTRL	0x70
#define MCP_ADD_RXB1SIDH    0x71
#define MCP_ADD_RXB1SIDL	0X72
#define MCP_ADD_RXB1EID8	0x73
#define MCP_ADD_RXB1EID0	0x74
#define MCP_ADD_RXB1DLC		0x75
#define MCP_ADD_RXB1D0		0x76
#define MCP_ADD_RXB1D1		0x77
#define MCP_ADD_RXB1D2		0x78
#define MCP_ADD_RXB1D3		0x79
#define MCP_ADD_RXB1D4		0x7A
#define MCP_ADD_RXB1D5		0x7B
#define MCP_ADD_RXB1D6		0x7C
#define MCP_ADD_RXB1D7		0x7D

/************************************************************************/
/* Register Values                                                      */
/************************************************************************/



// Message Transmit Registers
// TODO

// Message Receive Registers
// TODO

// Acceptance Filter Registers
// TODO

// Bit Time Configuration Registers
// 
// CNF1 - Configuration 1
#define MCP_VAL_SJW1		0x00
#define MCP_VAL_SJW2		0x40
#define MCP_VAL_SJW3		0x80
#define	MCP_VAL_SJW4		0xC0
// CNF2 - Configuration 2
// TODO
// CNF3 - Configuration 3
// TODO


// Error Detection Registers
// TODO
// TEC - Transmit Error Counter
// TODO
// REC - Receive Error Counter
// TODO
// EFLG - Error Flag

// Interrupt Registers
// 
// CANINTE - CAN interrupt flag enable
#define MCP_VAL_RX0IE		0x01
#define MCP_VAL_RX1IE		0x02
#define MCP_VAL_TX0IE		0x04
#define MCP_VAL_TX1IE		0x08
#define MCP_VAL_TX2IE		0x10
#define MCP_VAL_ERRIE		0x20
#define MCP_VAL_WAKIE		0x40
#define MCP_VAL_MERRIE		0x80
// combinations
#define MCP_VAL_RX_INT_ENABLE		0x03 // enable all rx interrupts
#define MCP_VAL_TX_INT_ENABLE		0x1C // enable all tx interrupts
#define MCP_VAL_RX_TX_INT_ENABLE	0x1F // enable all rx and tx interrupts
#define MCP_VAL_RX_TX_INT_DISABLE	0x00 // disable all rx and tx interrupts
//
// CANINTF - CAN interrupt flag
// flags are single bits in a register
// flag = bit is set
#define MCP_FLAG_RX0IF		0x01
#define MCP_FLAG_RX1IF		0x02
#define MCP_FLAG_TX0IF		0x04
#define MCP_FLAG_TX1IF		0x08
#define MCP_FLAG_TX2IF		0x10
#define MCP_FLAG_ERRIF		0x20
#define MCP_FLAG_WAKIF		0x40
#define MCP_FLAG_MERRIF		0x80

// CAN Control Registers
// 
// CANSTAT
//
// CANCTRL
// Note: Reading CANSTAT will return values consistent with all modes except PWRUP
#define MCP_VAL_MODE_NORMAL		0x00
#define MCP_VAL_MODE_SLEEP		0x20
#define MCP_VAL_MODE_LOOPBACK	0x40
#define MCP_VAL_MODE_LISTENONLY	0x60
#define MCP_VAL_MODE_CONFIG		0x80
//
#define MCP_VAL_MODE_PWRUP		0xE0 // check against pwrup value to confirm that MCP has completed startup
#define MCP_VAL_ABORT_TX		0x10
#define MCP_VAL_TX_ONESHOT		0x08
#define MCP_VAL_CLKOUT_ENABLE	0x04
#define MCP_VAL_CLKOUT_DISABLE	0x00
#define MCP_VAL_CLKOUT_PRE_1	0x00
#define MCP_VAL_CLKOUT_PRE_2	0x01
#define MCP_VAL_CLKOUT_PRE_4	0x02
#define MCP_VAL_CLKOUT_PRE_8	0x03

struct spi_device spi_device_MCP_CAN_NORTH_IVI_conf;
struct spi_device spi_device_MCP_CAN_SOUTH_CAR_conf;

#define MCP_NORTH &spi_device_MCP_CAN_NORTH_IVI_conf
#define MCP_SOUTH &spi_device_MCP_CAN_SOUTH_CAR_conf

#define MCP_NPCS_NORTH 2
#define MCP_NPCS_SOUTH 0

#define MCP_INST_DUMMY 0x00

void init_mcp_pins(void);

/**
 * \brief Chip select an MCP device
 * 
 * \param device Pointer to struct holding Chip Select id
 * 
 * \return void
 */
static inline void mcp_select(struct spi_device *device)
{
	spi_select_device(MCP_SPI, device);
}

/**
 * \brief Unselect a selected MCP device
 * 
 * \param device Pointer to struct holding Chip Select id
 * 
 * \return void
 */
static inline void mcp_deselect(struct spi_device *device)
{
	spi_deselect_device(MCP_SPI, device);
}

/**
 * \brief Write a single byte of data to SPI and wait until transmission
 * 
 * \param data Byte to be written
 * 
 * \return void
 */
static inline void mcp_write_single(uint8_t data)
{
	spi_write_single(MCP_SPI, data);
	//wait for transmission
	while(!spi_is_tx_empty(MCP_SPI));
}

/**
 * \brief Read a single byte of data from SPI
 * 
 * \param 
 * 
 * \return uint8_t Received byte 
 */
static inline uint8_t mcp_read_single(void)
{
	uint8_t data;
	while(!spi_is_rx_ready(MCP_SPI));
	spi_read_single(MCP_SPI, &data);
	return data;
}

/**
 * \brief Write a byte to SPI and return the byte received
 * 
 * \param in_data Byte to be written
 * 
 * \return uint8_t Byte received
 */
static inline uint8_t mcp_read_write_single(uint8_t in_data)
{
	uint8_t out_data;
	mcp_write_single(in_data);
	out_data = mcp_read_single();
	return out_data;
}

/**
 * \brief Given a register address on an MCP, return value of register
 * 
 * \param device Pointer to struct holding device specific chip select id
 * \param addr Register address in MCP device
 * 
 * \return uint8_t Value of register read
 */
static inline uint8_t mcp_read_register(struct spi_device *device, const uint8_t addr)
{
	uint8_t val;
	
	mcp_select(device);
	
	mcp_write_single(MCP_INST_READ);
	mcp_write_single(addr);
	val = mcp_read_write_single(MCP_INST_DUMMY);
	
	mcp_deselect(device);
	
	return val;
}

/**
 * \brief MCP supports reading of multiple registers. If after a read instruction,
 * bytes are sent, MCP will read value of next consecutive register(s) in memory.
 * \param device Pointer to struct holding device specific chip selection id
 * \param start_addr Address to begin reading from
 * \param rx_buffer Pointer to array where values will be stored
 * \param length Number to be read <= read buffer length
 * 
 * \return void
 */
static inline void mcp_read_registers_consecutive(struct spi_device *device, 
		const uint8_t start_addr, uint8_t *rx_buffer, const uint8_t length)
{
	mcp_select(device);
	
	mcp_write_single(MCP_INST_READ);
	mcp_write_single(start_addr);
	spi_read_packet(MCP_SPI, rx_buffer, length);
	
	mcp_deselect(device);
}

/**
 * \brief Set a specific register value in an MCP device
 * 
 * \param device Pointer to struct holding MCP chip select id
 * \param addr Address of register to set
 * \param val Value to be set
 * 
 * \return void
 */
static inline void mcp_set_register(struct spi_device *device, const uint8_t addr, 
		const uint8_t val)
{
	mcp_select(device);
	
	//possibly refactor this to send this as a single array in loop...
	mcp_write_single(MCP_INST_WRITE);
	mcp_write_single(addr);
	mcp_write_single(val);
	
	mcp_deselect(device);
}

/**
 * \brief MCP supports setting of multiple registers. If after a write instruction,
 * bytes are sent, MCP will set value of next consecutive register(s) in memory.
 * 
 * \param device Pointer to struct holding device specific chip selection id
 * \param start_addr Address to begin setting to
 * \param tx_buffer Array holding byte values to be transmitted
 * \param length Length of transmit buffer array
 * 
 * \return 
 */
static inline void mcp_set_registers_consecutive(struct spi_device *device, const uint8_t start_addr,
		const uint8_t *tx_buffer, const uint8_t length)
{
	mcp_select(device);
	
	mcp_write_single(MCP_INST_WRITE);
	mcp_write_single(start_addr);
	spi_write_packet(MCP_SPI, tx_buffer, length);
	
	mcp_deselect(device);
}

/**
 * \brief MCP allows a shorthand instruction for getting the bytes in an rx buffer.
 * Like other read instructions, this is used as a consecutive read. User should
 * anticipate a full sized CAN message, 13 bytes, and prepare a buffer size accordingly.
 * After the device is deselected following this instruction, the MCP will clear
 * the corresponding rx flag.
 * 
 * \param device Pointer to struct holding device specific chip selection id
 * \param read_instruction Special read rx instruction following the pattern
 * 1001 0nm0 where values for 'n,m' indicate one of the four rx locations.
 * For instance nm 00 starts at RXB0SIDH and nm 10 starts at RXB1SIDH.
 * \param rx_buffer
 * 
 * \return void
 */
static inline void mcp_read_rx_buffer(struct spi_device *device, uint8_t read_instruction,
		uint8_t *rx_buffer)
{
	mcp_select(device);
	
	//rx buffer provided needs to be as large as largest possible size
	//13 bytes = 5 <id + possible eid + dlc> + 8 <data> 
	
	//send instruction to read rx buffer
	mcp_write_single(read_instruction);
	
	//packet read will get the id + eid + dlc, should be 5 bytes
	spi_read_packet(MCP_SPI, rx_buffer, MCP_CAN_MSG_HEADER_SIZE);
	
	//decide based on dlc how many data bytes to copy into buffer
	spi_read_packet(MCP_SPI, rx_buffer, rx_buffer[MCP_CAN_MSG_HEADER_SIZE - 1]);
	
	//instruction clears CANINTF.RXnIF when CS raised at end
	mcp_deselect(device);
}

//TODO: mcp_load_tx_buffer()

//TODO: mcp_init_can()

/**
 * \brief Special bit modify instruction for MCP to adjust single bit value in a register.
 * Please note from the manual:
 *		Executing the Bit Modify command on registers that are not bit-modifiable will
 *		force the mask to FFh. This will allow bytewrites to the registers, not bit modify.
 * 
 * \param device Pointer to struct holding chip selection id of MCP target device
 * \param addr Address of MCP register
 * \param mask Mask applied to value to be set. 
 * \param value Value to be masked, resulting in single bits modified in register.
 * 
 * \return void
 */
static inline void mcp_modify_register(struct spi_device *device, const uint8_t addr, 
		const uint8_t mask, const uint8_t value)
{
	mcp_select(device);
	
	mcp_write_single(MCP_INST_BIT_MODIFY);
	mcp_write_single(addr);
	mcp_write_single(mask);
	mcp_write_single(value);
	
	mcp_deselect(device);
}

/**
 * \brief Set a desired mode for MCP. Function attmepts to verify that desired mode
 * has been set, and will return result.
 * 
 * \param device Pointer to struct holding device specific chip selection id
 * \param mode Byte value indicating desired mode of operation to move into
 * 
 * \return uint8_t 0 == Success, >0 == Fail
 */
static inline uint8_t mcp_set_control_mode(struct spi_device *device, const uint8_t mode)
{
	uint8_t result;
	
	//send new mode as a modification to existing  value in control register
	mcp_modify_register(device, MCP_ADD_CANCTRL, MCP_MASK_CTRL_MODE, mode);
	
	result = mcp_read_register(device, MCP_ADD_CANCTRL);
	result &= MCP_MASK_CTRL_MODE;
	
	if (result == mode)
	{
		return MCP_RETURN_SUCCESS;
	} 
	else
	{
		return MCP_RETURN_FAIL;
	}
}

#if DBG_MCP
//prints readable interpretation of MCP status request
void mcp_print_status(uint8_t status, uint8_t device_id);
#endif

/**
 * \brief Ask MCP device to return quick status information
 * 
 * \param device Struct pointer holding MCP device chip selection id
 * \param instruction Status instruction, either MCP_INST_READ_STATUS or MCP_INST_READ_RX_STATUS
 * 
 * \return uint8_t Byte holding status information
 */
static inline uint8_t mcp_read_status(struct spi_device *device, uint8_t instruction)
{
	uint8_t status;
	
	mcp_select(device);
	mcp_write_single(instruction);
	status = mcp_read_write_single(instruction);	
	mcp_deselect(device);
	
	return status;
}

/**
 * \brief Send a reset instruction over SPI to an MCP device
 * 
 * \param device Pointer to struct holding MCP device chip select id
 * 
 * \return void
 */
static inline void mcp_reset_spi(struct spi_device *device)
{
	mcp_select(device);
	//send reset instruction
	mcp_write_single(MCP_INST_RESET);
	mcp_deselect(device);
}

/**
 * \brief Reset a device using the assigned pin. Pin is driven low for at least
 * 2us, per MCP25625 manual
 * 
 * \param device Pointer to struct containing chip select id of target MCP device
 * 
 * \return void
 */
static inline void mcp_reset_pin(struct spi_device *device)
{
	switch(device->id)
	{
		case MCP_NPCS_NORTH:
		gpio_set_pin_low(IVI_RESET);
		RESET_PIN_HOLD_DELAY();
		gpio_set_pin_high(IVI_RESET);
		break;
		
		case MCP_NPCS_SOUTH:
		gpio_set_pin_low(CAR_RESET);
		RESET_PIN_HOLD_DELAY();
		gpio_set_pin_high(CAR_RESET);
		break;
		
		default:
		break;
	}
}

void init_mcp_spi(void);

extern void init_mcp_module(void);

extern void test_mcp_spi_after_reset(void);
#endif /* MCP_H_ */