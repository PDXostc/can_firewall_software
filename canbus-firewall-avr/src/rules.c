/*
* rules.c
*
* Created: 8/31/2015 2:45:32 PM
*  Author: smiller6
*
* Rule evaluation implementation
*/

#include "rules.h"

//should always initialize this one for tracking
rules_in_progress_t rules_in_progress = {
    .num_rules_in_progress = 0,
    .working_sets = {NULL}
};

rule_t flash_can_ruleset[(SIZE_RULESET*2)];

/* Useful extraction methods for getting what we need out of the CAN frame data field */
 inline void get_frame_prio(const Union64 *data, uint8_t *prio) {
    get_frame_data_u8(data, prio, DATA_PRIO_MASK, DATA_PRIO_OFFSET);
}

inline void get_frame_cmd(const Union64 *data, uint8_t *cmd)
{
    get_frame_data_u8(data, cmd, DATA_CMD_MASK, DATA_CMD_OFFSET);
}

inline void get_frame_mask(const Union64 *data, uint32_t *mask)
{
    get_frame_data_u32(data, mask, DATA_MASK_MASK, DATA_MASK_OFFSET);
}

inline void get_frame_filter(const Union64 *data, uint32_t *filter)
{
    get_frame_data_u32(data, filter, DATA_FILTER_MASK, DATA_FILTER_OFFSET);
}

inline void get_frame_xform(const Union64 *data, uint8_t *xform)
{
    get_frame_data_u8(data, xform, DATA_XFORM_MASK, DATA_XFORM_OFFSET);
}

inline void get_frame_id_operand(const Union64 *data, uint32_t *idoperand)
{
    get_frame_data_u32(data, idoperand, DATA_ID_OPERAND_MASK, DATA_ID_OPERAND_OFFSET);
}

inline void get_frame_dt_operand_01(const Union64 *data, uint16_t *dtoperand_out)
{
    get_frame_data_u16(data, dtoperand_out, DATA_DT_OPERAND_01_MASK, DATA_DT_OPERAND_01_OFFSET);
}

inline void get_frame_dt_operand_02(const Union64 *data, uint16_t *dtoperand_out)
{
    get_frame_data_u16(data, dtoperand_out, DATA_DT_OPERAND_02_MASK, DATA_DT_OPERAND_02_OFFSET);
}

inline void get_frame_dt_operand_03(const Union64 *data, uint16_t *dtoperand_out)
{
    get_frame_data_u16(data, dtoperand_out, DATA_DT_OPERAND_03_MASK, DATA_DT_OPERAND_03_OFFSET);
}

inline void get_frame_dt_operand_04(const Union64 *data, uint16_t *dtoperand_out)
{
    get_frame_data_u16(data, dtoperand_out, DATA_DT_OPERAND_04_MASK, DATA_DT_OPERAND_04_OFFSET);
}

inline void get_frame_hmac_01(const Union64 *data, uint16_t *dtoperand_out)
{
    get_frame_data_u16(data, dtoperand_out, DATA_HMAC_01_MASK, DATA_HMAC_01_OFFSET);
}

inline void get_frame_hmac_02(const Union64 *data, uint16_t *dtoperand_out)
{
    get_frame_data_u16(data, dtoperand_out, DATA_HMAC_02_MASK, DATA_HMAC_02_OFFSET);
}

inline void get_frame_hmac_03(const Union64 *data, uint16_t *dtoperand_out)
{
    get_frame_data_u16(data, dtoperand_out, DATA_HMAC_03_MASK, DATA_HMAC_03_OFFSET);
}

