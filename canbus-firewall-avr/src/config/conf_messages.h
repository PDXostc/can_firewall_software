/*
* conf_messages.h
*
* Created: 8/26/2015 2:07:02 PM
*  Author: smiller6
*/


#ifndef CONF_MESSAGES_H_
#define CONF_MESSAGES_H_

#include <asf.h>

//--------------------------------------------New Rule Msg
can_msg_t msg_new_rule;

// -----------------------------------------------------------------


extern void print_can_message(can_msg_t *msg);

//test messages section ----///////////////////////////////////////

//test passing message
can_msg_t msg_pass_all;
#endif /* CONF_MESSAGES_H_ */