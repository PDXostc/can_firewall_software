/*
* rules.c
*
* Created: 8/31/2015 2:45:32 PM
*  Author: smiller6
*
* Rule evaluation implementation
*/

#include "rules.h"



/* Useful extraction methods for getting what we need out of the CAN frame data field */
 inline void get_frame_prio(const Union64 *data, uint8_t *prio) {
    *prio = ((data->u64 & DATA_PRIO_MASK)>>DATA_PRIO_OFFSET); /* prio mask */
}

inline void get_frame_cmd(const Union64 *data, uint8_t *cmd)
{
    *cmd = ((data->u64 & DATA_CMD_MASK)>>DATA_CMD_OFFSET);
}

inline void get_frame_mask(const Union64 *data, uint32_t *mask)
{
    *mask = ((data->u64 & DATA_MASK_MASK)>>DATA_MASK_OFFSET);
}

inline void get_frame_filter(const Union64 *data, uint32_t *filter)
{
    *filter = ((data->u64 & DATA_FILTER_MASK)>>DATA_FILTER_OFFSET);
}

inline void get_frame_xform(const Union64 *data, uint8_t *xform)
{
    *xform = ((data->u64 & DATA_XFORM_MASK)>>DATA_XFORM_OFFSET);
}

void print_rule(rule_t *rule) {
    print_dbg("\n\n\r____RULE____\n\r");
    
    print_dbg("\n\r Priority:  \n\r");
    print_dbg_hex(rule->prio);
    print_dbg("\n\r Mask:  \n\r");
    print_dbg_hex(rule->mask);
    print_dbg("\n\r Filter:  \n\r");
    print_dbg_hex(rule->filter);
    print_dbg("\n\r Xform:  \n\r");
    print_dbg_hex(rule->xform);
    print_dbg("\n\r ID Operand:  \n\r");
    print_dbg_hex(rule->idoperand);
    print_dbg("\n\r Data Operand:  \n\r");
    print_dbg_hex(rule->dtoperand >> 32);
    print_dbg_hex(rule->dtoperand);
    
    print_dbg("____END_RULE____");
}

void print_ruleset(rule_t *ruleset, int numrules) {
    if(numrules <= 0) return;
    
    for(int i = 0; i < numrules; i++) {
        print_rule(&ruleset[i]);
    }
    
}

bool save_rule_to_flash(rule_t *source_rule, rule_t *dest_rule)
{
    #if DBG_FLASH
    print_dbg("\n\r Saving rule to flash...\n\r");
    #endif

    bool success = false;
    
    flashc_memcpy((void *)&dest_rule->prio,      &source_rule->prio,      sizeof(source_rule->prio),      true);
    flashc_memcpy((void *)&dest_rule->mask,      &source_rule->mask,      sizeof(source_rule->mask),      true);
    flashc_memcpy((void *)&dest_rule->filter,    &source_rule->filter,    sizeof(source_rule->filter),    true);
    flashc_memcpy((void *)&dest_rule->xform,     &source_rule->xform,     sizeof(source_rule->xform),     true);
    flashc_memcpy((void *)&dest_rule->idoperand, &source_rule->idoperand, sizeof(source_rule->idoperand), true);
    flashc_memcpy((void *)&dest_rule->dtoperand, &source_rule->dtoperand, sizeof(source_rule->dtoperand), true);
    
    //methods should have returned an assert if unsuccessful
    #if DBG_FLASH
    print_dbg("\n\r Rule Saved.\n\r");
    #endif
    success = true;
    return success;
}

bool save_ruleset_to_flash(rule_t *source, rule_t *dest, int num)
{
    if((num < 0) || (num == 0)) return false;
    bool success = false;        
    for(int i = 0; i < num; i++ ) {
        success = save_rule_to_flash(&source[i], &dest[i]);
    }
    return success;
}

rule_t create_rule_from_working_set(rule_working_t *working) {
    rule_t new_rule;
    
    new_rule.prio =         working->prio;
    new_rule.mask =         working->prep_01.mask;
    new_rule.filter =       working->prep_02.filter;
    new_rule.xform =        working->prep_01.xform;
    new_rule.idoperand =    working->prep_04.idoperand;

    //There's probably a much more efficient way to accomplish loading the uint16 parcels into the uint64    

    new_rule.dtoperand = working->prep_03.dtoperand02[0];
    new_rule.dtoperand = new_rule.dtoperand << 16;
    new_rule.dtoperand |= working->prep_03.dtoperand02[1];
    new_rule.dtoperand = new_rule.dtoperand << 16;
    new_rule.dtoperand |= working->prep_03.dtoperand02[2];
    new_rule.dtoperand = new_rule.dtoperand << 16;
    new_rule.dtoperand |= working->prep_02.dtoperand01;
    
    return new_rule;
}

inline void load_rule(rule_t *source_rule, rule_t *dest_rule)
{
    *dest_rule = *source_rule;
}

void load_ruleset(rule_t *source, rule_t *dest, int num)
{
    if((num < 0) || (num == 0)) return;
    for(int i = 0; i < num; i++) {
        load_rule(&source[i], &dest[i]);
    }
}