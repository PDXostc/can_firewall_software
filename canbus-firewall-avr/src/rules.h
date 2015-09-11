/*
* rules.h
*
* Created: 8/31/2015 4:40:34 PM
*  Author: smiller6
*/


#ifndef RULES_H_
#define RULES_H_

#include "asf.h"


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
    uint16_t hmac[1];// = 0x0201;
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

//prep rule 10 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
    //unused 0x0201
} rule_prep_10_t;

typedef struct {
    uint32_t sequence;// = 0x000000; //init to zero so it is never higher than we expect
    //unused 0x0201
} rule_store_t;

//working set memory storage
typedef struct {
    uint8_t prio;
    uint16_t bitfield_completed;
    //structs for rules in progress
    rule_prep_01_t prep_01;
    rule_prep_02_t prep_02;
    rule_prep_03_t prep_03;
    rule_prep_04_t prep_04;
    rule_prep_05_t prep_05;
    rule_prep_06_t prep_06;
    rule_prep_07_t prep_07;
    rule_prep_08_t prep_08;
    rule_prep_09_t prep_09;
    rule_prep_10_t prep_10;
    rule_store_t store;
} rule_working_t;




uint8_t get_frame_prio(Union64* data);

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