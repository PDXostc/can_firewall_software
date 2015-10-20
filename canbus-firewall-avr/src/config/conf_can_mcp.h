/*
 * conf_can_mcp.h
 *
 * Created: 10/20/2015 1:42:07 PM
 *  Author: smiller6
 */ 


#ifndef CONF_CAN_MCP_H_
#define CONF_CAN_MCP_H_

#include "mcp_definitions.h"

/************************************************************************/
/* CNF1
 * bit 7-6 SJW<1:0>: Synchronization Jump Width Length bits
 * 11 = Length = 4 x TQ
 * 10 = Length = 3 x TQ
 * 01 = Length = 2 x TQ
 * 00 = Length = 1 x TQ
 * 
 * bit 5-0 BRP<5:0>: Baud Rate Prescaler bits
 * TQ = 2 x (BRP + 1)/FOSC 
 * */
/************************************************************************/

#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_BRP		(0x00)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_SJW		(0x04)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF1	(MCP_VAL_CAN_500kbps_CLOCK_16Mhz_BRP |\
												 MCP_VAL_CAN_500kbps_CLOCK_16Mhz_SJW)

/************************************************************************/
/* CNF2
* bit 7 BTLMODE: PS2 Bit Time Length bit
* 1 = Length of PS2 determined by PHSEG2<2:0> bits of CNF3
* 0 = Length of PS2 is the greater of PS1 and IPT (2 TQ)
* 
* bit 6 SAM: Sample Point Configuration bit
* 1 = Bus line is sampled three times at the sample point
* 0 = Bus line is sampled once at the sample point
* 
* bit 5-3 PHSEG1<2:0>: PS1 Length bits
* (PHSEG1 + 1) x TQ
* 
* bit 2-0 PRSEG<2:0>: Propagation Segment Length bits
* (PRSEG + 1) x TQ
 * */
/************************************************************************/

#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_BTLMODE		(MCP_VAL_BTL_MODE_ENABLE)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_PRSEG		(0x07)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_PHSEG1		(0x04 << 3)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF2		(MCP_VAL_CAN_500kbps_CLOCK_16Mhz_PRSEG |\
													 MCP_VAL_CAN_500kbps_CLOCK_16Mhz_PHSEG1)


/************************************************************************/
/* CNF3
* bit 7 SOF: Start-of-Frame Signal bit
* If in CANCTRL Register, CLKEN = 1:
* 1 = CLKOUT pin enabled for SOF signal
* 0 = CLKOUT pin enabled for clockout function
* If CANCTRL Register, CLKEN = 0, Bit is don’t care.
* 
* bit 6 WAKFIL: Wake-up Filter bit
* 1 = Wake-up filter enabled
* 0 = Wake-up filter disabled
* 
* bit 5-3 Unimplemented: Reads as ‘0’
* 
* bit 2-0 PHSEG2<2:0>: PS2 Length bits
* (PHSEG2 + 1) x TQ
* Minimum valid setting for PS2 is 2 TQ
 * */
/************************************************************************/

//currently, default communication involves clock out start of frame disabled and wakeup filter disabled
//wakeup filter should be set prior to going to sleep

#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_SOF			(MCP_VAL_SOF_DISABLE)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_WAKFIL		(MCP_VAL_WAKFIL_DISABLE)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_PHSEG		(0x04)
#define MCP_VAL_CAN_500kbps_CLOCK_16Mhz_CNF3		(MCP_VAL_CAN_500kbps_CLOCK_16Mhz_PHSEG |\
													 MCP_VAL_CAN_500kbps_CLOCK_16Mhz_SOF |\
													 MCP_VAL_CAN_500kbps_CLOCK_16Mhz_WAKFIL)





#endif /* CONF_CAN_MCP_H_ */