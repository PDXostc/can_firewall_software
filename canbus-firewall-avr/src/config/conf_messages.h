/*
* conf_messages.h
*
* Created: 8/26/2015 2:07:02 PM
*  Author: smiller6
*/

/* Definitions for CAN message structures
*
* appli_[rx/tx]_msg overwrites the mob structure in driver with our default values for mob
*
* msg_tx_sot defines a CAN message in transmission
*
*/

#ifndef CONF_MESSAGES_H_
#define CONF_MESSAGES_H_

#include <asf.h>

/*
* Mail Box Definition
*/
// -----------------------------------------------------------------
// CAN Message Definition: Tx Message

can_msg_t msg_tx_sot;


// MOB Message Definition: Tx Message
can_mob_t north_tx_msg[8];
can_mob_t south_tx_msg[8];
// -----------------------------------------------------------------
// CAN Message Definition: Rx Message

can_msg_t msg_rx_listening;
can_msg_t msg_rx_listening01;
can_msg_t msg_rx_listening02;

// MOB Message Definition: Rx Message
can_mob_t north_rx_msg[8];
can_mob_t south_rx_msg01;
can_mob_t south_rx_msg02;

//--------------------------------------------New Rule Msg
can_msg_t new_rule_msg;

// -----------------------------------------------------------------

can_msg_t msg_remote_rx;


// MOB Message Definition: Rx Remote Message
can_mob_t appli_remote_rx_msg;


extern void print_can_message(can_msg_t *msg);

//test messages section ----///////////////////////////////////////

//basic test messages for new rule incoming test
can_msg_t msg_prep_01;

can_msg_t msg_prep_02;

can_msg_t msg_prep_03;

can_msg_t msg_prep_04;

can_msg_t msg_prep_05;

can_msg_t msg_prep_06;

can_msg_t msg_prep_07;

can_msg_t msg_prep_08;

can_msg_t msg_prep_09;

can_msg_t msg_prep_10;
#endif /* CONF_MESSAGES_H_ */