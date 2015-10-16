/*
 * mcp.h
 *
 * Created: 10/14/2015 10:27:41 AM
 *  Author: smiller6
 */ 

#include <asf.h>

#ifndef MCP_H_
#define MCP_H_

#define MCP_SPI_MAX_BAUDRATE		10000000 //10Mhz
#define MCP_SPI_BAUDRATE			9000000	 //9Mhz

#ifndef MCP_SPI
#define MCP_SPI		(&AVR32_SPI0)
#endif

#define SPI_DELAY_US	3
#define SPI_DELAY	delay_us(SPI_DELAY_US);

//#define MCP_RESET_DELAY_US	//NOT SET, TBD
//#define MCP_RESET_DELAY	delay_us(SPI_RESET_DELAY_US);

//Atmel SPI modes
//MCP requires mode 0,0 or mode 1,1. 
// TODO / NOTE: current testing shows beter using mode 3...need concrete answer for why
#define MCP_SPI_MODE_0_0		SPI_MODE_1
#define MCP_SPI_MODE_1_1		SPI_MODE_2
#define MCP_SPI_MODE_1_0		SPI_MODE_3

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

#define MCP_INST_READ_STATUS	0xB0

#define MCP_INST_BIT_MODIFY		0x05

//MCP register addresses
#define MCP_ADD_CANCTRL		0x0F
#define MCP_ADD_CNF1		0x2A

struct spi_device spi_device_MCP_CAN_NORTH_IVI_conf;
struct spi_device spi_device_MCP_CAN_SOUTH_CAR_conf;

void init_mcp_pins(void);
void init_mcp_spi(void);

extern void test_mcp_spi_after_reset(void);
extern void test_mcp_spi_wip(void);
#endif /* MCP_H_ */