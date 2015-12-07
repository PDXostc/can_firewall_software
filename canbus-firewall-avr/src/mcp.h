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
#include "led.h"

//including messages for testing, these should not be included in this file in the future
#include "conf_messages.h"

#ifndef MCP_H_
#define MCP_H_

#define MCP_SPI_MAX_BAUDRATE		10000000 //10Mhz
//#define MCP_SPI_BAUDRATE			9000000	 //9Mhz
//#define MCP_SPI_BAUDRATE			4000000	 //9Mhz
#define MCP_SPI_BAUDRATE			MCP_SPI_MAX_BAUDRATE

#ifndef MCP_SPI
#define MCP_SPI		(&AVR32_SPI0)
#endif

#define MCP_DEFAULT_CAN_RATE_NORTH				MCP_VAL_CAN_1mbps_CLOCK_16Mhz
#define MCP_DEFAULT_CAN_RATE_SOUTH				MCP_VAL_CAN_1mbps_CLOCK_16Mhz

#define MCP_DEFAULT_INTERRUPT_VALUE_NORTH		MCP_VAL_INT_RX_ENABLE
#define MCP_DEFAULT_INTERRUPT_VALUE_SOUTH		MCP_VAL_INT_RX_ENABLE

#define RESET_PIN_HOLD_DELAY_US		3
#define RESET_PIN_HOLD_DELAY()		delay_us(RESET_PIN_HOLD_DELAY_US)

//following the convention that functions return 0 if successful, >0 for fail states
#define MCP_RETURN_SUCCESS		0
#define MCP_RETURN_FAIL			1

//Atmel SPI modes
//MCP requires mode 0,0 or mode 1,1.
// TODO / NOTE: current testing shows beter using mode 3...need concrete answer for why
#define MCP_SPI_MODE_0_0		SPI_MODE_1
#define MCP_SPI_MODE_1_1		SPI_MODE_2
#define MCP_SPI_MODE_1_0		SPI_MODE_3
#define MCP_SPI_MODE			MCP_SPI_MODE_1_0


// spi devices north and south. using Atmel chip select structure
const struct spi_device spi_device_MCP_CAN_NORTH_IVI_conf;
const struct spi_device spi_device_MCP_CAN_SOUTH_CAR_conf;

#define MCP_DEV_NORTH &spi_device_MCP_CAN_NORTH_IVI_conf
#define MCP_DEV_SOUTH &spi_device_MCP_CAN_SOUTH_CAR_conf

#define MCP_NPCS_NORTH 2
#define MCP_NPCS_SOUTH 0

#define MCP_INST_DUMMY 0x00

//LED TESTING
#define USE_LED 0

//RX1 specific configuration format
struct RX_config {
	//rx buffer 0
	uint32_t _RXM0;			// mask 0
	uint32_t _RXF0;			// filter 0
	uint32_t _RXF1;			// filter 1
	
	//rx buffer 1
	uint32_t _RXM1;			// mask 1
	uint32_t _RXF2;			// filter 2
	uint32_t _RXF3;			// filter 3
	uint32_t _RXF4;			// filter 4
	uint32_t _RXF5;			// filter 5
	uint8_t  _RX0_EID;		// extended id bits. shift or mask these out to obtain flags
							// RXM0, 0x01; 
							// RXF0, 0x02; 
							// RXF2, 0x04
	uint8_t  _RX1_EID;		// extended id bits. shift or mask these out to obtain flags
							// RXM1, 0x01; 
							// RXF2, 0x02; 
							// RXF3, 0x04; 
							// RXF4, 0x10;
							// RXF5, 0x20
	uint8_t  _RXB0_BUKT;	// Rollover enable
							// 
	//  filter acceptance setting, can be:
	//		_OFF (accept all messages regardless of filters)
	//		_STD_ONLY (accept only standard format messages)
	//		_EXT_ONLY (accept only extended format messages)
	//		_STD_EXT (accept both standard and extended format according to filters)
	//	NOTE: be advised that using extended format filtering will apply the extended id portion
	//	of the filter to the first two data bytes of a standard message, if set
	uint8_t _MCP_VAL_RX0_CTRL;	
	uint8_t _MCP_VAL_RX1_CTRL;
	} RX_config;
	
struct RX_config rx_config_default;
struct RX_config rx_config_test_01;
// ptr to config we are using
struct RX_config *rx_config_ptr;

//RX_config masks
#define MCP_MASK_RXM0_EID	(0x01)
#define MCP_MASK_RXF0_EID	(0x02)
#define MCP_MASK_RXF1_EID	(0x04)

#define MCP_MASK_RXM1_EID	(0x01)
#define MCP_MASK_RXF2_EID	(0x02)
#define MCP_MASK_RXF3_EID	(0x04)
#define MCP_MASK_RXF4_EID	(0x10)
#define MCP_MASK_RXF5_EID	(0x20)

// MCP CAN message struct. Use arrays of these to form que
struct MCP_CAN_msg {
	//direction, other descriptions to store...
	// bit 0 clear = northbound, set = southbound
	uint8_t attributes;
	uint8_t msg[MCP_CAN_MSG_SIZE];
	};

