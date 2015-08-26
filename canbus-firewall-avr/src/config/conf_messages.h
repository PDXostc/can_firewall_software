/*
 * conf_messages.h
 *
 * Created: 8/26/2015 2:07:02 PM
 *  Author: smiller6
 */ 


#ifndef CONF_MESSAGES_H_
#define CONF_MESSAGES_H_

/* CAN Message Object Definitions 
 * 
 *
 */
#if defined (__ICCAVR32__)
can_msg_t msg_tx_sot =
{
    0x133,                    // Identifier
    0x1ff,                    // Mask
    0x0102030405060708LL,     // Data
};
#else
can_msg_t msg_tx_sot =
{
    {
        {
            .id = 0x133,                    // Identifier
            .id_mask  = 0x1ff,              // Mask
        },
    },
    .data.u64 = 0x0102030405060708LL,    // Data
};
#endif

#if defined (__ICCAVR32__)
can_msg_t msg_rx_listening =
{
    0,                // Identifier
    0,                // Mask
    0x0LL,            // Data
};
#else
can_msg_t msg_rx_listening =
{
    {
        {
            .id = 0,                      // Identifier
            .id_mask  = 0,                // Mask
        },
    },
    .data.u64 = 0x0LL,                 // Data
};
#endif



#endif /* CONF_MESSAGES_H_ */