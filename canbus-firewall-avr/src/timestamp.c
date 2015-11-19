/*
 * timestamp.c
 *
 * Created: 11/18/2015 4:41:20 PM
 *  Author: smiller6
 */ 

#include "timestamp.h"

volatile struct timestamp_t timestamps[NUM_TIMESTAMPS] = {0};

volatile struct timestamp_t *timestamp_ptr_current = &timestamps[0];

volatile uint32_t timestamp_count = 0;