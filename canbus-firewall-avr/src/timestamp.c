/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * timestamp.c
 *
 */ 

#include "timestamp.h"

volatile struct timestamp_t timestamps[NUM_TIMESTAMPS] = {0};

volatile struct timestamp_t *timestamp_ptr_current = &timestamps[0];

volatile uint32_t timestamp_count = 0;