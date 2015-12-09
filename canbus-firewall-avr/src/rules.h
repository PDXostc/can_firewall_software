/*
* rules.h
*
* Created: 8/31/2015 4:40:34 PM
*  Author: smiller6
*/


#ifndef RULES_H_
#define RULES_H_

#include <string.h>
#include "asf.h"
#include "conf_debug.h"
#include "polarssl/sha2.h"
#include "hmac.h"

/* NOTE on firewall default implementation
 * The firewall should block by default. 
 * Rules define traffic that is allowed to pass through the system.
 * Not providing a ruleset means that the values inialize to zero. The implementation
 * of the standard means that if a ruleset consists of entirely 0, the filtered message
 * will be overwritten to 0. This driver treats zeroed data as data marked for discard
 * and will not retransmit any message that has been zeroed.
 * 
 */

/* Defines for extraction methods */
/*
#define REF_                        0x0807060504030201
*/
#define DATA_PRIO_MASK              0xFF00000000000000                            
#define DATA_PRIO_OFFSET            56

#define DATA_CMD_MASK               0x00FF000000000000
#define DATA_CMD_OFFSET             48

#define DATA_MASK_MASK              0x0000FFFFFFFF0000
#define DATA_MASK_OFFSET            16

#define DATA_XFORM_MASK             0x000000000000FF00
#define DATA_XFORM_OFFSET           8

#define DATA_FILTER_MASK            0x0000FFFFFFFF0000
#define DATA_FILTER_OFFSET          16

#define DATA_DT_OPERAND_01_MASK     0x000000000000FFFF
#define DATA_DT_OPERAND_01_OFFSET   0

#define DATA_DT_OPERAND_02_MASK     0x0000FFFF00000000
#define DATA_DT_OPERAND_02_OFFSET   32

#define DATA_DT_OPERAND_03_MASK     0x00000000FFFF0000
#define DATA_DT_OPERAND_03_OFFSET   16

#define DATA_DT_OPERAND_04_MASK     0x000000000000FFFF
#define DATA_DT_OPERAND_04_OFFSET   0

#define DATA_ID_OPERAND_MASK        0x0000FFFFFFFF0000
#define DATA_ID_OPERAND_OFFSET      16

#define DATA_HMAC_01_MASK           0x0000FFFF00000000
#define DATA_HMAC_01_OFFSET         32

#define DATA_HMAC_02_MASK           0x00000000FFFF0000
#define DATA_HMAC_02_OFFSET         16

#define DATA_HMAC_03_MASK           0x000000000000FFFF
#define DATA_HMAC_03_OFFSET         0

#define DATA_SEQUENCE_MASK         0X0000FFFFFFFF0000
#define DATA_SEQUENCE_OFFSET       16

/* Command enumeration defines, directly corresponds to value of CMD byte in a preparation frame */
#define CMD_PREP_01                         1
#define CMD_PREP_02                         2
#define CMD_PREP_03                         3
#define CMD_PREP_04                         4
#define CMD_PREP_05                         5
#define CMD_PREP_06                         6
#define CMD_PREP_07                         7
#define CMD_PREP_08                         8
#define CMD_PREP_09                         9
#define CMD_STORE                           10


/* Defines for bitfield tracker of received preparation rule frames */
#define BITFIELD_FRAME_RCVD                  1  //set bit true when received corresponding frame
#define BITFIELD_NUM_POSITIONS               9  //number of positions we expect the field to account for
#define BITFIELD_POSITION_PREP_01            0  //following offsets/positions of bits for each prep frame
#define BITFIELD_POSITION_PREP_02            1
#define BITFIELD_POSITION_PREP_03            2
#define BITFIELD_POSITION_PREP_04            3
#define BITFIELD_POSITION_PREP_05            4
#define BITFIELD_POSITION_PREP_06            5
#define BITFIELD_POSITION_PREP_07            6
#define BITFIELD_POSITION_PREP_08            7
#define BITFIELD_POSITION_PREP_09            8
#define BITFIELD_COMPLETE                    0x1FF

/* Xform enumeration
 * The transform field "xform" describes the operations to be applied to an incoming
 * message filtered before it is passed on. We are hard coding using defines
 * because driver understanding of xform must correspond to the external standard.
 * Value corresponds to the half byte value range 0:F
 */
