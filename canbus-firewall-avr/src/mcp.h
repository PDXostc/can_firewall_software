/*
* mcp.h
*
* Created: 10/14/2015 10:27:41 AM
*  Author: smiller6
*/

#include <asf.h>

#include "conf_debug.h"
#include "mcp_definitions.h"
#include "conf_can_mcp.h"
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
#define MCP_SPI_MODE			 MCP_SPI_MODE_1_0



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

//TODO: mcp_configure_can_timings()
extern uint8_t mcp_configure_bit_timings(struct spi_device *device, uint8_t mcp_val_can_rate);

//TODO: mcp_init_can()
extern uint8_t mcp_init_can(struct spi_device *device, uint8_t mcp_val_can_rate);

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
void mcp_print_bit_timings(uint8_t device_id, uint8_t cnf1, uint8_t cnf2, uint8_t cnf3);
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