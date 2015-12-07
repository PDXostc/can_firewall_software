/*
 * mcp_message_que.h
 *
 * Created: 11/12/2015 11:45:46 AM
 *  Author: smiller6
 */ 


#ifndef MCP_MESSAGE_QUE_H_
#define MCP_MESSAGE_QUE_H_


#include <asf.h>
#include <string.h>
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

/* Linear message pipeline pointers
 * Messages received from MCP will be copied to location pointed to by que_ptr_rx
 * Messages are processed at location pointed to by que_ptr_proc
 * Messages are evaluated for transmission at location pointed to by que_ptr_tx
 */
volatile struct MCP_message_t *que_ptr_rx;
volatile struct MCP_message_t *que_ptr_proc;
volatile struct MCP_message_t *que_ptr_tx;

// Transmit pointer status
struct TX_status_t {
	// trasmit pointer evaluation counter
	// used to track number of messages that have been marked as pending for
	// transmission evaluation
	uint32_t tx_pending_count;
};

volatile struct TX_status_t TX_status;

// Process pointer status
struct PROC_status_t {
	// processing pointer evaluation counter
	// used to track number of messages that have been marked as pending for
	// processing
	uint32_t proc_pending_count;
};

volatile struct PROC_status_t PROC_status;

//ptr que function calls


/**
 * \brief Sets the "direction" of the message. This should be done when the message
 * is received. The direction denotes "northbound" (received from south, transmitted
 * north) or "southbound" (received from north, transmitted south)
 * 
 * \param ptr Pointer to message struct
 * \param direction Predefined value signifying northbound or southbound direction
 * 
 * \return void
 */
static inline void set_que_ptr_direction(volatile struct MCP_message_t *ptr, uint8_t direction)
{
	ptr->direction = direction;
}


/**
 * \brief Gets the "direction" of the message. The direction denotes "northbound" 
 * (received from south, transmitted north) or "southbound" (received from north, 
 * transmitted south)
 * 
 * \param ptr Pointer to message struct
 * 
 * \return uint8_t Predefined value signifying northbound or southbound direction
 */
static inline uint8_t get_que_ptr_direction(volatile struct MCP_message_t *ptr)
{
	return ptr->direction;
}

/**
 * \brief Advance pointer, given pointer address. Checks for bounds of que and 
 * resets position if necessary.
 * 
 * \param ptr Pointer to pointer to be modified
 * 
 * \return extern void
 */
extern void que_advance_ptr(volatile struct MCP_message_t **ptr);

// struct containing necessary options for initilializing the Atmel Peripheral
// Direct Access Memory Controller (PDCA). 
extern volatile pdca_channel_options_t PDCA_options_mcp_spi_msg_rx;
extern volatile pdca_channel_options_t PDCA_options_mcp_spi_msg_tx;

// init ptrs func
// 
/**
 * \brief Initializes locations of message pipeline pointers to the beginning of
 * the que. Hardcoded to use the module's MCP format message que.
 * 
 * \param 
 * 
 * \return extern void
 */
extern void init_mcp_message_que(void);

/**
 * \brief Erase an MCP message block corresponding to the size of an MCP message
 * struct. Most often used during processing when blocking/discarding a message.
 * 
 * \param proc Pointer to message struct
 * 
 * \return void
 */
extern void wipe_msg_id(volatile struct MCP_message_t **proc);


//testing messages in que

#endif /* MESSAGE_QUE_H_ */