#define XFORM_SET		0x00
#define XFORM_OR		0x01
#define XFORM_AND		0x02
#define XFORM_XOR		0x03
#define XFORM_INV		0x04
#define XFORM_BLOCK		0x05
#define XFORM_PASS		0x06
//should not exceed		0x0F


/* Rules in progress setting */
#define MAX_RULES_IN_PROGRESS                16

#define SIZE_RULESET        16


//Memory structures, proposed

//+ New Rule Working Set
//typedef unsigned char uint8_t;   //(1 byte, 8 bit)
//typedef unsigned short uint16_t; //(2 byte, 16 bit)
//typedef unsigned int uint32_t;     //(4 byte, 32 bit)

//structure for full rule stored in memory
typedef struct {
    uint8_t     prio;       // 1
    uint8_t     xform;      // 1
    uint32_t    mask;       // 4
    uint32_t    filter;     // 4    
    uint32_t    idoperand;  // 4
    uint64_t    dtoperand;  // 8
} rule_t; // 22

//prep rule 01 frame
typedef struct {
    uint32_t mask;// = 0x04030201;
    uint8_t xform;// = 0x01;
    //rsdv 0x01
} rule_prep_01_t;

//prep rule 02 frame
typedef struct {
    uint32_t filter;// = 0x04030201;
    uint16_t dtoperand01;// = 0x0201;
} rule_prep_02_t;

//prep rule 03 frame
typedef struct {
    uint16_t dtoperand02[3];// = {0x0605,0x0403,0x0201};
} rule_prep_03_t;

//prep rule 04 frame
typedef struct {
    uint32_t idoperand;// = 0x04030201;
    uint16_t hmac;// = 0x0201;
} rule_prep_04_t;

//prep rule 05 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_05_t;

//prep rule 06 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_06_t;

//prep rule 07 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_07_t;

//prep rule 08 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_08_t;

//prep rule 09 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_09_t;

typedef struct {
    uint32_t sequence;// = 0x000000; //init to zero so it is never higher than we expect
    //unused 0x0201
} rule_store_t;

//working set memory storage
typedef struct {
    uint8_t prio;
    uint16_t bitfield_completed;
    //location in managed working sets array
    int at_index;
    //structs for rules in progress
    rule_prep_01_t mask_xform;
    rule_prep_02_t filter_dtoperand_01;
    rule_prep_03_t dt_operand_02;
    rule_prep_04_t id_operand_hmac_01;
    rule_prep_05_t hmac_02;
    rule_prep_06_t hmac_03;
    rule_prep_07_t hmac_04;
    rule_prep_08_t hmac_05;
    rule_prep_09_t hmac_06;
    rule_store_t store_sequence;
} rule_working_t;

/* tracking of rules in progress
 * intended to serve as a global index of the rules that are in progress of assembly
 */
typedef struct {
    int num_rules_in_progress;
    rule_working_t *working_sets[MAX_RULES_IN_PROGRESS];    
    }rules_in_progress_t;

extern rules_in_progress_t rules_in_progress;

//north and south rulesetsare stored flash together, with the convention being first half is north, second half is south in nvram
#if defined (__GNUC__)
__attribute__((__section__(".flash_rsvd")))
#endif
extern rule_t flash_can_ruleset[(SIZE_RULESET*2)]
#if defined (__ICAVR32__)
@ "FLASHRSVD"
#endif
;

#if defined (__GNUC__)
__attribute__((__section__(".userpage")))
#endif
static int stored_sequence
#if defined (__ICAVR32__)
@ "USERPAGE"
#endif
;



//extern rule_t flash_can_ruleset[(SIZE_RULESET*2)];
/**
 * \brief Extract prio information from can frame data field
 * Prio indicates the overall priority index intended for the final rule. All preparation frames
 * corresponding to a given rule should have the same prio number.
 * \param data Pointer to CAN frame data field
 * \param prio Pointer to struct entry
 * 
 * \return extern void
 */
extern  void get_frame_prio(const Union64 *data, uint8_t *prio_out);
/**
 * \brief Extract cmd information from can frame data field
 * CMD corresponds to rule preparation number, ie what data we should expect to see in the 
 * rest of the data field. 
 * 
 * \param data Pointer to CAN frame data field
 * \param cmd Pointer to struct entry
 * 
 * \return extern void
 */
