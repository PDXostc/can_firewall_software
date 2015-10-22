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

void init_mcp_pins(void)
{
	//set external pins
	gpio_configure_pin(CAR_RESET, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(CAR_STBY, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
	gpio_configure_pin(CAR_INT, GPIO_DIR_INPUT);
	
	gpio_configure_pin(IVI_RESET, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(IVI_STBY, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
	gpio_configure_pin(IVI_INT, GPIO_DIR_INPUT);
	
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
}

void init_mcp_spi(void)
{
	mcp_reset_pin(MCP_NORTH);
	mcp_reset_pin(MCP_SOUTH);
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
	mcp_select(MCP_NORTH);
	mcp_write_single(MCP_INST_DUMMY);
	//delay_us(3);
	mcp_deselect(MCP_NORTH);
	mcp_select(MCP_SOUTH);
	mcp_write_single(MCP_INST_DUMMY);
	//delay_us(3);
	mcp_deselect(MCP_SOUTH);	
}

uint8_t mcp_configure_bit_timings(struct spi_device *device, uint8_t mcp_val_can_rate)
{
	uint8_t result = MCP_RETURN_FAIL;
	
	switch(mcp_val_can_rate)
	{
		case MCP_VAL_CAN_500kbps_CLOCK_16Mhz:
		mcp_set_register(device, MCP_ADD_CNF1, MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF1);
		mcp_set_register(device, MCP_ADD_CNF2, MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF2);
		mcp_set_register(device, MCP_ADD_CNF3, MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF3);
		result = MCP_RETURN_SUCCESS;
		
		#if DBG_MCP
		if (mcp_read_register(device, MCP_ADD_CNF1) != MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF1)
		{
			result += 1;
		}
		if (mcp_read_register(device, MCP_ADD_CNF2) != MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF2)
		{
			result += 1;
		} 
		if (mcp_read_register(device, MCP_ADD_CNF3) != MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF3)
		{
			result += 1;
		}		
		#endif
		
		break;
	}
	
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

void mcp_load_tx_buffer_atmel_to_mcp(struct spi_device *device, can_mob_t *mob, 
		uint8_t instruction)
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
		tx_data[i + 6] = (uint8_t)((mob->can_msg->data.u64 >> (decrementing_shifter * 2)) & 0xFF);
	}
	
}

// uint8_t mcp_init_can(struct spi_device *device, uint8_t mcp_val_can_rate)
// {
// 
// }

void init_mcp_module(void)
{
	init_mcp_pins();
	init_mcp_spi();
}

#if DBG_MCP
void mcp_print_status(uint8_t status, uint8_t device_id)
{
	print_dbg("\n\r__MCP_STATUS__DEVICE_");
	print_dbg_char_hex(device_id);
	print_dbg(" __");
	
	PRINT_NEWLINE()
	print_dbg("CANINTF.RX0IF -- ");
	print_dbg_char_hex(status & 0x01);
	
	PRINT_NEWLINE()
	print_dbg("CANINTF.RX1IF -- ");
	print_dbg_char_hex(status & 0x02);
	
	PRINT_NEWLINE()
	print_dbg("TXB0CNTRL.TXREQ -- ");
	print_dbg_char_hex(status & 0x04);
	
	PRINT_NEWLINE()
	print_dbg("CANINTF.TX0IF -- ");
	print_dbg_char_hex(status & 0x08);
	
	PRINT_NEWLINE()
	print_dbg("TXB1CTRL.TXREQ -- ");
	print_dbg_char_hex(status & 0x10);
	
	PRINT_NEWLINE()
	print_dbg("CANINTIF.TXB1IF -- ");
	print_dbg_char_hex(status & 0x20);
	
	PRINT_NEWLINE()
	print_dbg("TXB2CNTRL.TXREQ -- ");
	print_dbg_char_hex(status & 0x40);
	
	PRINT_NEWLINE()
	print_dbg("CANINTIF.TX2IF -- ");
	print_dbg_char_hex(status & 0x80);
	
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

//TODO: mcp_print_rx_status
#endif

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
	mcp_print_status(status, device->id);
	
	print_dbg("\n\rMCP RX STATUS: ");
	print_dbg_short_hex(mcp_read_status(device, MCP_INST_READ_RX_STATUS));
	
	print_dbg_short_hex(mcp_read_register(device, MCP_ADD_CANSTAT));
	
// 	//test CAN config setting
// 	mcp_configure_bit_timings(device, MCP_VAL_CAN_500kbps_CLOCK_16Mhz);
// 
// 	mcp_print_bit_timings(device->id, 
// 			mcp_read_register(device, MCP_ADD_CNF1),
// 			mcp_read_register(device, MCP_ADD_CNF2),
// 			mcp_read_register(device, MCP_ADD_CNF3));
// 
// 	//test filter and mask setting action
// 	mcp_configure_can_id(device, MCP_ADD_RXF0SIDH, 0xA0A0A0A0, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXF1SIDH, 0xFFFFFFFF, false);
// 	mcp_configure_can_id(device, MCP_ADD_RXF0SIDH, 0xA0A0A0A0, true);
// 	mcp_configure_can_id(device, MCP_ADD_RXF1SIDH, 0xFFFFFFFF, true);

	
// 	mcp_set_register(device, MCP_ADD_CANCTRL, MCP_VAL_MODE_NORMAL);
// 	mcp_read_register(device, MCP_ADD_CANSTAT);
// 	mcp_reset_spi(device);
// 	mcp_read_register(device, MCP_ADD_CANSTAT);

}

void test_setup_mcp_can(struct spi_device *device)
{
	//test CAN config setting
	if(mcp_configure_bit_timings(device, MCP_VAL_CAN_500kbps_CLOCK_16Mhz) == MCP_RETURN_SUCCESS)
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

	//test filter and mask setting action
	//set all for accept standard
	mcp_configure_can_id(device, MCP_ADD_RXF0SIDH, 0x00000000, false);
	mcp_configure_can_id(device, MCP_ADD_RXF1SIDH, 0x00000000, false);
	mcp_configure_can_id(device, MCP_ADD_RXF2SIDH, 0x00000000, false);
	mcp_configure_can_id(device, MCP_ADD_RXF3SIDH, 0x00000000, false);
	mcp_configure_can_id(device, MCP_ADD_RXF4SIDH, 0x00000000, false);
	mcp_configure_can_id(device, MCP_ADD_RXF5SIDH, 0x00000000, false);
	mcp_configure_can_id(device, MCP_ADD_RXM0SIDH, 0x00000000, false);
	mcp_configure_can_id(device, MCP_ADD_RXM1SIDH, 0x00000000, false);
	
	//set rxb0 and rxb1 configurations...
	//TODO
	// if we don't configure the RXBnCTRL, they remain in accept all mode...
	mcp_set_register(device, MCP_ADD_RXB0CTRL, MCP_VAL_RXM_OFF);
	mcp_set_register(device, MCP_ADD_RXB1CTRL, MCP_VAL_RXM_OFF);
	
	//enter normal mode
	if(mcp_set_control_mode(device, MCP_VAL_MODE_NORMAL) == MCP_RETURN_SUCCESS)
	{
		PRINT_NEWLINE()
		print_dbg("\n\rNormal Configuration Accepted...\n\r");
	} else {
		PRINT_NEWLINE()
		print_dbg("\n\rFAILED TO ENTER NORMAL CONFIGURATION MODE!");
	}
}

void test_setup_transmit_mcp_can(struct spi_device *device)
{
	mcp_configure_can_id(device, MCP_ADD_TXB0SIDH, 0x00000000, false);
	//temp manual setting of transmit registers
	mcp_set_register(device, MCP_ADD_TXB0DLC, 0x00);
	//disable interrupts for now
	mcp_set_register(device, MCP_ADD_CANINTE, 0x00);
	
	mcp_configure_can_id(device, MCP_ADD_TXB1SIDH, 0xA5A5A5A5, false);
	mcp_set_register(device, MCP_ADD_TXB1DLC, 0x00);
	
	mcp_configure_can_id(device, MCP_ADD_TXB1SIDH, 0xFFFFFFFF, false);
	mcp_set_register(device, MCP_ADD_TXB2DLC, 0x00);
	
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

void mcp_request_to_send(struct spi_device *device, uint8_t txb)
{
	mcp_select(device);
	
	mcp_write_single(txb);
	
	mcp_deselect(device);
}
