/*
 * mcp_definitions.h
 *
 * Created: 10/20/2015 2:59:41 PM
 *  Author: smiller6
 */ 


#ifndef MCP_DEFINITIONS_H_
#define MCP_DEFINITIONS_H_

//MCP storage parameters
#define MCP_CAN_MSG_SIZE		13 // bytes
#define MCP_CAN_MSG_HEADER_SIZE 5
#define MCP_CAN_MSG_DATA_SIZE	8

//MCP SPI Instructions
#define MCP_INST_RESET			0xC0
#define MCP_INST_READ			0x03

#define MCP_INST_READ_RX_0		0x90	//read from rx buffer 0. sequential reading into next address will yield data...
#define MCP_INST_READ_RX_0D		0x92
#define MCP_INST_READ_RX_1		0x94
#define MCP_INST_READ_RX_1D		0x96

#define MCP_INST_WRITE			0x02

#define MCP_INST_LOAD_TX_0		0x40	//starts at TXBnSIDH, sequential writing will fill buffer with message
#define MCP_INST_LOAD_TX_1		0x42
#define MCP_INST_LOAD_TX_2		0x44

#define MCP_INST_RTS_TXB0		0x81
#define MCP_INST_RTS_TXB1		0x82
#define MCP_INST_RTS_TXB2		0x84
#define MCP_INST_RTS_ALL		0x87

#define MCP_INST_READ_STATUS	0xA0

#define MCP_INST_READ_RX_STATUS	0xB0

#define MCP_INST_BIT_MODIFY		0x05

#define MCP_MASK_CTRL_MODE		0xE0

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
#define MCP_VAL_RTR					(0x40)

// Transmit register enumerations, used when case switching to figure addresses or quick instructions...
#define MCP_ENUM_TXB_0				0
#define MCP_ENUM_TXB_1				1
#define MCP_ENUM_TXB_2				2

#define MCP_VAL_TXRTS_DIGITAL_INPUT	(0x00)
#define MCP_VAL_TXBnCTRL_CLEAR		(0x00)

// Message Receive Registers
// TODO
#define MCP_VAL_RXM_OFF				(0x60) // note: this setting receives ALL messages
#define MCP_VAL_RXM_EXT_ONLY		(0x40) // only receive ext id msgs that pass filter
#define MCP_VAL_RXM_STD_ONLY		(0x20) // only receive std id msgs that pass filter
#define MCP_VAL_RXM_STD_EXT			(0x00) // receive both std and ext id msgs that pass filter

#define MCP_VAL_BUKT_ROLLOVER_EN    (0x40) // RXB0 will rollover and rewrite RXB1 if full

// Acceptance Filter Registers
// TODO

// Bit Time Configuration Registers
//
// CNF1 - Configuration 1
#define MCP_VAL_SJW1				(0x00)
#define MCP_VAL_SJW2				(0x40)
#define MCP_VAL_SJW3				(0x80)
#define	MCP_VAL_SJW4				(0xC0)
// CNF2 - Configuration 2
#define MCP_VAL_BTL_MODE_ENABLE		(0x80)
#define MCP_VAL_BTL_MODE_DISABLE	(Ox00)
// CNF3 - Configuration 3
#define MCP_VAL_SOF_DISABLE			(0x00)
#define MCP_VAL_SOF_ENABLE			(0x80)

