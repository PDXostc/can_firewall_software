/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * conf_messages.c
 *
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

can_msg_t msg_00 = {
    {
        {
            .id = 0x000,                    // Identifier
            .id_mask  = 0x000,              // Mask
        },
    },
    .data.u64 = 0x0000000000000000LL,    // Data
};

can_msg_t msg_a5 = {
    {
        {
            .id = 0x0A5,                    // Identifier
            .id_mask  = 0x000,              // Mask
        },
    },
    .data.u64 = 0xA5A5A5A5A5A5A5A5LL,    // Data
};

can_msg_t msg_7ff = {
    {
        {
            .id = 0x7ff,                    // Identifier
            .id_mask  = 0x000,              // Mask
        },
    },
    .data.u64 = 0xFFFFFFFFFFFFFFFFLL,    // Data
};

can_mob_t mob_test_00 = {
	.can_msg = &msg_00,
	.dlc = 8
};

can_mob_t mob_test_a5 = {
	.can_msg = &msg_a5,
	.dlc = 8
};

can_mob_t mob_test_7ff = {
	.can_msg = &msg_7ff,
	.dlc = 8
};
