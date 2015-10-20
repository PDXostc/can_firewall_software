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
	gpio_configure_pin(CAR_STBY, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(CAR_INT, GPIO_DIR_INPUT);
	
	gpio_configure_pin(IVI_RESET, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(IVI_STBY, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
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

//TODO: mcp_print_rx_status
#endif

//quick test to make sure that we can get, set, and reset values
void test_mcp_spi_after_reset(void)
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
	mcp_reset_spi(MCP_NORTH);
	//reset wait test:
	//
	delay_us(8);
	
	status = mcp_read_status(MCP_NORTH, MCP_INST_READ_STATUS);
	
	mcp_print_status(status, *MCP_NORTH.id);
	
	print_dbg("\n\rMCP RX STATUS: ");
	print_dbg_short_hex(mcp_read_status(MCP_NORTH, MCP_INST_READ_RX_STATUS));
	
	print_dbg_short_hex(mcp_read_register(MCP_SOUTH, MCP_ADD_CANSTAT));

	mcp_reset_spi(MCP_SOUTH);
	//reset wait test:
	//
	delay_us(8);
	
	status = mcp_read_status(MCP_NORTH, MCP_INST_READ_STATUS);
	
	mcp_print_status(status, *MCP_SOUTH.id);
	
	print_dbg("\n\rMCP RX STATUS: ");
	print_dbg_short_hex(mcp_read_status(MCP_SOUTH, MCP_INST_READ_RX_STATUS));
	
	print_dbg_short_hex(mcp_read_register(MCP_SOUTH, MCP_ADD_CANSTAT));

	
	mcp_set_register(MCP_SOUTH, MCP_ADD_CANCTRL, MCP_VAL_MODE_NORMAL);
	mcp_read_register(MCP_SOUTH, MCP_ADD_CANSTAT);
	mcp_reset_spi(MCP_SOUTH);
	mcp_read_register(MCP_SOUTH, MCP_ADD_CANSTAT);

}
