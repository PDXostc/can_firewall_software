/*
 * interrupt_machines.h
 *
 * Created: 11/5/2015 1:44:20 PM
 *  Author: smiller6
 */ 


#ifndef INTERRUPT_MACHINES_H_
#define INTERRUPT_MACHINES_H_

#include <asf.h>
#include "mcp.h"
#include "pdca_interface.h"
#include "mcp_message_que.h"
#include "timestamp.h"

//interrupt levels
#define INT_LEVEL_PROC			AVR32_INTC_INT0
#define INT_LEVEL_MCP_MACHINE	AVR32_INTC_INT1
#define INT_LEVEL_PDCA			AVR32_INTC_INT1
#define INT_LEVEL_MCP_EIC		AVR32_INTC_INT2

//interrupt definitions for external MCP interrupts
#define EXT_INT_PIN_FUNCTION    1
#define EXT_INT_NUM_LINES		2
#define EXT_INT_IVI_LINE		AVR32_EIC_INT1
#define EXT_INT_CAR_LINE		AVR32_EIC_INT6
#define EXT_INT_IVI_IRQ			AVR32_EIC_IRQ_1
#define EXT_INT_CAR_IRQ			AVR32_EIC_IRQ_6

//structure holding external interrupt controller settings
eic_options_t eic_options[EXT_INT_NUM_LINES];

/************************************************************************/
/* JOBS                                                                 */
/************************************************************************/

// usage
// SET_JOB(MCP_status.jobs, JOB_n)
#define SET_MCP_JOB(jobs, set)		(jobs |= set)
#define UNSET_MCP_JOB(jobs, unset)	(jobs &= ~unset)

#define SET_JOB_0		(1 << 0)
#define SET_JOB_1		(1 << 1)
#define SET_JOB_2		(1 << 2)
#define SET_JOB_3		(1 << 3)
#define SET_JOB_4		(1 << 4)
#define SET_JOB_5		(1 << 5)
#define SET_JOB_6		(1 << 6)
#define SET_JOB_7		(1 << 7)
#define SET_JOB_8		(1 << 8)
#define SET_JOB_9		(1 << 9)
#define SET_JOB_10		(1 << 10)
#define SET_JOB_11		(1 << 11)
#define SET_JOB_12		(1 << 12)
#define SET_JOB_13		(1 << 13)
#define SET_JOB_14		(1 << 14)
#define SET_JOB_15		(1 << 15)
#define SET_JOB_16		(1 << 16)
#define SET_JOB_17		(1 << 17)
#define SET_JOB_18		(1 << 18)
#define SET_JOB_19		(1 << 19)
#define SET_JOB_20		(1 << 20)
#define SET_JOB_21		(1 << 21)
#define SET_JOB_22		(1 << 22)
#define SET_JOB_23		(1 << 23)
#define SET_JOB_24		(1 << 24)
#define SET_JOB_25		(1 << 25)
#define SET_JOB_26		(1 << 26)
#define SET_JOB_27		(1 << 27)
#define SET_JOB_28		(1 << 28)
#define SET_JOB_29		(1 << 29)
#define SET_JOB_30		(1 << 30)
#define SET_JOB_31		(1 << 31)

#define JOB_0			(0x00000001)
#define JOB_1			(0x00000002)
#define JOB_2			(0x00000004)
#define JOB_3			(0x00000008)
#define JOB_4			(0x00000010)
#define JOB_5			(0x00000020)
#define JOB_6			(0x00000040)
#define JOB_7			(0x00000080)
#define JOB_8			(0x00000100)
#define JOB_9			(0x00000200)
#define JOB_10			(0x00000400)
#define JOB_11			(0x00000800)
#define JOB_12			(0x00001000)
#define JOB_13			(0x00002000)
#define JOB_14			(0x00004000)
#define JOB_15			(0x00008000)
#define JOB_16			(0x00010000)
#define JOB_17			(0x00020000)
#define JOB_18			(0x00040000)
#define JOB_19			(0x00080000)
#define JOB_20			(0x00100000)
#define JOB_21			(0x00200000)
#define JOB_22			(0x00400000)
#define JOB_23			(0x00800000)
#define JOB_24			(0x01000000)
#define JOB_25			(0x02000000)
#define JOB_26			(0x04000000)
#define JOB_27			(0x08000000)
#define JOB_28			(0x10000000)
#define JOB_29			(0x20000000)
#define JOB_30			(0x40000000)
#define JOB_31			(0x80000000)

