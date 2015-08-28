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

#define DBG_ON          1
#define DBG_TEST        1
#define DBG_CLKS        DBG_ON && 1
#define DBG_CAN_MSG     DBG_ON && 1

#define PRINT_DBG(x)        { if(DBG_ON) {print_dbg(const char *x);} }
#define PRINT_DBG_TEST      { if(DBG_TEST) {print_dbg("Test\n");} }

#endif /* CONF_DEBUG_H_ */