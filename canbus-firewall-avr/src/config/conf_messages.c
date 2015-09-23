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

/*
* Mail Box Definition
*/
// -----------------------------------------------------------------
// CAN Message Definition: Tx Message
#if defined (__ICCAVR32__)
can_msg_t msg_tx_sot = {
    0x00,                    // Identifier
    0x00,                    // Mask
    0x0102030405060708LL,     // Data
};
#else
can_msg_t msg_tx_sot = {
    {
        {
            .id = 0x00,                    // Identifier
            .id_mask  = 0x00,              // Mask
        },
    },
    .data.u64 = 0x0102030405060708LL,    // Data
};
#endif

// MOB Message Definition: Tx Message
can_mob_t north_tx_msg[8] = {
    {
        CAN_MOB_NOT_ALLOCATED,            // Handle: by default CAN_MOB_NOT_ALLOCATED
        &msg_tx_sot,                      // Pointer on CAN Message
        8,                                // Data length DLC
        CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
        CAN_STATUS_NOT_COMPLETED
    },          // Status: by default CAN_STATUS_NOT_COMPLETED
};
can_mob_t south_tx_msg[8] = {
    {
        CAN_MOB_NOT_ALLOCATED,            // Handle: by default CAN_MOB_NOT_ALLOCATED
        &msg_tx_sot,                      // Pointer on CAN Message
        8,                                // Data length DLC
        CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
        CAN_STATUS_NOT_COMPLETED
    },          // Status: by default CAN_STATUS_NOT_COMPLETED
};
// -----------------------------------------------------------------
// CAN Message Definition: Rx Message
#if defined (__ICCAVR32__)
can_msg_t msg_rx_listening = {
    0,                // Identifier
    0,                // Mask
    0x0LL,            // Data
};
can_msg_t msg_rx_listening01 = {
    0x64,                // Identifier
    0xff,                // Mask
    0x0LL,            // Data
};
can_msg_t msg_rx_listening02 = {
    0x3FC,                // Identifier
    0xFF,                // Mask
    0x0LL,            // Data
};
#else
can_msg_t msg_rx_listening = {
    {
        {
            .id = 0x00,                      // Identifier
            .id_mask  = 0x00,                // Mask
        },
    },
    .data.u64 = 0x0LL,                 // Data
};
can_msg_t msg_rx_listening01 = {
    {
        {
            .id = 0x64,                      // Identifier
            .id_mask  = 0xFF,                // Mask
        },
    },
    .data.u64 = 0x0LL,                 // Data
};
can_msg_t msg_rx_listening02 = {
    {
        {
            .id = 0x3FC,                      // Identifier
            .id_mask  = 0xFFF,                // Mask
        },
    },
    .data.u64 = 0x0LL,                 // Data
};
#endif

// MOB Message Definition: Rx Message
can_mob_t north_rx_msg[8] = {
    {
        CAN_MOB_NOT_ALLOCATED,            // Handle: by default CAN_MOB_NOT_ALLOCATED
        &msg_rx_listening,                // Pointer on CAN Message
        8,                                // Data length DLC
        CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
        CAN_STATUS_NOT_COMPLETED
    },          // Status: by default CAN_STATUS_NOT_COMPLETED
};
can_mob_t south_rx_msg01 = {
    {
        CAN_MOB_NOT_ALLOCATED,            // Handle: by default CAN_MOB_NOT_ALLOCATED
        &msg_rx_listening01,                // Pointer on CAN Message
        8,                                // Data length DLC
        CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
        CAN_STATUS_NOT_COMPLETED
    },          // Status: by default CAN_STATUS_NOT_COMPLETED
};
can_mob_t south_rx_msg02 = {
    {
        CAN_MOB_NOT_ALLOCATED,            // Handle: by default CAN_MOB_NOT_ALLOCATED
        &msg_new_rule,                // Pointer on CAN Message
        8,                                // Data length DLC
        CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
        CAN_STATUS_NOT_COMPLETED
    },          // Status: by default CAN_STATUS_NOT_COMPLETED
};

//--------------------------------------------New Rule Msg
// can_msg_t msg_new_rule = {
//     {
//         {
//             .id = 0x7FE,                      // Identifier, should be last number in address space
//             .id_mask  = 0x7FF,                // Mask, should check all bits
//         },
//     },
//     .data.u64 = 0x0LL,                 // Data, should be uninitialized
// };

can_msg_t msg_new_rule = {
    {
        {
            .id = 0x7FE,                      // Identifier, should be last number in address space
            .id_mask  = 0x000,                // Mask, should check all bits
        },
    },
    .data.u64 = 0x0LL,                 // Data, should be uninitialized
};

// -----------------------------------------------------------------
// CAN Message Definition: Rx Remote Message
#if defined (__ICCAVR32__)
can_msg_t msg_remote_rx = {
    0x110,                // Identifier
    0x1ff,                // Mask
    0x55AA00000000AA55    // Data Response
};
#else
can_msg_t msg_remote_rx = {
    {
        {
            .id = 0x110,                      // Identifier
            .id_mask  = 0x1ff,                // Mask
        },
    },
    .data.u64 = 0x55AA00000000AA55,        // Data Response
};
#endif

// MOB Message Definition: Rx Remote Message
can_mob_t appli_remote_rx_msg = {
    CAN_MOB_NOT_ALLOCATED,          // Handle: by default CAN_MOB_NOT_ALLOCATED
    &msg_remote_rx,                 // Pointer on CAN Message
    8,                              // Data length DLC
    CAN_REMOTE_FRAME,               // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
    CAN_STATUS_NOT_COMPLETED
    // Status: by default CAN_STATUS_NOT_COMPLETED
};


void print_can_message(can_msg_t *msg) {
    print_dbg("\n\rCan Message:\n\r");
    print_dbg("\n\ID:  \n\r");
    print_dbg_hex(msg->id);
    print_dbg("\n\:Data:  \n\r");
    print_dbg_ulong(msg->data.u64 >> 32);
    print_dbg_ulong(msg->data.u64);
}

//test messages section ----///////////////////////////////////////

//basic test messages for new rule incoming test
//TODO: get new hmac; hmac generation for this frame set was not valid. hmac fields need to be updated
can_msg_t msg_prep_01 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x04010000FFFF0100LL,    // Data
};

can_msg_t msg_prep_02 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x04020000AAC10807LL,    // Data
};

can_msg_t msg_prep_03 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x0403060504030201LL,    // Data
};

can_msg_t msg_prep_04 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x0404FFFF00003e0cLL,    // Data
};

can_msg_t msg_prep_05 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x040507ab0b451614LL,    // Data
};

can_msg_t msg_prep_06 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x0406d402afec3afdLL,    // Data
};

can_msg_t msg_prep_07 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x0407b55862880ce6LL,    // Data
};

can_msg_t msg_prep_08 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x0408d461b5ee17acLL,    // Data
};

can_msg_t msg_prep_09 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x0409692d2089a6a2LL,    // Data
};

can_msg_t msg_prep_10 = {
    {
        {
            .id = 0x7FE,                    // Identifier
            .id_mask  = 0x7FF,              // Mask
        },
    },
    .data.u64 = 0x040A000000010000LL,    // Data
};