// Job masks
// Used to check against pending jobs flags integer
// ie if((JOBS & JOB_n) == JOB_n) goto state for JOB_n
#define JOB_RESET_NORTH			JOB_31
#define JOB_RESET_SOUTH			JOB_30
#define JOB_CONFIGURE_NORTH		JOB_29
#define JOB_CONFIGURE_SOUTH		JOB_28
#define JOB_GET_STATUS_NORTH	JOB_27
#define JOB_GET_STATUS_SOUTH	JOB_26
#define JOB_GET_ERROR_REG_NORTH	JOB_25
#define JOB_GET_ERROR_REG_SOUTH	JOB_24
// insert sleep job here

#define JOB_TX_PENDING			JOB_20

#define JOB_RX_0_NORTH			JOB_15
#define JOB_RX_0_SOUTH			JOB_14
#define JOB_RX_1_NORTH			JOB_13
#define JOB_RX_1_SOUTH			JOB_12

//set wait job

//set no job
#define JOB_NO_JOBS		(0x00000000)

/************************************************************************/
/* MCP STATE MACHINE                                                    */
/************************************************************************/

// Potential States of the MCP handler machine
enum MCP_STATE {
	START,
	JOB_START,
	NO_JOBS,
	RESET_NORTH,
	RESET_SOUTH,
	ENTER_CONFIG_MODE_NORTH,
	ENTER_CONFIG_MODE_SOUTH,
	CONFIGURE_BIT_TIMINGS_NORTH,
	CONFIGURE_BIT_TIMINGS_SOUTH,
	SET_MCP_INTERRUPT_ENABLE_NORTH,
	SET_MCP_INTERRUPT_ENABLE_SOUTH,
	CONFIGURE_RX_0_NORTH,
	CONFIGURE_RX_1_NORTH,
	CONFIGURE_RX_0_SOUTH,
	CONFIGURE_RX_1_SOUTH,
	CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_NORTH,
	CONFIGURE_READY_TO_SEND_PINS_TO_DIGITAL_SOUTH,
	ENTER_NORMAL_MODE_NORTH,
	ENTER_NORMAL_MODE_SOUTH,
	ENTER_NORMAL_MODE_NORTH_CALLBACK,
	ENTER_NORMAL_MODE_SOUTH_CALLBACK,
	GET_STATUS_NORTH,
	GET_STATUS_SOUTH,
	GET_STATUS_NORTH_CALLBACK,
	GET_STATUS_SOUTH_CALLBACK,
	GET_ERROR_REG_NORTH,
	GET_ERROR_REG_SOUTH,
	GET_ERROR_REG_NORTH_CALLBACK,
	GET_ERROR_REG_SOUTH_CALLBACK,
	TX_PENDING,
	EVALUATE_TX_POINTER,
	LOAD_TXB_0_NORTH,
	LOAD_TXB_1_NORTH,
	LOAD_TXB_2_NORTH,
	LOAD_TXB_0_NORTH_CALLBACK,
	LOAD_TXB_1_NORTH_CALLBACK,
	LOAD_TXB_2_NORTH_CALLBACK,
	LOAD_TXB_0_SOUTH,
	LOAD_TXB_1_SOUTH,
	LOAD_TXB_2_SOUTH,
	LOAD_TXB_0_SOUTH_CALLBACK,
	LOAD_TXB_1_SOUTH_CALLBACK,
	LOAD_TXB_2_SOUTH_CALLBACK,
	ADVANCE_TX,
	READ_RX_0_NORTH,
	READ_RX_1_NORTH,
	READ_RX_0_NORTH_CALLBACK,
	READ_RX_1_NORTH_CALLBACK,
	READ_RX_0_SOUTH,
	READ_RX_1_SOUTH,
	READ_RX_0_SOUTH_CALLBACK,
	READ_RX_1_SOUTH_CALLBACK,
};

