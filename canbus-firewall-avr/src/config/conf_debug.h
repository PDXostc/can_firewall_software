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
#define DBG_CLKS        DBG_ON && 1 //clocks
#define DBG_CAN_MSG     DBG_ON && 1 //can messages
#define DBG_FLASH       DBG_ON && 1 //flash
#define DBG_HMAC        DBG_ON && 1 //hmac
#define DBG_RULES       DBG_ON && 1 //rules

#define PRINT_NEWLINE       { if(DBG_ON) {print_dbg("\n\r");} }

#endif /* CONF_DEBUG_H_ */