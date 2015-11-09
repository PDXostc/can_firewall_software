/*
* mcp.c
*
* Created: 10/14/2015 10:28:10 AM
*  Author: smiller6
*/

#include "mcp.h"

//spi device corresponds to spi chip select per board
struct spi_device spi_device_MCP_CAN_NORTH_IVI_conf = {
	.id = 2 //chip select NPCS[2]
};
struct spi_device spi_device_MCP_CAN_SOUTH_CAR_conf = {
	.id = 0 //chip select NPCS[0]
};

//used for testing often
uint8_t tx_zero[13] = {0};


void init_mcp_pins(void)
{
	
	//setup gpio for mcp chips and spi connection
	const gpio_map_t mcp_spi_gpio_map = {
		{	MISO_PIN, 0		//PD01, FUNCTION A
		},
		{	MOSI_PIN, 0		//PD00, FUNCTION A
		},
		{	SCLK_PIN, 0		//PD02, FUNCTION A
		},
		{	NPCS_0_PIN, 0	//PD03, FUNCTION A
		},
		{	NPCS_2_PIN, 1	//PC17, FUNCTION B
		}
	};
	
	//enable spi gpio
	gpio_enable_module(mcp_spi_gpio_map, sizeof(mcp_spi_gpio_map) / sizeof(mcp_spi_gpio_map[0]));
	
	//setup gpio map for mcp interrupts
	const gpio_map_t mcp_int_gpio_map = {
 		{	IVI_INT_PIN,  1 //PC03, FUNCTION B
 		},
		{	CAR_INT_PIN, 1 //PD21, FUNCTION B
		}
	};
	
	//enable interrupt 
	gpio_enable_module(mcp_int_gpio_map, sizeof(mcp_int_gpio_map) / sizeof(mcp_int_gpio_map[0]));
	
	//set external pins
	gpio_configure_pin(CAR_RESET_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(CAR_STBY_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
	//gpio_configure_pin(CAR_INT_PIN, GPIO_DIR_INPUT | GPIO_PULL_UP);
	gpio_enable_pin_pull_up(CAR_INT_PIN);
	
	gpio_configure_pin(IVI_RESET_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(IVI_STBY_PIN, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
// 	//gpio_configure_pin(IVI_INT_PIN, GPIO_DIR_INPUT | GPIO_PULL_UP);
 	gpio_enable_pin_pull_up(IVI_INT_PIN);
	
	
}

void init_mcp_spi(void)
{
	mcp_reset_pin(MCP_DEV_NORTH);
	mcp_reset_pin(MCP_DEV_SOUTH);
	spi_master_init(MCP_SPI);

	//spi_master service enables some settings by default.
	//of note is 'active mode', meaning that the CS remains active after a transfer.
	// if not using this interface, remember to enable active manually,
	// by setting the CSRn.CSAAT bit 1
	spi_master_setup_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf,
	MCP_SPI_MODE, MCP_SPI_BAUDRATE, spi_device_MCP_CAN_NORTH_IVI_conf.id);

	spi_master_setup_device(MCP_SPI, &spi_device_MCP_CAN_SOUTH_CAR_conf,
	MCP_SPI_MODE, MCP_SPI_BAUDRATE, spi_device_MCP_CAN_SOUTH_CAR_conf.id);

	spi_enable(MCP_SPI);

	//prime spi
	mcp_select(MCP_DEV_NORTH);
	mcp_write_single(MCP_INST_DUMMY);
	mcp_deselect(MCP_DEV_NORTH);
	mcp_select(MCP_DEV_SOUTH);
	mcp_write_single(MCP_INST_DUMMY);
	mcp_deselect(MCP_DEV_SOUTH);	
	
	#if DBG_MCP
	print_dbg("\n\r__MCP _STATUS_AFTER_RESET_AND_SPI_");
	mcp_print_status(MCP_DEV_NORTH);
	print_dbg("\n\rCanSTAT REgister");
	mcp_print_registers(MCP_DEV_NORTH, MCP_ADD_CANSTAT, 1);
	print_dbg("\n\rCANINTE Register");
	mcp_print_registers(MCP_DEV_NORTH, MCP_ADD_CANINTE, 1);
	print_dbg("\n\rCANINTF Register");
	mcp_print_registers(MCP_DEV_NORTH, MCP_ADD_CANINTF, 1);
	#endif
}

void init_mcp_module(void)
{
	init_mcp_pins();
	init_mcp_spi();
}

uint8_t mcp_configure_bit_timings(struct spi_device *device, uint8_t mcp_val_can_rate)
{
	uint8_t result = MCP_RETURN_FAIL;
	uint8_t cnf1, cnf2, cnf3;
	
	switch(mcp_val_can_rate)
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
		mcp_set_register(device, MCP_ADD_CNF1, cnf1);
		mcp_set_register(device, MCP_ADD_CNF2, cnf2);
		mcp_set_register(device, MCP_ADD_CNF3, cnf3);
	}
		
		
	#if DBG_MCP
	//confirm settings and adjust result if unexpected values, return value will
	// indicate number of mismatches
	if (mcp_read_register(device, MCP_ADD_CNF1) != cnf1)
	{
		result += 1;
	}
	if (mcp_read_register(device, MCP_ADD_CNF2) != cnf2)
	{
		result += 1;
	} 
	if (mcp_read_register(device, MCP_ADD_CNF3) != cnf3)
	{
		result += 1;
	}		
	#endif
		
	
	return result;	
	
}

//need: because acceptance filter registers not arranged identically,
//	they should be configured separately
//	standard and extended are handled differently:
//		extended applies mask and filter to entire 29 bit id
//		standard applies the extended id field to the first 16 bits of data field...
//		
//	RXB0 has
//		RXF0
//		RXF1
//		RXM0
//	RXB1 has
//		RXF2
//		RXF3
//		RXF4
//		RXF5
//		RXM1
//	
//	methods:
//		configure can id
//		configure rxb0ctrl
//		configure rxb1ctrl
//		
//set filter(device, u8 filter_start_addr, u32 id, bool exide)
// 

//turns out we can use the same function for mask or filter...
void mcp_configure_can_id(struct spi_device *device, uint8_t start_addr,
		uint32_t id, bool exide)
{
	//if extended, treat id as extended and set registers consecutively
	//else, set for standard, ignoring anything higher than standard bits
	// 	
	//values to be loaded and sent in one transmission
	uint8_t can_id[4];
	
	if (exide)
	{
		//set RXFnSIDH, high 10:3 bits of standard
		can_id[0] = (uint8_t) ((id >> 3) & 0xFF);
		//RXFnSIDL,     low 2:0 bits of standard, exide bit, and high 17:16 bits of extended
		can_id[1] = (uint8_t) (((id & 0x07) << 5) | (0x01 << 3) | ((id >> 27) & 0x03));
		//RXFnEID8, high 15:8 bits of exid
		can_id[2] = (uint8_t) ((id >> 19) & 0xFF);
		//RXFnEID0, low 7:0 bits of exid
		can_id[3] = (uint8_t) ((id >> 11) & 0xFF);		
	} 
	else
	{
		//set RXFnSIDH, high 10:3 bits of standard
		can_id[0] = (uint8_t) ((id >> 3) & 0xFF);
		//RXFnSIDL, low 2:0 bits of standard, mask out lower unneeded bits, just in case
		can_id[1] = (uint8_t) (((id & 0x07) << 5) & 0xE0); 
		//RXFnEID8, high 15:8 bits of exid, set to 0, unused
		can_id[2] = 0;
		//RXFnEID0, low 7:0 bits of exid, set to 0, unused
		can_id[3] = 0;
	}
	
	mcp_set_registers_consecutive(device, start_addr, can_id, 4);
	
#if DBG_MCP_CAN_RX
	//print results of this operation:
	PRINT_NEWLINE()
	print_dbg("__Configured: Filter or Mask Registers__");
	PRINT_NEWLINE()
	print_dbg("_Input: ");
	print_dbg_char_hex(id >> 24);
	print_dbg_char_hex(id >> 16);
	print_dbg_char_hex(id >> 8);
	print_dbg_char_hex(id);
	PRINT_NEWLINE()
	print_dbg("_Result: ");
	mcp_print_registers(device, start_addr, 4);
	PRINT_NEWLINE()
	print_dbg("__End Configured__");	
#endif
}

void mcp_configure_rx_0(struct spi_device *device, struct RX_config *rx_config)
{
	mcp_configure_can_id(device, MCP_ADD_RXM0SIDH, rx_config->_RXM0, (rx_config->_RX0_EID) & MCP_MASK_RXM0_EID);
	mcp_configure_can_id(device, MCP_ADD_RXF0SIDH, rx_config->_RXF0, (rx_config->_RX0_EID) & MCP_MASK_RXF0_EID);
	mcp_configure_can_id(device, MCP_ADD_RXF1SIDH, rx_config->_RXF1, (rx_config->_RX0_EID) & MCP_MASK_RXF1_EID);
	mcp_set_register(device, MCP_ADD_RXB0CTRL, (rx_config->_MCP_VAL_RX0_CTRL) | (rx_config->_RXB0_BUKT));
}

void mcp_configure_rx_1(struct spi_device *device, struct RX_config *rx_config)
{
	mcp_configure_can_id(device, MCP_ADD_RXM1SIDH, rx_config->_RXM1, (rx_config->_RX1_EID) & MCP_MASK_RXM1_EID);
	mcp_configure_can_id(device, MCP_ADD_RXF2SIDH, rx_config->_RXF2, (rx_config->_RX1_EID) & MCP_MASK_RXF2_EID);
	mcp_configure_can_id(device, MCP_ADD_RXF3SIDH, rx_config->_RXF3, (rx_config->_RX1_EID) & MCP_MASK_RXF3_EID);
	mcp_configure_can_id(device, MCP_ADD_RXF4SIDH, rx_config->_RXF4, (rx_config->_RX1_EID) & MCP_MASK_RXF4_EID);
	mcp_configure_can_id(device, MCP_ADD_RXF5SIDH, rx_config->_RXF5, (rx_config->_RX1_EID) & MCP_MASK_RXF5_EID);
	mcp_set_register(device, MCP_ADD_RXB1CTRL, rx_config->_MCP_VAL_RX1_CTRL);
}

void mcp_load_tx_buffer_atmel_to_mcp(struct spi_device *device, can_mob_t *mob, 
		uint8_t txb_enum, bool send_immediate)
{
	//probably loading full sized message...
	uint8_t tx_data[13];
	uint32_t id = mob->can_msg->id;
	bool exide = (mob->can_msg->ide_bit == 1);
	uint8_t dlc = mob->dlc; 
	uint8_t rtr = ((uint8_t)(mob->can_msg->rtr_bit)) << 6;
	
	if (exide)
	{
		//set RXFnSIDH, high 10:3 bits of standard
		tx_data[0] = (uint8_t) ((id >> 3) & 0xFF);
		//RXFnSIDL,     low 2:0 bits of standard, exide bit, and high 17:16 bits of extended
		tx_data[1] = (uint8_t) (((id & 0x07) << 5) | (0x01 << 3) | ((id >> 27) & 0x03));
		//RXFnEID8, high 15:8 bits of exid
		tx_data[2] = (uint8_t) ((id >> 19) & 0xFF);
		//RXFnEID0, low 7:0 bits of exid
		tx_data[3] = (uint8_t) ((id >> 11) & 0xFF);
	}
	else
	{
		//set RXFnSIDH, high 10:3 bits of standard
		tx_data[0] = (uint8_t) ((id >> 3) & 0xFF);
		//RXFnSIDL, low 2:0 bits of standard, mask out lower unneeded bits, just in case
		tx_data[1] = (uint8_t) (((id & 0x07) << 5) & 0xE0);
		//RXFnEID8, high 15:8 bits of exid, set to 0, unused
		tx_data[2] = 0;
		//RXFnEID0, low 7:0 bits of exid, set to 0, unused
		tx_data[3] = 0;
	}
	
	//set dlc and rtr
	tx_data[4] = (uint8_t)(dlc | rtr);
	
	//set data bytes for using dlc to count
	if(dlc > 8) dlc = 8;
	int decrementing_shifter = 7;
	for (int i = 0; i < dlc; i++)
	{
		tx_data[i + 5] = (uint8_t)((mob->can_msg->data.u64 >> (decrementing_shifter * 8)) & 0xFF);
		decrementing_shifter--;
	}
	
	// clear TXREQ, if necessary
	// load using quick load instruction
	// assume that requisite interrupts have been enabled by setup functions
	// set TXREQ	
	mcp_load_tx_buffer(device, tx_data, txb_enum, MCP_CAN_MSG_HEADER_SIZE + dlc, send_immediate);
	
}

// generic load tx buffer function to be called by platform specific functions
void mcp_load_tx_buffer(struct spi_device *device, uint8_t *tx_data, uint8_t txb_enum,
		uint8_t length, bool send_immediate)
{
	uint8_t txb_ctrl_addr, txb_load_inst, txb_send_inst;
	
	switch (txb_enum)
	{
		case MCP_ENUM_TXB_0:
		txb_ctrl_addr = MCP_ADD_TXB0CTRL;
		txb_load_inst = MCP_INST_LOAD_TX_0;
		txb_send_inst = MCP_INST_RTS_TXB0;
		break;
		
		case MCP_ENUM_TXB_1:
		txb_ctrl_addr = MCP_ADD_TXB1CTRL;
		txb_load_inst = MCP_INST_LOAD_TX_1;
		txb_send_inst = MCP_INST_RTS_TXB1;
		break;
		
		case MCP_ENUM_TXB_2:
		txb_ctrl_addr = MCP_ADD_TXB2CTRL;
		txb_load_inst = MCP_INST_LOAD_TX_2;
		txb_send_inst = MCP_INST_RTS_TXB2;
		break;
		
		default:
		break;
	}
	
	// we intend to overwrite anything existing in the transmit buffer, so the first
	// action is to reset the TXBnCTRL
	mcp_set_register(device, txb_ctrl_addr, 0x00);
	
	//use quick load instruction to buffer for entire data set and length
	mcp_select(device);
	
	mcp_write_single(txb_load_inst);
	
	spi_write_packet(MCP_SPI, tx_data, length);
	
	mcp_deselect(device);
	
	//if we wish to send immediately, issue request for this buffer
	if (send_immediate)
	{
		mcp_request_to_send(device, txb_send_inst);
	}
	
}

// initialize CAN to desired rate and mode
uint8_t mcp_init_can(struct spi_device *device, uint8_t mcp_val_can_rate, struct RX_config *rx_config, uint8_t control_mode)
{
	uint8_t success = MCP_RETURN_FAIL;
	
	//ensure configuration mode
	success = mcp_set_control_mode(device, MCP_VAL_MODE_CONFIG);
	
	//if could not enter config mode, early fail
	if (success != MCP_RETURN_SUCCESS)
	{
		#if DBG_MCP
		PRINT_NEWLINE()
		print_dbg("UNABLE TO SET CONFIGURATION MODE ON DEVICE: ");
		print_dbg_char_hex(device->id);
		#endif
		return MCP_RETURN_FAIL;
	}
	
// 	/************************************************************************/
// 	/* Temporary Configuration of Interrupts                                */
// 	/************************************************************************/
// 	mcp_set_register(device, MCP_ADD_CANINTE, MCP_VAL_INT_RX_ENABLE);
// 	mcp_set_register(device, MCP_ADD_CANINTF, 0x00);
	
	// set CAN rate
	success = mcp_configure_bit_timings(device, mcp_val_can_rate);
	
#if DBG_MCP
	if(success == MCP_RETURN_SUCCESS)
	{
		print_dbg("\n\rConfigure Bit timings Success...");
		} else {
		print_dbg("\n\rConfigure Bit timings FAIL!...");
	}
	
	mcp_print_bit_timings(device->id,
	mcp_read_register(device, MCP_ADD_CNF1),
	mcp_read_register(device, MCP_ADD_CNF2),
	mcp_read_register(device, MCP_ADD_CNF3)
	);
#endif

	// set filter and acceptance registers. RX_config struct holds mask and filter ids,
	// extended id flags, and desired CTRL register values
	mcp_configure_rx_0(device, rx_config);
	mcp_configure_rx_1(device, rx_config);
	
	// clear transmit buffers
	mcp_set_register(device, MCP_ADD_TXB0CTRL, MCP_VAL_TXBnCTRL_CLEAR);
	mcp_set_registers_consecutive(device, MCP_ADD_TXB0SIDH, tx_zero, MCP_CAN_MSG_SIZE);
	mcp_set_register(device, MCP_ADD_TXB1CTRL, MCP_VAL_TXBnCTRL_CLEAR);
	mcp_set_registers_consecutive(device, MCP_ADD_TXB1SIDH, tx_zero, MCP_CAN_MSG_SIZE);
	mcp_set_register(device, MCP_ADD_TXB2CTRL, MCP_VAL_TXBnCTRL_CLEAR);
	mcp_set_registers_consecutive(device, MCP_ADD_TXB2SIDH, tx_zero, MCP_CAN_MSG_SIZE);
	
	// set ready to send control pins, 0 for digital input means that MCP is not
	// to external pins for RTS requests, using SPI primarily instead.
	mcp_set_register(device, MCP_ADD_TXRTSCTRL, MCP_VAL_TXRTS_DIGITAL_INPUT);
	
	success = mcp_set_control_mode(device, control_mode);
	
#if DBG_MCP
	if(success == MCP_RETURN_SUCCESS)
	{
		PRINT_NEWLINE()
		print_dbg("\n\rNormal Configuration Accepted...\n\r");
	} else {
		PRINT_NEWLINE()
		print_dbg("\n\rFAILED TO ENTER NORMAL CONFIGURATION MODE!");
	}
	print_dbg("\n\rControl Mode Register: ");
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_CANCTRL));
	print_dbg("\n\rControl Status Register: ");
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_CANSTAT));
#endif
	
	return success;
}

