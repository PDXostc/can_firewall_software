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
	spi_master_init(MCP_SPI);
	
	//spi_master service enables some settings by default.
	//of note is 'active mode', meaning that the CS remains active after a transfer.
	// if not using this interface, remember to enable active manually,
	// by setting the CSRn.CSAAT bit 1
	spi_master_setup_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf,
	MCP_SPI_MODE, MCP_SPI_BAUDRATE, 0);
	
	spi_enable(MCP_SPI);
	
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	delay_ms(1);
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	spi_write_packet(MCP_SPI, 0, 1);
	delay_ms(1);
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	
}

void test_mcp_spi_after_reset(void)
{
	//send reset command
	//ask for value
	// write register
	// ask for value
	// reset
	// ask for value
	//
	//
	uint8_t rx[1];
	uint8_t tx[1] = {MCP_INST_RESET};
	//reset
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	delay_us(3);
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	spi_write_packet(MCP_SPI, tx, 1);
	delay_us(3);
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	delay_us(2000);
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	spi_write_single(MCP_SPI, 0x03);
	delay_us(3);
	spi_write_single(MCP_SPI, 0x0F);
	delay_us(3);
	spi_write_single(MCP_SPI, 0x00);
	delay_us(3); //allow time for reset
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	delay_us(3);
	
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	delay_ms(1);
	tx[0] = MCP_INST_READ;
	spi_write_packet(MCP_SPI, tx, 1);
	delay_ms(1);
	//read to address of CANFG1
	tx[0] = MCP_ADD_CANCTRL;
	spi_write_packet(MCP_SPI, tx, 1);
	delay_ms(1);
	spi_read_packet(MCP_SPI, &rx[0], 1);
	delay_ms(1);
	print_dbg_short_hex(rx[0]);
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
}

void test_mcp_spi_wip(void)
{
	//send reset command
	//ask for value
	// write register
	// ask for value
	// reset
	// ask for value
	//
	//
	uint8_t rx[1];
	uint8_t tx[1] = {MCP_INST_RESET};
	//reset
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	spi_write_packet(MCP_SPI, tx, 1);
	delay_ms(10); //allow time for reset
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	tx[0] = MCP_INST_READ;
	spi_write_packet(MCP_SPI, tx, 1);
	//read to address of CANFG1
	tx[0] = MCP_ADD_CNF1;
	spi_write_packet(MCP_SPI, tx, 1);
	spi_read_single(MCP_SPI, &rx[0]);
	print_dbg_short_hex(rx[0]);
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	
	//write to CANF1
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	tx[0] = MCP_INST_WRITE;
	spi_write_packet(MCP_SPI, tx, 1);
	tx[0] = MCP_ADD_CNF1;
	spi_write_packet(MCP_SPI, tx, 1);
	tx[0] = 0x11;
	spi_write_packet(MCP_SPI, tx, 1);
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	
	//read to make sure we got the write correct
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	tx[0] = MCP_INST_READ;
	spi_write_packet(MCP_SPI, tx, 1);
	//read to address of CANFG1
	tx[0] = MCP_ADD_CNF1;
	spi_write_packet(MCP_SPI, tx, 1);
	spi_read_single(MCP_SPI, &rx[0]);
	print_dbg_short_hex(rx[0]);
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	
	//reset
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	spi_write_packet(MCP_SPI, tx, 1);
	delay_ms(10); //allow time for reset
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	
	//read to make sure we got the write correct
	spi_select_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	tx[0] = MCP_INST_READ;
	spi_write_packet(MCP_SPI, tx, 1);
	//read to address of CANFG1
	tx[0] = MCP_ADD_CNF1;
	spi_write_packet(MCP_SPI, tx, 1);
	spi_read_single(MCP_SPI, &rx[0]);
	print_dbg_short_hex(rx[0]);
	spi_deselect_device(MCP_SPI, &spi_device_MCP_CAN_NORTH_IVI_conf);
	
}