inline void get_frame_data_u8(const Union64 *data, uint8_t *target, unsigned long long mask, int offset)
{
    Assert(sizeof(*target) == sizeof(uint8_t));
    *target = (uint8_t)((data->u64 & mask)>>offset);
}
inline void get_frame_data_u16(const Union64 *data, uint16_t *target, unsigned long long mask, int offset)
{
    Assert(sizeof(*target) == sizeof(uint16_t));
    *target = (uint16_t)((data->u64 & mask)>>offset);
}
inline void get_frame_data_u32(const Union64 *data, uint32_t *target, unsigned long long mask, int offset)
{
    Assert(sizeof(*target) == sizeof(uint32_t));
    *target = (uint32_t)((data->u64 & mask)>>offset);
}
inline void get_frame_data_u64(const Union64 *data, uint64_t *target, unsigned long long mask, int offset)
{
    Assert(sizeof(*target) == sizeof(uint64_t));
    *target = (uint64_t)((data->u64 & mask)>>offset);
}

inline void set_bitfield_received(uint16_t *bitfield, int position)
{
    *bitfield |= (uint16_t)(BITFIELD_FRAME_RCVD << position);
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

void print_hmac_in_working_set(rule_working_t *working, bool split_newlines)
{
    print_dbg("\n\r HMAC: \n\r");
    print_dbg_short_hex(working->id_operand_hmac_01.hmac);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_02.hmac[0]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_02.hmac[1]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_02.hmac[2]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_03.hmac[0]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_03.hmac[1]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_03.hmac[2]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_04.hmac[0]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_04.hmac[1]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_04.hmac[2]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_05.hmac[0]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_05.hmac[1]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_05.hmac[2]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_06.hmac[0]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_06.hmac[1]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg_short_hex(working->hmac_06.hmac[2]);
    if(split_newlines) print_dbg("\n\r");
    print_dbg("\n\rEND_HMAC+++\n\r");
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

int create_working_set_managed()
{
    //check within bounds
    if (rules_in_progress.num_rules_in_progress >= MAX_RULES_IN_PROGRESS) return -1;
    
    //assume num rules in progress is a running count, giving us next available index
    int i = rules_in_progress.num_rules_in_progress;
    
    //allocate new to pointer collection
    rules_in_progress.working_sets[i] = create_working_set();
    
    //check that allocation was successful
    if(rules_in_progress.working_sets[i] != NULL) {
        //tell working set where it is 
        rules_in_progress.working_sets[i]->at_index = i;
        //increment count
        rules_in_progress.num_rules_in_progress += 1;
        return i;    
    } else return -1;
    
}

bool delete_working_set_managed(rule_working_t* working_set[], int at_index)
{
    //do nothing if handed a null pointer
    if(working_set[at_index] == NULL) return false;
    //if count of rules in progress is not greater than 0, this must be a mistake
    if((rules_in_progress.num_rules_in_progress > 0) == false) return false;
    
    //free working set in memory
    delete_working_set(working_set[at_index]);
    
    //shift pointers in collection, decrementing their position 
    for(int i = at_index; i < MAX_RULES_IN_PROGRESS; i++)
    {
        //is next one a null pointer? if so, end of currently stored, break our
        if(working_set[i + 1] == NULL) break;
        //otherwise, move the next in line to our current index
        working_set[i] = working_set[i + 1]; 
        //let our working set know where it is, just in case
        working_set[i]->at_index = i;
    }
    rules_in_progress.num_rules_in_progress -= 1;
    return true;
}

rule_working_t* create_working_set()
{
    return malloc(sizeof(rule_working_t));
}

void delete_working_set(rule_working_t *working)
{
    free(working);
}

rule_t create_rule_from_working_set(rule_working_t *working) {
    rule_t new_rule;
    
    new_rule.prio =         working->prio;
    new_rule.mask =         working->mask_xform.mask;
    new_rule.filter =       working->filter_dtoperand_01.filter;
    new_rule.xform =        working->mask_xform.xform;
    new_rule.idoperand =    working->id_operand_hmac_01.idoperand;

    //There's probably a much more efficient way to accomplish loading the uint16 parcels into the uint64    

    new_rule.dtoperand = working->filter_dtoperand_01.dtoperand01;
    new_rule.dtoperand = new_rule.dtoperand << 16;
    new_rule.dtoperand |= working->dt_operand_02.dtoperand02[0];
    new_rule.dtoperand = new_rule.dtoperand << 16;
    new_rule.dtoperand |= working->dt_operand_02.dtoperand02[1];
    new_rule.dtoperand = new_rule.dtoperand << 16;
    new_rule.dtoperand |= working->dt_operand_02.dtoperand02[2];    
    
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

bool verify_new_rule_sequence(rule_working_t *working)
{
    //TODO: actually check sequence
    
    //stub returns true
    return true;
}

bool verify_new_rule_hmac(rule_working_t *working)
{
    //TODO:actually validate HMAC
    
    //stub returns true
    return true;
}

bool verify_new_rule_complete(rule_working_t *working)
{
    //verify that all bits are set complete in a bitfield
    return (working->bitfield_completed == BITFIELD_COMPLETE);
}

bool handle_new_rule_data_cmd(Union64 *data, int working_set_index)
{
    bool success = false;
    if((working_set_index > -1) == false) return success = false;
    uint8_t cmd = 0;
    get_frame_cmd(data, &cmd);
    switch(cmd){
        case CMD_PREP_01:
        //corresponds to mask and xform information
        get_frame_mask(data, &rules_in_progress.working_sets[working_set_index]->mask_xform.mask);
        get_frame_xform(data, &rules_in_progress.working_sets[working_set_index]->mask_xform.xform);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_01);
        success = true;
        break;
        
        case CMD_PREP_02:
        //filter and dt_operand_01
        get_frame_filter(data, &rules_in_progress.working_sets[working_set_index]->filter_dtoperand_01.filter);
        get_frame_dt_operand_01(data, &rules_in_progress.working_sets[working_set_index]->filter_dtoperand_01.dtoperand01);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_02);
        success = true;
        break;
        
        case CMD_PREP_03:
        get_frame_dt_operand_02(data, &rules_in_progress.working_sets[working_set_index]->dt_operand_02.dtoperand02[0]);
        get_frame_dt_operand_03(data, &rules_in_progress.working_sets[working_set_index]->dt_operand_02.dtoperand02[1]);
        get_frame_dt_operand_04(data, &rules_in_progress.working_sets[working_set_index]->dt_operand_02.dtoperand02[2]);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_03);
        success = true;
        break;
        
        case CMD_PREP_04:
        get_frame_id_operand(data, &rules_in_progress.working_sets[working_set_index]->id_operand_hmac_01.idoperand);
        get_frame_hmac_03(data, &rules_in_progress.working_sets[working_set_index]->id_operand_hmac_01.hmac);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_04);
        success = true;
        break;
        
        case CMD_PREP_05:
        get_frame_hmac_01(data, &rules_in_progress.working_sets[working_set_index]->hmac_02.hmac[0]);
        get_frame_hmac_02(data, &rules_in_progress.working_sets[working_set_index]->hmac_02.hmac[1]);
        get_frame_hmac_03(data, &rules_in_progress.working_sets[working_set_index]->hmac_02.hmac[2]);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_05);
        success = true;
        break;
        
        case CMD_PREP_06:
        get_frame_hmac_01(data, &rules_in_progress.working_sets[working_set_index]->hmac_03.hmac[0]);
        get_frame_hmac_02(data, &rules_in_progress.working_sets[working_set_index]->hmac_03.hmac[1]);
        get_frame_hmac_03(data, &rules_in_progress.working_sets[working_set_index]->hmac_03.hmac[2]);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_06);
        success = true;
        break;
        
        case CMD_PREP_07:
        get_frame_hmac_01(data, &rules_in_progress.working_sets[working_set_index]->hmac_04.hmac[0]);
        get_frame_hmac_02(data, &rules_in_progress.working_sets[working_set_index]->hmac_04.hmac[1]);
        get_frame_hmac_03(data, &rules_in_progress.working_sets[working_set_index]->hmac_04.hmac[2]);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_07);
        success = true;
        break;
        
        case CMD_PREP_08:
        get_frame_hmac_01(data, &rules_in_progress.working_sets[working_set_index]->hmac_05.hmac[0]);
        get_frame_hmac_02(data, &rules_in_progress.working_sets[working_set_index]->hmac_05.hmac[1]);
        get_frame_hmac_03(data, &rules_in_progress.working_sets[working_set_index]->hmac_05.hmac[2]);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_08);
        success = true;
        break;
        
        case CMD_PREP_09:
        get_frame_hmac_01(data, &rules_in_progress.working_sets[working_set_index]->hmac_06.hmac[0]);
        get_frame_hmac_02(data, &rules_in_progress.working_sets[working_set_index]->hmac_06.hmac[1]);
        get_frame_hmac_03(data, &rules_in_progress.working_sets[working_set_index]->hmac_06.hmac[2]);
        set_bitfield_received(&rules_in_progress.working_sets[working_set_index]->bitfield_completed, BITFIELD_POSITION_PREP_09);
        success = true;
        break;
        
        case CMD_STORE:
        //it's a store rule, panic!
        
        //to be written:
        //success = verify_sequence & verify_hmac & verify_rule_complete & store_rule_to_flash
        success = verify_new_rule_sequence(rules_in_progress.working_sets[working_set_index])
        & verify_new_rule_hmac(rules_in_progress.working_sets[working_set_index])
        & verify_new_rule_complete(rules_in_progress.working_sets[working_set_index]);
        
        #if DBG_HMAC
        print_hmac_in_working_set(rules_in_progress.working_sets[working_set_index], true);
        #endif
        
        //verified the rule, assemble and store it
        if (success == true)
        {
            rule_t rule_to_save = create_rule_from_working_set(rules_in_progress.working_sets[working_set_index]);
            success = save_rule_to_flash(&rule_to_save, &flash_can_ruleset[rule_to_save.prio]);
        }
        
        //we got here because of a store command. whether or not we are successful, we should destroy the work in progress
        delete_working_set_managed(rules_in_progress.working_sets, working_set_index);
        break;
        
        default:
        success = false;
    }
    
    return success;
}

