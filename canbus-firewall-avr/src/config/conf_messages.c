/*
 * conf_messages.c
 *
 * Created: 9/16/2015 11:02:33 AM
 *  Author: smiller6
 */ 


/* Definitions for CAN message structures
*
* 
*
* 
*
*/

#include "conf_messages.h"

can_msg_t msg_new_rule = {
    {
        {
            .id = 0x7FE,                      // Identifier, should be last number in address space
            .id_mask  = 0x000,                // Mask, should check all bits
        },
    },
    .data.u64 = 0x0LL,                 // Data, should be uninitialized
};

void print_can_message(can_msg_t *msg) {
    print_dbg("\n\rCan Message:\n\r");
    print_dbg("\n\rID:  \n\r");
    print_dbg_hex(msg->id);
    print_dbg("\n\r:Data:  \n\r");
    print_dbg_ulong(msg->data.u64 >> 32);
    print_dbg_ulong(msg->data.u64);
}

can_msg_t msg_pass_all = {
    {
        {
            .id = 0x000,                    // Identifier
            .id_mask  = 0x000,              // Mask
        },
    },
    .data.u64 = 0xFFFFFFFFFFFFFFFFLL,    // Data
};