#if DBG_MCP
void mcp_print_status(struct spi_device *device)
{
	uint8_t status_byte;
	
	status_byte = mcp_read_status(device, MCP_INST_READ_STATUS);
	
	print_dbg("\n\r__MCP_STATUS__DEVICE_");
	print_dbg_char_hex(device->id);
	print_dbg(" __");
	
	PRINT_NEWLINE()
	print_dbg("CANINTF.RX0IF -- ");
	print_dbg_char_hex(status_byte & 0x01);
	
	PRINT_NEWLINE()
	print_dbg("CANINTF.RX1IF -- ");
	print_dbg_char_hex(status_byte & 0x02);
	
	PRINT_NEWLINE()
	print_dbg("TXB0CNTRL.TXREQ -- ");
	print_dbg_char_hex(status_byte & 0x04);
	
	PRINT_NEWLINE()
	print_dbg("CANINTF.TX0IF -- ");
	print_dbg_char_hex(status_byte & 0x08);
	
	PRINT_NEWLINE()
	print_dbg("TXB1CTRL.TXREQ -- ");
	print_dbg_char_hex(status_byte & 0x10);
	
	PRINT_NEWLINE()
	print_dbg("CANINTIF.TXB1IF -- ");
	print_dbg_char_hex(status_byte & 0x20);
	
	PRINT_NEWLINE()
	print_dbg("TXB2CNTRL.TXREQ -- ");
	print_dbg_char_hex(status_byte & 0x40);
	
	PRINT_NEWLINE()
	print_dbg("CANINTIF.TX2IF -- ");
	print_dbg_char_hex(status_byte & 0x80);
	
	print_dbg("\n\r_END_STATUS__________");
}

