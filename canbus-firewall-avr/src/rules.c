/*
 * rules.c
 *
 * Created: 8/31/2015 2:45:32 PM
 *  Author: smiller6
 *
 * Rule evaluation implementation
 */ 

#include "rules.h"

/* Defines for extraction methods */
#define DATA_PRIO_MASK      0xFF00000000000000
#define DATA_PRIO_OFFSET    55

/* Useful extraction methods for getting what we need out of the CAN frame data field */
uint8_t get_frame_prio(Union64* data) {
    uint8_t prio;
    return prio = ((data->u64 & DATA_PRIO_MASK)>>DATA_PRIO_OFFSET); /* prio mask */
}