//main function for processing incoming new rule data
//assumes this data has already been identified as belonging to a new rule frame
bool handle_new_rule_data(Union64 *data)
{
    //successful handling
    bool success = false;
    //determine prio, ie which rule this frame should correspond to
    uint8_t frame_prio = 0;
    get_frame_prio(data, &frame_prio);
    //start index, will be used to create or assign
    int working_set_index = 0;
    //found matching
    bool prio_match = false;
    //determine if there is already a working set for this rule prio
    for(working_set_index; working_set_index < MAX_RULES_IN_PROGRESS; working_set_index++) {
        if(rules_in_progress.working_sets[working_set_index] == NULL) break;
        prio_match = (frame_prio == rules_in_progress.working_sets[working_set_index]->prio);
        
        if(prio_match == true) {
            //break out and operate on this one corresponding to our prio
            
            break;
        }
    }
    
    //if we found a match from the loop, send it to be processed by cmd
    if (prio_match == true)
    {
        success = handle_new_rule_data_cmd(data, working_set_index);
    }
    
    //if no match was found, we must create a new working set, then send this on to be processed by cmd
    if(prio_match == false)
    {
        working_set_index = create_working_set_managed();
        
        if (working_set_index > -1)
        {
            //set prio of newly created working set to prio in frame we created with
            rules_in_progress.working_sets[working_set_index]->prio = frame_prio;
            success = handle_new_rule_data_cmd(data, working_set_index);
            } else {
            //creation unsuccessful
            success = false;
        }
    }
    
    return success;
}