void mcp_print_bit_timings(uint8_t device_id, uint8_t cnf1, uint8_t cnf2, uint8_t cnf3)
{
	print_dbg("\n\r__MCP_BIT_TIMINGS__DEVICE_");
	print_dbg_char_hex(device_id);
	print_dbg(" __");
	
	PRINT_NEWLINE()
	print_dbg("CNF1.SJW -- ");
	print_dbg_char_hex(cnf1 & 0xC0);
	print_dbg(" = ");
	print_dbg_ulong(((cnf1 & 0xC0) >> 6) + 1);	
	
	PRINT_NEWLINE()
	print_dbg("CNF1.BRP -- ");
	print_dbg_char_hex(cnf1 & 0x3F);
	print_dbg(" = ");
	print_dbg_ulong(cnf1 & 0x3F);
	
	PRINT_NEWLINE()
	print_dbg("CNF2.BTLMODE -- ");
	print_dbg_char_hex((cnf2 & 0x80) >> 7);
	
	PRINT_NEWLINE()
	print_dbg("CNF2.SAM -- ");
	print_dbg_char_hex((cnf2 & 0x40) >> 6);
	
	PRINT_NEWLINE()
	print_dbg("CNF2.PHSEG1 -- ");
	print_dbg_char_hex(cnf2 & 0x38);
	print_dbg(" = ");
	print_dbg_ulong(((cnf2 & 0x38) >> 3) + 1);
	
	PRINT_NEWLINE()
	print_dbg("CNF2.PRSEG -- ");
	print_dbg_char_hex(cnf2 & 0x07);
	print_dbg(" = ");
	print_dbg_ulong((cnf2 & 0x07) + 1);
	
	PRINT_NEWLINE()
	print_dbg("CNF3.SOF -- ");
	print_dbg_char_hex((cnf3 & 0x80) >> 7);
	
	PRINT_NEWLINE()
	print_dbg("CNF3.WAKFIL -- ");
	print_dbg_char_hex((cnf3 & 0x40) >> 6);
	
	PRINT_NEWLINE()
	print_dbg("CNF2.PHSEG2 -- ");
	print_dbg_char_hex(cnf3 & 0x07);
	print_dbg(" = ");
	print_dbg_ulong((cnf3 & 0x07) + 1);
	
	print_dbg("\n\r_END_BIT_TIMINGS__________");
}