extern  void get_frame_cmd(const Union64 *data, uint8_t *cmd_out);
/**
 * \brief Extract mask information from can frame data field
 * 
 * \param data Pointer to CAN frame data field
 * \param mask Pointer to struct entry
 * 
 * \return extern void
 */
extern  void get_frame_mask(const Union64 *data, uint32_t *mask_out);

/**
 * \brief Extract filter information from can frame data field
 * 
 * \param data Pointer to CAN frame data field
 * \param filter Pointer to struct entry
 * 
 * \return extern void
 */
extern  void get_frame_filter(const Union64 *data, uint32_t *filter_out);

/**
 * \brief Extract Transform information from can frame data field
 * 
 * \param data      Pointer to CAN frame data field
 * \param xform    Pointer to struct entry
 * 
 * \return extern void
 */
extern  void get_frame_xform(const Union64 *data, uint8_t *xform_out);

/**
 * \brief Extract Identifier Operand information from can frame data field
 * 
 * \param data          Pointer to CAN frame data field
 * \param idoperand   Pointer to struct entry
 * 
 * \return extern void
 */
extern  void get_frame_id_operand(const Union64 *data, uint32_t *idoperand_out);

/**
 * \brief Extract data operand information from CAN frame data field
 * Function will target bits 0:15 for return
 * Desired data will be packed, user responsibility to provide correct mask
 * 
 * \param data      Pointer to CAN frame data field
 * \param dtoperand_out     Pointer to struct array
 * 
 * \return extern void
 */
extern  void get_frame_dt_operand_01(const Union64 *data, uint16_t *dtoperand_out);

/**
 * \brief Extract data operand information from CAN frame data field
 * Function will target bits 16:31 for return
 * Desired data will be packed, user responsibility to provide correct mask
 * 
 * \param data      Pointer to CAN frame data field
 * \param dtoperand_out     Pointer to struct array
 * 
 * \return extern void
 */
extern  void get_frame_dt_operand_02(const Union64 *data, uint16_t *dtoperand_out);

/**
 * \brief Extract data operand information from CAN frame data field
 * Function will target bits 32:47 for return
 * Desired data will be packed, user responsibility to provide correct mask
 * 
 * \param data      Pointer to CAN frame data field
 * \param dtoperand_out     Pointer to struct array
 * 
 * \return extern void
 */
extern  void get_frame_dt_operand_03(const Union64 *data, uint16_t *dtoperand_out);

/**
 * \brief Extract data operand information from CAN frame data field
 * Function will target bits 48:63 for return
 * Desired data will be packed, user responsibility to provide correct mask
 * 
 * \param data      Pointer to CAN frame data field
 * \param dtoperand_out     Pointer to struct array
 * 
 * \return extern void
 */
extern  void get_frame_dt_operand_04(const Union64 *data, uint16_t *dtoperand_out);

/**
 * \brief Extract hmac information from CAN frame data field
 * Function will target bits 0:15 for return
 * Desired data will be packed, user responsibility to provide correct mask
 * 
 * \param data      Pointer to CAN frame data field
 * \param dtoperand_out     Pointer to struct array
 * 
 * \return extern void
 */
extern  void get_frame_hmac_01(const Union64 *data, uint16_t *hmac_out);

/**
 * \brief Extract hmac information from CAN frame data field
 * Function will target bits 16:31 for return
 * Desired data will be packed, user responsibility to provide correct mask
 * 
 * \param data      Pointer to CAN frame data field
 * \param dtoperand_out     Pointer to struct array
 * 
 * \return extern void
 */
extern  void get_frame_hmac_02(const Union64 *data, uint16_t *hmac_out);

/**
 * \brief Extract hmac information from CAN frame data field
 * Function will target bits 32:47 for return
 * Desired data will be packed, user responsibility to provide correct mask
 * 
 * \param data      Pointer to CAN frame data field
 * \param dtoperand_out     Pointer to struct array
 * 
 * \return extern void
 */
extern void get_frame_hmac_03(const Union64 *data, uint16_t *hmac_out);