struct MCP_state_machine_t {
	enum MCP_STATE mcp_state;
	};

volatile struct MCP_state_machine_t mcp_stm;

// MCP status and jobs pending
struct MCP_status_t {
	//store mcp status poll byte
	uint8_t status_byte_north;
	uint8_t status_byte_south;
	//store mcp error flag registers
	uint8_t error_byte_north;
	uint8_t error_byte_south;
	// pending jobs to handle for mcp device, status byte will set the lower 8 bits
	uint32_t jobs;
	// Attention required
	// Bit 0 = North action
	// Bit 1 = South action
	uint8_t attention;
	// might need dedicated BUSY switch
	//BUSY flag
	// set bit 0 = busy North
	// set bit 1 = busy South
	//uint8_t PDCA_busy;
	// bit timing instruction and consecutive registers to be filled with timing information
	uint8_t timings_north[5];
	uint8_t timings_south[5];
	// rx configuration pointers
	struct RX_config *rx_config_north;
	struct RX_config *rx_config_south;
};

volatile struct MCP_status_t mcp_status;

// func wrappers for getting and setting the state of the MCP machine
inline static void mcp_stm_set_state(volatile struct MCP_state_machine_t *stm, enum MCP_STATE state)
{
	stm->mcp_state = state;
}

inline static enum MCP_STATE mcp_stm_get_state(volatile struct MCP_state_machine_t *stm)
{
	return stm->mcp_state;
};


// func wrappers for setting jobs
inline static void mcp_stm_set_job(volatile struct MCP_status_t *status, volatile uint32_t jobs)
{
	SET_MCP_JOB(status->jobs, jobs);
}

inline static void mcp_stm_unset_job(volatile struct MCP_status_t *status, volatile uint32_t jobs)
{
	UNSET_MCP_JOB(status->jobs, jobs);
}

#define MCP_SET_ATTN(attn, set)		(attn |= set)
#define MCP_UNSET_ATTN(attn, set)	(attn &= ~set)

uint8_t init_eic_options(void);

uint8_t init_mcp_status(void);

extern void init_interrupt_machines(void);

void run_mcp_state_machine(void);

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void mcp_interrupt_handler_north(void);

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void mcp_interrupt_handler_south(void);

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void proc_int_handler(void);

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void mcp_machine_int_handler(void);

static inline void mcp_machine_int_set(void)
{
	// gpio_set_pin_low(MCP_MACHINE_INT_PIN);
	gpio_local_clr_gpio_pin(MCP_MACHINE_INT_PIN);
}

static inline void mcp_machine_int_clear(void)
{
	// gpio_set_pin_high(MCP_MACHINE_INT_PIN);
	gpio_local_set_gpio_pin(MCP_MACHINE_INT_PIN);
	gpio_clear_pin_interrupt_flag(MCP_MACHINE_INT_PIN);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void pdca_rx_transfer_complete_int_handler(void);

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
extern void pdca_tx_transfer_complete_int_handler(void);

extern void PDCA_set_job_rx(const struct spi_device *device, volatile pdca_channel_options_t *options_tx, volatile pdca_channel_options_t *options_rx, uint8_t pdca_busy_flag);
extern void PDCA_set_job_tx(const struct spi_device *device, volatile pdca_channel_options_t *options_tx, uint8_t pdca_busy_flag);

#endif /* INTERRUPT_MACHINES_H_ */