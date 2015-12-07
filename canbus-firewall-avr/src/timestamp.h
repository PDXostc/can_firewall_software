/*
 * timestamp.h
 *
 * Created: 11/18/2015 4:17:27 PM
 *  Author: smiller6
 */ 


#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <asf.h>

#include <string.h>

#define TIMESTAMP_NAME_LENGTH	16
#define NUM_TIMESTAMPS			256

struct timestamp_t {
	char name[TIMESTAMP_NAME_LENGTH];
	uint64_t stamp;
	uint32_t since_last;
	};

volatile struct timestamp_t timestamps[NUM_TIMESTAMPS];

volatile struct timestamp_t *timestamp_ptr_current;
volatile uint32_t timestamp_count;

static inline void set_timestamp(const char* name, volatile uint64_t stamp)
{
	AVR32_ENTER_CRITICAL_REGION()
	strncpy(timestamp_ptr_current->name, name, strlen(name));
	timestamp_ptr_current->stamp = stamp;
	// next, unsafe...
	if (timestamp_ptr_current == &timestamps[NUM_TIMESTAMPS - 1])
	{
		timestamp_ptr_current = &timestamps[0];
		
	} else {		
		timestamp_ptr_current = timestamp_ptr_current + 1;
	}
	timestamp_count++;
	AVR32_LEAVE_CRITICAL_REGION()
}

static void calc_timestamps_since_last(int count)
{
	uint32_t last = 0;
	for (int i = 0; i < count; i++)
	{
		timestamps[i].since_last = timestamps[i].stamp - last;
		last = timestamps[i].stamp;
	}
}

#endif /* TIMESTAMP_H_ */