void mcp_print_registers(struct spi_device *device, uint8_t start_addr, int length)
{
	uint8_t rx_buffer[length];
	
	mcp_read_registers_consecutive(device, start_addr, rx_buffer, length);
	
	PRINT_NEWLINE()
	print_dbg("__Register_Consecutive_Values_At_Start_Address: ");
	print_dbg_char_hex(start_addr);
	print_dbg("__");
	
	PRINT_NEWLINE()
	for (int i = 0; i < length; i++)
	{
		print_dbg_char_hex(rx_buffer[i]);
		print_dbg(" | ");
	}
	
	PRINT_NEWLINE()
	print_dbg("__End_Register_Values__");
}

void mcp_print_txbnctrl(struct spi_device *device)
{
	//print tx buffers...
	PRINT_NEWLINE()
	print_dbg("TXB0CTRL");
	PRINT_NEWLINE()
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_TXB0CTRL));
			
	PRINT_NEWLINE()
	print_dbg("TXB2CTRL");
	PRINT_NEWLINE()
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_TXB1CTRL));
			
	PRINT_NEWLINE()
	print_dbg("TXB2CTRL");
	PRINT_NEWLINE()
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_TXB2CTRL));
}

void mcp_print_error_registers(struct spi_device *device)
{
	//print error counts
	PRINT_NEWLINE()
	print_dbg("TEC");
	PRINT_NEWLINE()
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_TEC));
			
	PRINT_NEWLINE()
	print_dbg("REC");
	PRINT_NEWLINE()
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_REC));
			
	PRINT_NEWLINE()
	print_dbg("EFLG");
	PRINT_NEWLINE()
	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_EFLG));
}