#define MCP_VAL_WAKFIL_DISABLE		(0x00)
#define MCP_VAL_WAKFIL_ENABLE		(0x40)


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
#define MCP_VAL_RX0IE				(0x01)
#define MCP_VAL_RX1IE				(0x02)
#define MCP_VAL_TX0IE				(0x04)
#define MCP_VAL_TX1IE				(0x08)
#define MCP_VAL_TX2IE				(0x10)
#define MCP_VAL_ERRIE				(0x20)
#define MCP_VAL_WAKIE				(0x40)
#define MCP_VAL_MERRIE				(0x80)
// combinations
#define MCP_VAL_INT_RX_ENABLE		(0x03) // enable all rx interrupts
#define MCP_VAL_INT_TX_ENABLE		(0x1C) // enable all tx interrupts
#define MCP_VAL_INT_RX_TX_ENABLE	(0x1F) // enable all rx and tx interrupts
#define MCP_VAL_INT_RX_TX_DISABLE	(0x00) // disable all rx and tx interrupts
//
// CANINTF - CAN interrupt flag
// flags are single bits in a register
// flag = bit is set
#define MCP_FLAG_RX0IF				(0x01)
#define MCP_FLAG_RX1IF				(0x02)
#define MCP_FLAG_TX0IF				(0x04)
#define MCP_FLAG_TX1IF				(0x08)
#define MCP_FLAG_TX2IF				(0x10)
#define MCP_FLAG_ERRIF				(0x20)
#define MCP_FLAG_WAKIF				(0x40)
#define MCP_FLAG_MERRIF				(0x80)

// CAN Control Registers
//
// CANSTAT
//
// CANCTRL
// Note: Reading CANSTAT will return values consistent with all modes except PWRUP
#define MCP_VAL_MODE_NORMAL			(0x00)
#define MCP_VAL_MODE_SLEEP			(0x20)
#define MCP_VAL_MODE_LOOPBACK		(0x40)
#define MCP_VAL_MODE_LISTENONLY		(0x60)
#define MCP_VAL_MODE_CONFIG			(0x80)
//
#define MCP_VAL_MODE_PWRUP			(0xE0) // check against pwrup value to confirm that MCP has completed startup
#define MCP_VAL_ABORT_TX			(0x10)
#define MCP_VAL_TX_ONESHOT			(0x08)
#define MCP_VAL_CLKOUT_ENABLE		(0x04)
#define MCP_VAL_CLKOUT_DISABLE		(0x00)
#define MCP_VAL_CLKOUT_PRE_1		(0x00)
#define MCP_VAL_CLKOUT_PRE_2		(0x01)
#define MCP_VAL_CLKOUT_PRE_4		(0x02)
#define MCP_VAL_CLKOUT_PRE_8		(0x03)

/************************************************************************/
/* Bytes and bitwise                                                    */
/************************************************************************/
#define MCP_BYTE_SIDH					(0)
#define MCP_BYTE_SIDL					(1)
#define MCP_BYTE_EID8					(2)
#define MCP_BYTE_EID0					(3)
#define MCP_BYTE_DLC					(4)
#define MCP_BYTE_D0						(5)
#define MCP_BYTE_D1						(6)
#define MCP_BYTE_D2						(7)
#define MCP_BYTE_D3						(8)
#define MCP_BYTE_D4						(9)
#define MCP_BYTE_D5						(10)
#define MCP_BYTE_D6						(11)
#define MCP_BYTE_D7						(12)


// all bits from SIDH register
#define	MCP_MASK_STD_BITS_10_3			(0xFF)
#define MCP_OFFSET_STD_BITS_10_3		(0)

// get the least significant bits from the SIDL register
#define MCP_MASK_STD_BITS_2_0			(0xE0)
// to shift these into least significant position for conversion
#define MCP_OFFSET_OUT_STD_BITS_2_0		(5)

// get the most significant bits of an EID from SIDL
#define MCP_MASK_EID_BITS_17_16			(0x03)
#define MCP_OFFSET_OUT_EID_BITS_17_16	(0)

// get all bits from the EID8 register
#define MCP_MASK_EID_BITS_15_8			(0xFF)
#define MCP_OFFSET_OUT_EID_BITS_15_8	(0)

// get all bits from the EID8 register
#define MCP_MASK_EID_BITS_7_0			(0xFF)
#define MCP_OFFSET_OUT_EID_BITS_7_0		(0)

// eid bit from SIDL register
#define MCP_MASK_IDE_BIT				(0x08)
#define MCP_OFFSET_OUT_IDE_BIT			(3)

// dlc bits from DLC register
#define MCP_MASK_DLC					(0x0F)

#endif /* MCP_DEFINITIONS_H_ */