//NORTHBOUND message, set when receiving from SOUTH controller
#define MCP_MSG_DIR_NORTH 0x00 

//SOUTHBOUND message, set when receiving from NORTH controller
#define MCP_MSG_DIR_SOUTH 0x01

void init_mcp_pins(void);

/**
 * \brief Chip select an MCP device
 * 
 * \param device Pointer to struct holding Chip Select id
 * 
 * \return void
 */
static inline void mcp_select(const struct spi_device *device)
{
	spi_select_device(MCP_SPI, device);

	//test flush the fifo...
	MCP_SPI->rdr;
	MCP_SPI->rdr;
	MCP_SPI->rdr;
	MCP_SPI->rdr;
	
	MCP_SPI->cr |= AVR32_SPI_FLUSHFIFO;
	
	#if USE_LED
	if(device->id > 0) {set_led(LED_01, LED_ON);} else {set_led(LED_02, LED_ON);}
	
	#endif
}

/**
 * \brief Unselect a selected MCP device
 * 
 * \param device Pointer to struct holding Chip Select id
 * 
 * \return void
 */
static inline void mcp_deselect(const struct spi_device *device)
{
	spi_deselect_device(MCP_SPI, device);
	
	#if USE_LED
	if(device->id > 0) {set_led(LED_01, LED_OFF);} else {set_led(LED_02, LED_OFF);}
	
	#endif
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
static inline uint8_t mcp_read_register(const struct spi_device *device, const uint8_t addr)
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
static inline void mcp_read_registers_consecutive(const struct spi_device *device, 
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
static inline void mcp_set_register(const struct spi_device *device, const uint8_t addr, 
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
static inline void mcp_set_registers_consecutive(const struct spi_device *device, const uint8_t start_addr,
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
static inline void mcp_read_rx_buffer(const struct spi_device *device, uint8_t read_instruction,
		uint8_t *rx_buffer)
{
	mcp_select(device);
	
	//rx buffer provided needs to be as large as largest possible size
	//13 bytes = 5 <id + possible eid + dlc> + 8 <data> 
	
	//send instruction to read rx buffer
	mcp_write_single(read_instruction);
	
	//packet read will get the id + eid + dlc, should be 5 bytes
	spi_read_packet(MCP_SPI, rx_buffer, MCP_CAN_MSG_HEADER_SIZE);
	
	//decide based on dlc how many data bytes to copy into buffer, starting at data byte 
	spi_read_packet(MCP_SPI, &rx_buffer[5], (rx_buffer[MCP_CAN_MSG_HEADER_SIZE - 1] & 0x0F));
	
	//instruction clears CANINTF.RXnIF when CS raised at end
	mcp_deselect(device);
}

/**
 * \brief Issue Request to Send instruction to MCP device. Instruction corresponds
 * to MCP instruction format 10000nnn, where each of the lower n bits in logical 1
 * state will signify a request to send on the corresponding TX buffer.
 * 
 * \param device Struct holding chip select id
 * \param txb Special MCP formatted request for buffer(s) we wish to send on
 * 
 * \return void
 */
static inline void mcp_request_to_send(const struct spi_device *device, uint8_t txb)
{
	mcp_select(device);
	
	mcp_write_single(txb);
	
	mcp_deselect(device);
}


/**
 * \brief Set MCP device to desired CAN rate, according to definitions. Bear in mind
 * that the three bit timing registers produce a CAN baud rate = timings/clock.
 * Please see MCP25625 manual Section 3.8 for details.
 * 
 * \param device Struct holding chip select id
 * \param mcp_val_can_rate Enumeration indicating which set of configurations will 
 * be used in CNF registers
 * 
 * \return extern uint8_t Success or fail
 */
extern uint8_t mcp_configure_bit_timings(const struct spi_device *device, uint8_t mcp_val_can_rate);

/**
 * \brief Set four registers xSIDH, xSIDL, xEID8, xEID0 on MCP to either a standard 
 * or extended format id.
 * 
 * \param device Struct holding chip select id
 * \param start_addr Starting location for bank of four registers to target with id bits.
 * \param id 32bit id will be interpreted down to either 11 or 29 bit format
 * \param exide Flag id to be interpreted as either standard or extended
 * 
 * \return void
 */
void mcp_configure_can_id(const struct spi_device *device, uint8_t start_addr, uint32_t id, bool exide);

/**
 * \brief Set RXB0 mask, filters, and control registers according to supplied configuration structure.
 * RXB0 and RXB1 differ in layout and are independently configurable.
 * 
 * \param device Struct holding chip select id
 * \param rx_config Pointer to Configuration structure. Structure holds configuration for both buffers.
 * 
 * \return void
 */
void mcp_configure_rx_0(const struct spi_device *device, struct RX_config *rx_config);

/**
 * \brief Set RXB1 mask, filters, and control registers according to supplied configuration structure.
 * RXB0 and RXB1 differ in layout and are independently configurable.
 * 
 * \param device Struct holding chip select id
 * \param rx_config Pointer to Configuration structure. Structure holds configuration for both buffers.
 * 
 * \return void
 */
void mcp_configure_rx_1(const struct spi_device *device, struct RX_config *rx_config);
/**
 * \brief Special function for converting from proprietary Atmel format CAN message 
 * and MOb storage format to MCP standard format, loading into tx buffer, and issuing a 
 * request to send. After assembling a 13 byte message, uses mcp_load_tx_buffer to
 * write the packets.
 * 
 * \param device Struct holding chip select id
 * \param mob Pointer to Atmel MOb holding CAN message information
 * \param txb_enum Enumerator indicating which tx buffer to load with message
 * \param send_immediate Flag to immediately request to send loaded message
 * 
 * \return extern void
 */
extern void mcp_load_tx_buffer_atmel_to_mcp(const struct spi_device *device, can_mob_t *mob, uint8_t txb_enum, bool send_immediate);


/**
 * \brief Initialize can configuration for MCP target device. Function will attempt
 * to enter confguration mode, which is required for mask, filter, and rate 
 * configuration. If unable to set configuration mode, function will abort with 
 * an early fail. Receive buffers will be configured according to supplied structure.
 * Transmit registers will be cleared. Function will attempt to enter supplied 
 * control mode before exit.
 * 
 * \param device Struct holding chip select id
 * \param mcp_val_can_rate Enumeration indicating desired can baud rate @ clock speed
 * \param rx_config Pointer to structure holding RXB configuration information
 * \param control_mode Desired control mode to set MCP device to at end of function
 * 
 * \return extern uint8_t Success or fail
 */
extern uint8_t mcp_init_can(const struct spi_device *device, uint8_t mcp_val_can_rate, struct RX_config *rx_config, uint8_t control_mode);

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
static inline void mcp_modify_register(const struct spi_device *device, const uint8_t addr, 
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
static inline uint8_t mcp_set_control_mode(const struct spi_device *device, const uint8_t mode)
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
// prints readable interpretation of MCP status request
void mcp_print_status(const struct spi_device *device);
// print bit timing register information
void mcp_print_bit_timings(uint8_t device_id, uint8_t cnf1, uint8_t cnf2, uint8_t cnf3);
// prints any set of registers
void mcp_print_registers(const struct spi_device *device, uint8_t start_addr, int length);
#endif

/**
 * \brief Ask MCP device to return quick status information
 * 
 * \param device Struct pointer holding MCP device chip selection id
 * \param instruction Status instruction, either MCP_INST_READ_STATUS or MCP_INST_READ_RX_STATUS
 * 
 * \return uint8_t Byte holding status information
 */
static inline uint8_t mcp_read_status(const struct spi_device *device, uint8_t instruction)
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
static inline void mcp_reset_spi(const struct spi_device *device)
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
static inline void mcp_reset_pin(const struct spi_device *device)
{
	switch(device->id)
	{
		case MCP_NPCS_NORTH:
		gpio_set_pin_low(IVI_RESET_PIN);
		RESET_PIN_HOLD_DELAY();
		gpio_set_pin_high(IVI_RESET_PIN);
		break;
		
		case MCP_NPCS_SOUTH:
		gpio_set_pin_low(CAR_RESET_PIN);
		RESET_PIN_HOLD_DELAY();
		gpio_set_pin_high(CAR_RESET_PIN);
		break;
		
		default:
		break;
	}
}

/**
 * \brief Resets both MCP devices, and initializes the Atmel SPI peripheral
 * in Master mode. Primes the SPI bus with a single byte for each MCP.
 * 
 * \param 
 * 
 * \return void
 */
void init_mcp_spi(void);

/**
 * \brief Loads 13 byte data array holding CAN message data into MCP transmit buffer.
 * 
 * \param device Struct holding chip select id
 * \param tx_data Pointer to array holding CAN message bytes. Must be formatted to MCP register requirements.
 * \param txb_enum Enumerator indicating which transmit buffer to load
 * \param length Length of message supplied. Usually 13 bytes, can be shortened if DLC less than 8.
 * \param send_immediate Flag to issue request to send instruction after loading
 * 
 * \return void
 */
void mcp_load_tx_buffer(const struct spi_device *device, uint8_t *tx_data, uint8_t txb_enum, uint8_t length, bool send_immediate);

extern void init_mcp_module(void);

extern void mcp_request_to_send(const struct spi_device *device, uint8_t txb);
#if DBG_MCP
// temp test function to get status of MCP chipset, particularly after a reset
extern void test_mcp_spi_after_reset(const struct spi_device *device);
// deprecated function for testing the initialization of the MCP CAN controller
extern void test_setup_mcp_can(struct spi_device *device);
// test the transmission of messages using the MCP
extern void test_setup_transmit_mcp_can(const struct spi_device *device);
// print txb register information
extern void mcp_print_txbnctrl(const struct spi_device *device);
// print error register information
extern void mcp_print_error_registers(const struct spi_device *device);
#endif

#endif /* MCP_H_ */