/*
* rules.h
*
* Created: 8/31/2015 4:40:34 PM
*  Author: smiller6
*/


#ifndef RULES_H_
#define RULES_H_

#include "asf.h"


/* Defines for extraction methods */
#define REF_                        0x0807060504030201
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

/* Rules in progress setting */
#define MAX_RULES_IN_PROGRESS                16

//Memory structures, proposed

//+ New Rule Working Set
//typedef unsigned char uint8_t;   //(1 byte, 8 bit)
//typedef unsigned short uint16_t; //(2 byte, 16 bit)
//typedef unsigned int uint32_t;     //(4 byte, 32 bit)

//structure for full rule stored in memory
typedef struct {
    uint8_t     prio;
    uint32_t    mask;
    uint32_t    filter;
    uint8_t     xform;
    uint32_t    idoperand;
    uint64_t    dtoperand;
} rule_t;

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

//expose working rules in progress structure
extern rules_in_progress_t rules_in_progress;

/**
 * \brief Extract prio information from can frame data field
 * Prio indicates the overall priority index intended for the final rule. All preparation frames
 * corresponding to a given rule should have the same prio number.
 * \param data Pointer to CAN frame data field
 * \param prio Pointer to struct entry
 * 
 * \return extern void
 */
extern inline void get_frame_prio(const Union64 *data, uint8_t *prio_out);
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
extern inline void get_frame_cmd(const Union64 *data, uint8_t *cmd_out);
/**
 * \brief Extract mask information from can frame data field
 * 
 * \param data Pointer to CAN frame data field
 * \param mask Pointer to struct entry
 * 
 * \return extern void
 */
extern inline void get_frame_mask(const Union64 *data, uint32_t *mask_out);

/**
 * \brief Extract filter information from can frame data field
 * 
 * \param data Pointer to CAN frame data field
 * \param filter Pointer to struct entry
 * 
 * \return extern void
 */
extern inline void get_frame_filter(const Union64 *data, uint32_t *filter_out);

/**
 * \brief Extract Transform information from can frame data field
 * 
 * \param data      Pointer to CAN frame data field
 * \param xform    Pointer to struct entry
 * 
 * \return extern void
 */
extern inline void get_frame_xform(const Union64 *data, uint8_t *xform_out);

/**
 * \brief Extract Identifier Operand information from can frame data field
 * 
 * \param data          Pointer to CAN frame data field
 * \param idoperand   Pointer to struct entry
 * 
 * \return extern void
 */
extern inline void get_frame_id_operand(const Union64 *data, uint32_t *idoperand_out);

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
extern inline void get_frame_dt_operand_01(const Union64 *data, uint16_t *dtoperand_out);

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
extern inline void get_frame_dt_operand_02(const Union64 *data, uint16_t *dtoperand_out);

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
extern inline void get_frame_dt_operand_03(const Union64 *data, uint16_t *dtoperand_out);

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
extern inline void get_frame_dt_operand_04(const Union64 *data, uint16_t *dtoperand_out);

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
extern inline void get_frame_hmac_01(const Union64 *data, uint16_t *hmac_out);

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
extern inline void get_frame_hmac_02(const Union64 *data, uint16_t *hmac_out);

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
extern inline void get_frame_hmac_03(const Union64 *data, uint16_t *hmac_out);

extern inline void get_frame_data_u8(const Union64 *data, uint8_t *target, unsigned long long mask, int offset);
extern inline void get_frame_data_u16(const Union64 *data, uint16_t *target, unsigned long long mask, int offset);
extern inline void get_frame_data_u32(const Union64 *data, uint32_t *target, unsigned long long mask, int offset);
extern inline void get_frame_data_u64(const Union64 *data, uint64_t *target, unsigned long long mask, int offset);

/**
 * \brief Set a bit in a given bitfield and position to 1, indicating a rule preparation frame was received successfully 
 * 
 * \param bitfield Pointer to the bitfield to OR with
 * \param position Offset position to set value to
 * 
 * \return extern void
 */
extern inline void set_bitfield_received(uint16_t *bitfield, int position);

/*! \brief Print a given rule using debug output to usart.
*
* \param *rule Pointer to rule structure.
*/
extern void print_rule(rule_t *rule);

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
extern inline void load_rule(rule_t *source_rule, rule_t *dest_rule);

/*! \breif Load ruleset, multiple rules from one location to another
 * Simple copy
 * \param source collection of rules to copy from
 * \param dest collection of rules to copy to
 * \param num number of rules to be copied
 */
extern void load_ruleset(rule_t *source, rule_t *dest, int num);
#endif /* RULES_H_ */