/**
 * \brief Extract sequence number from CAN frame data field
 * 
 * \param data Pointer to CAN frame data field
 * \param sequence_out Pointer to struct member
 * 
 * \return extern void
 */
extern void get_frame_sequence(const Union64 *data, uint32_t *sequence_out);

/**
 * \brief Utility for getting unsigned 8 bit val from 64 bit val. 
 * 
 * \param data    Pointer to 64 bit data field
 * \param target Pointer target for output
 * \param mask   64 bit mask, & with data to isolate value
 * \param offset  least significant bit of desired value, used to shift before conversion to output
 * 
 * \return extern void
 */
extern void get_frame_data_u8(const Union64 *data, uint8_t *target, unsigned long long mask, int offset);

/**
 * \brief Utility for getting unsigned 16 bit val from 64 bit val. 
 * 
 * \param data    Pointer to 64 bit data field
 * \param target Pointer target for output
 * \param mask   64 bit mask, & with data to isolate value
 * \param offset  least significant bit of desired value, used to shift before conversion to output
 * 
 * \return extern void
 */
extern void get_frame_data_u16(const Union64 *data, uint16_t *target, unsigned long long mask, int offset);

/**
 * \brief Utility for getting unsigned 32 bit val from 64 bit val. 
 * 
 * \param data    Pointer to 64 bit data field
 * \param target Pointer target for output
 * \param mask   64 bit mask, & with data to isolate value
 * \param offset  least significant bit of desired value, used to shift before conversion to output
 * 
 * \return extern void
 */
extern void get_frame_data_u32(const Union64 *data, uint32_t *target, unsigned long long mask, int offset);

/**
 * \brief Utility for getting unsigned 64 bit val from 64 bit val. 
 * 
 * \param data    Pointer to 64 bit data field
 * \param target Pointer target for output
 * \param mask   64 bit mask, & with data to isolate value
 * \param offset  least significant bit of desired value, used to shift before conversion to output
 * 
 * \return extern void
 */
extern void get_frame_data_u64(const Union64 *data, uint64_t *target, unsigned long long mask, int offset);

/**
 * \brief Set a bit in a given bitfield and position to 1, indicating a rule preparation frame was received successfully 
 * 
 * \param bitfield Pointer to the bitfield to OR with
 * \param position Offset position to set value to
 * 
 * \return extern void
 */
extern void set_bitfield_received(uint16_t *bitfield, int position);

/*! \brief Print a given rule using debug output to usart.
*
* \param *rule Pointer to rule structure.
*/
extern void print_rule(rule_t *rule);

/**
 * \brief Print a complete HMAC string in working set
 * Note: HMAC should be complete before printing
 * 
 * \param working Rule in progress
 * \param split_newlines Should the output be split into new lines for each 16bit chunk?
 * 
 * \return extern void
 */
extern void print_hmac_in_working_set(rule_working_t *working, bool split_newlines);

/* \brief Print a set of rules
*
* \param *ruleset Array of rules
* \param numrules Number of rules to be printed
*/
extern void print_ruleset(rule_t *ruleset, int numrules);

/**
 * \brief Creates a working set in memory for assembling a rule.
 * This version should allocate a pointer to the tracking structure using the private create,
 * and increment the count of working sets in memory.
 * 
 * \param 
 * 
 * \return extern int Index at successful creation
 */
extern int create_working_set_managed(void);
/**
 * \brief Deletes a working set in memory for assembling a rule, presumably used when finished 
 * assembling. This version should check to make sure to decrement the count of rules in progress
 * safely, and delete and rearrange the collection of pointers to the working sets safely as well
 * 
 * \param working Pointer to working set in memory that we wish to delete. Must not be null.
 * 
 * \return extern  bool Successful deletion
 */
extern bool delete_working_set_managed(rule_working_t* working_set[], int at_index);

/**
 * \brief Allocate space for a new working set and return a pointer to the location in memory
 * This is the private call used by the managed version. Public users and main program should use the
 * _managed version of the call.
 * 
 * 
 * \return rule_working_t*
 */
rule_working_t* create_working_set(void);
/**
 * \brief Delete a working rule in progress memory structure given the pointer to the structure.
 *  This is the private call used by the managed version. Public users and main program should use the 
 * _managed version of the call.
 * \param working Pointer to working set in memory
 * 
 * \return void
 */
