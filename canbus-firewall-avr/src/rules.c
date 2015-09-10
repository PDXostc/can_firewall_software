/*
* rules.c
*
* Created: 8/31/2015 2:45:32 PM
*  Author: smiller6
*
* Rule evaluation implementation
*/

#include "rules.h"

/* Defines for extraction methods */
#define DATA_PRIO_MASK      0xFF00000000000000
#define DATA_PRIO_OFFSET    55

/* Useful extraction methods for getting what we need out of the CAN frame data field */
uint8_t get_frame_prio(Union64* data) {
    uint8_t prio;
    return prio = ((data->u64 & DATA_PRIO_MASK)>>DATA_PRIO_OFFSET); /* prio mask */
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
    print_dbg_hex(rule->dtoperand);
    
    print_dbg("____END_RULE____");
}

void print_ruleset(rule_t *ruleset, int numrules) {
    if(numrules <= 0) return;
    int i = numrules -1;
    
    while(i >= 0) {
        print_rule(&ruleset[i]);
        i--;
    }
    
}

bool save_rule(rule_t *source_rule, rule_t *dest_rule)
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
    #if 0
    print_dbg("\n\r Rule Saved.\n\r");
    #endif
    success = true;
    return success;
}