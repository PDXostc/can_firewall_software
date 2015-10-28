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
can_msg_t msg_00;
can_msg_t msg_a5;
can_msg_t msg_7ff;

can_mob_t mob_test_00;
can_mob_t mob_test_a5;
can_mob_t mob_test_7ff;
#endif /* CONF_MESSAGES_H_ */