/*
 * conf_debug.h
 *
 * Created: 8/25/2015 1:20:18 PM
 *  Author: Steve Miller
 */ 

/*
* store defines and macros needed for various dbg output or configurations
*/

#ifndef CONF_DEBUG_H_
#define CONF_DEBUG_H_

#define DBG_ON          1			//master enable debug
#define DBG_TEST        1			//temp, misc
#define DBG_CLKS        DBG_ON  && 1 //clocks
#define DBG_CAN_MSG     DBG_ON  && 1 //can messages
#define DBG_FLASH       DBG_ON  && 1 //flash
#define DBG_HMAC        DBG_ON  && 1 //hmac
#define DBG_RULES       DBG_ON  && 1 //rules
#define DBG_MCP			DBG_ON  && 1 //mcp chipsets
#define DBG_MCP_CAN_RX  DBG_MCP && 0
#define DBG_MCP_CAN_TX  DBG_MCP && 0
#define DBG_INT			DBG_ON	&& 1
#define DBG_MCP_STATE	DBG_ON	&& 1

#define PRINT_NEWLINE()       { if(DBG_ON) {print_dbg("\n\r");} }

#endif /* CONF_DEBUG_H_ */