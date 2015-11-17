/*
 * mcp_message_que.h
 *
 * Created: 11/12/2015 11:45:46 AM
 *  Author: smiller6
 */ 


#ifndef MCP_MESSAGE_QUE_H_
#define MCP_MESSAGE_QUE_H_

#include <asf.h>
#include "mcp.h"

// Direction, used for determining the target direction in a number of contexts
// Used by other modules such as the PDCA and interrupt notifications for flagging
// which direction needs attention or is being communicated with
#define MCP_DIR_NORTH	0x01
#define MCP_DIR_SOUTH	0x02

#define MCP_QUE_SIZE		32

struct MCP_message_t {
	// message direction, Northbound or Southbound
	// This directionality means that messages received from south will be marked
	// direction = north, and messages received from north will be marked 
	// direction = south.
	uint8_t direction;
	// storage of actual message. 13 bytes is a standard size for CAN message storage.
	// in this case, the message is expected to be stored according to the MCP register
	// format
	uint8_t msg[MCP_CAN_MSG_SIZE];
	};

// Allocate a message que to live in HSB RAM
// Please note that the location specified as the attribute proceeding the declaration
// is specificly called out in a custom linker description. This file is included
// in the project. Memory locations in such a script are highly platform and hardware
// specific.
#if defined (__GNUC__) && defined (__AVR32__)
extern volatile struct MCP_message_t mcp_message_que[MCP_QUE_SIZE] __attribute__ ((__section__(".hsb_ram_loc")));
#endif

volatile struct MCP_message_t *que_ptr_rx;
volatile struct MCP_message_t *que_ptr_proc;
volatile struct MCP_message_t *que_ptr_tx;

// Transmit pointer status
struct TX_status_t {
	// trasmit pointer evaluation counter
	uint32_t tx_pending_count;
};

struct TX_status_t TX_status;

// Process pointer status
struct PROC_status_t {
	uint32_t proc_pending_count;
};

struct PROC_status_t PROC_status;

//ptr que function calls
static inline void set_que_ptr_direction(volatile struct MCP_message_t *ptr, uint8_t direction)
{
	ptr->direction = direction;
}

static inline uint8_t get_que_ptr_direction(volatile struct MCP_message_t *ptr)
{
	return ptr->direction;
}

//advance pointer, given pointer address. checks for bounds of que and resets position if necessary
extern void que_advance_ptr(volatile struct MCP_message_t **ptr);

extern pdca_channel_options_t PDCA_options_mcp_spi_msg_rx;
extern pdca_channel_options_t PDCA_options_mcp_spi_msg_tx;

// init ptrs func
// TODO

extern void init_message_que(void);

//testing messages in que

#endif /* MESSAGE_QUE_H_ */