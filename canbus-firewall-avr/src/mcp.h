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

#define MCP_INST_READ_STATUS	0xB0

#define MCP_INST_BIT_MODIFY		0x05

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

struct spi_device spi_device_MCP_CAN_NORTH_IVI_conf;
struct spi_device spi_device_MCP_CAN_SOUTH_CAR_conf;

void init_mcp_pins(void);
void init_mcp_spi(void);

extern void test_mcp_spi_after_reset(void);
extern void test_mcp_spi_wip(void);
#endif /* MCP_H_ */