void delete_working_set(rule_working_t *working);

/*! \brief Creates and returns a rule structure from a rule working set
* \param *working Pointer to a rule in progress structure
* \return  rule_t Created rule structure
*/
extern rule_t create_rule_from_working_set(rule_working_t *working);

/*! \brief Saves a rule to a location in flash memory
* Assumes flash memory is the destination, and that the rules are consistent in structure
* \param source_rule structure to copy from
* \param dest_rule structure to copy to
* \return bool Successful copy
*/
extern bool save_rule_to_flash(rule_t *source_rule, rule_t *dest_rule);

/*! \breif Save ruleset, multiple rules from one location to another, assumes saving to flash memory
 * Simple copy
 * \param source collection of rules to copy from
 * \param dest collection of rules to copy to
 * \param num number of rules to be copied
 */
extern bool save_ruleset_to_flash(rule_t *source, rule_t *dest, int num);

/*! \breif Load a rule from one location to another
 *  Simple copy
 * \param source_rule structure to copy from
 * \param dest_rule structure to copy to
 */
extern void load_rule(rule_t *source_rule, rule_t *dest_rule);

/*! \breif Load ruleset, multiple rules from one location to another
 * Simple copy
 * \param source collection of rules to copy from
 * \param dest collection of rules to copy to
 * \param num number of rules to be copied
 */
extern void load_ruleset(rule_t *source, rule_t *dest, int num);

/**
 * \brief Store new sequence number not greater than number already stored
 * 
 * \param working Pointer to working set
 * 
 * \return void
 */
void store_new_sequence_number(rule_working_t *working);

/**
 * \brief Will verify that the sequence number received is and should be greater than number stored
 * 
 * \param working Pointer to working set where rule in progress resides
 * 
 * \return extern bool Verified successfully
 */
extern bool verify_new_rule_sequence(rule_working_t *working);

/**
 * \brief Will verify the message sender and contents using the hmac received from the preparation frames
 * 
 * \param working Pointer to working set rule in progress
 * 
 * \return extern bool Verified successfully
 */
extern bool verify_new_rule_hmac(rule_working_t *working);

/**
 * \brief Print the entire result of an HMAC operation
 * 
 * \param 
 * 
 * \return void
 */
void print_hmac_operation_result(void);

/**
 * \brief Payload signature is used to generate the HMAC result. Payload signature conforms to a specific convention,
 * not necessarily how we are storing a frame, rule or working set internally
 * 
 * \param working Pointer to working set
 * 
 * \return void
 */
void generate_payload_buffer_from_working_set(rule_working_t *working);

/**
 * \brief Copies received HMAC signature values from a working set into a buffer for validation
 * 
 * \param working Pointer to working set
 * 
 * \return void
 */
void generate_hmac_buffer_from_working_set(rule_working_t *working);
/**
 * \brief Checks that the bitfield for the working set is marked complete for every frame expected to build a rule
 * 
 * \param working Pointer to working set rule in progress
 * 
 * \return extern bool Verified successfully
 */
extern bool verify_new_rule_complete(rule_working_t *working);

/**
 * \brief Handles the data provided by CMD byte. Performs extraction of data from field to working set structure.
 * Also executes the Store Rule case functions for verifying the validity of our rule in progress.
 * 
 * \param data Pointer to data field received
 * \param working_set_index Index of the pointer to the working set applicable for this rule
 * 
 * \return extern bool Case handled. Returns false if no case or if store rule not verified. 
 */
extern bool handle_new_rule_data_cmd(Union64 *data, int working_set_index);

/**
 * \brief Handles the data provided by a positively identified new rule frame. The frame prio byte will be examined,
 * and if a corresponding working set for the prio is already in progress. If a corresponding rule in progress is not 
 * found then a new working set will be created. In either event the data should be passed on to be handled according to
 * CMD byte. In the rare event when a new working set cannot be created, the function should return false, to indicate 
 * that the CAN frame responsible for the data should not yet be released and cleared.
 * 
 * \param data Pointer to data field received
 * 
 * \return extern bool
 */
extern bool handle_new_rule_data(Union64 *data);
#endif /* RULES_H_ */