//TODO: mcp_print_rx_status
#endif //DBG_MCP

#if DBG_MCP
//quick test to make sure that we can get, set, and reset values
void test_mcp_spi_after_reset(struct spi_device *device)
{
	uint8_t status;
	//send reset command
	//ask for value
	// write register
	// ask for value
	// reset
	// ask for value
	//
	//reset
	mcp_reset_spi(device);
	//reset wait test:
	//
	delay_us(8);
	
	status = mcp_read_status(device, MCP_INST_READ_STATUS);
	
	print_dbg("\n\r:::Status:::");
	mcp_print_status(device);
	
	print_dbg("\n\rMCP RX STATUS: ");
	print_dbg_short_hex(mcp_read_status(device, MCP_INST_READ_RX_STATUS));
	
	print_dbg_short_hex(mcp_read_register(device, MCP_ADD_CANSTAT));

}

//old, use real init_can instead...

// void test_setup_mcp_can(struct spi_device *device)
// {
// 	//test CAN config setting
// 	if(mcp_configure_bit_timings(device, MCP_VAL_CAN_1mbps_CLOCK_16Mhz) == MCP_RETURN_SUCCESS)
// 	{
// 		print_dbg("\n\rConfigure Bit timings Success...");
// 	} else {
// 		print_dbg("\n\rConfigure Bit timings FAIL!...");
// 	}
// 	
// 	mcp_print_bit_timings(device->id,
// 		mcp_read_register(device, MCP_ADD_CNF1),
// 		mcp_read_register(device, MCP_ADD_CNF2),
// 		mcp_read_register(device, MCP_ADD_CNF3)
// 		);
// 	
// 	//test filter and mask setting action
// 	//set all for accept standard
// 	mcp_configure_can_id(device, MCP_ADD_RXF0SIDH, 0x00000000, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXF1SIDH, 0x00000000, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXF2SIDH, 0x00000000, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXF3SIDH, 0x00000000, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXF4SIDH, 0x00000000, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXF5SIDH, 0x00000000, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXM0SIDH, 0x00000000, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXM1SIDH, 0x00000000, false);
// 				
// 	// clear receive buffers to zero for clean testing
// 	mcp_set_registers_consecutive(device, MCP_ADD_RXB0SIDH, tx_zero, MCP_CAN_MSG_SIZE);
// 	mcp_set_registers_consecutive(device, MCP_ADD_RXB1SIDH, tx_zero, MCP_CAN_MSG_SIZE);
// 		
// 	//set rxb0 and rxb1 configurations...
// 	//TODO
// 	// if we don't configure the RXBnCTRL, they remain in accept all mode...
// 	mcp_set_register(device, MCP_ADD_RXB0CTRL, MCP_VAL_RXM_OFF);
// 	mcp_set_register(device, MCP_ADD_RXB1CTRL, MCP_VAL_RXM_OFF);
// 		
// 	//test clear all can rx and tx buffers on startup
// 	mcp_set_register(device, MCP_ADD_TXB0CTRL, 0x00);
// 	mcp_set_registers_consecutive(device, MCP_ADD_TXB0SIDH, tx_zero, 13);
// 	mcp_set_register(device, MCP_ADD_TXB1CTRL, 0x00);
// 	mcp_set_registers_consecutive(device, MCP_ADD_TXB1SIDH, tx_zero, 13);
// 	mcp_set_register(device, MCP_ADD_TXB2CTRL, 0x00);
// 	mcp_set_registers_consecutive(device, MCP_ADD_TXB2SIDH, tx_zero, 13);
// 		
// 	//test forcing control mode and entire CANCTRL register...
// 	//enter normal mode and set 0 in osm and clkout
// 	// 
// 	//mcp_set_register(device, MCP_ADD_CANCTRL, 0x00 /*| MCP_VAL_TX_ONESHOT*/);
// 	// 
// 	// set for loopback during testing!!
// 	//mcp_set_register(device, MCP_ADD_CANCTRL, 0x40 | MCP_VAL_TX_ONESHOT);
// 	
// 	//mcp_modify_register(device, MCP_ADD_CANCTRL, MCP_MASK_CTRL_MODE, MCP_VAL_MODE_NORMAL);
// 	// 
// 	//mcp_set_control_mode(device, MCP_VAL_MODE_NORMAL);
// 	if(mcp_set_control_mode(device, MCP_VAL_MODE_NORMAL) == MCP_RETURN_SUCCESS)
// 	{
// 		PRINT_NEWLINE()
// 		print_dbg("\n\rNormal Configuration Accepted...\n\r");
// 	} else {
// 		PRINT_NEWLINE()
// 		print_dbg("\n\rFAILED TO ENTER NORMAL CONFIGURATION MODE!");
// 	}
// 	
// 	print_dbg("\n\rControl Mode Register: ");
// 	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_CANCTRL));
// 	print_dbg("\n\rControl Status Register: ");
// 	print_dbg_char_hex(mcp_read_register(device, MCP_ADD_CANSTAT));
// 
// }

//still useful until we are using the que logic to upload the tx messages
void test_setup_transmit_mcp_can(struct spi_device *device)
{
	//disable interrupts for now -- !!
	//mcp_set_register(device, MCP_ADD_CANINTE, MCP_VAL_INT_RX_TX_DISABLE);
	
	mcp_load_tx_buffer_atmel_to_mcp(device, &mob_test_7ff, MCP_ENUM_TXB_0, true);
	
	mcp_load_tx_buffer_atmel_to_mcp(device, &mob_test_a5, MCP_ENUM_TXB_1, true);
	
	mcp_load_tx_buffer_atmel_to_mcp(device, &mob_test_00, MCP_ENUM_TXB_2, true);
	
	
	
#if DBG_MCP_CAN_TX
	PRINT_NEWLINE()
	print_dbg("TXB0");
	mcp_print_registers(device, MCP_ADD_TXB0SIDH, 13);
	
	PRINT_NEWLINE()
	print_dbg("TXB1");
	mcp_print_registers(device, MCP_ADD_TXB1SIDH, 13);
	
	PRINT_NEWLINE()
	print_dbg("TXB2");
	mcp_print_registers(device, MCP_ADD_TXB2SIDH, 13);
#endif
}

#endif //DBG_MCP

