/*
 * Test.c
 *
 * Created: 9/16/2015 10:53:59 AM
 *  Author: smiller6
 */ 

#include "test.h"

/* TESTS ****************************************************************************************************/

//silly, get rid of this later
void print_success(bool success)
{
    if (success == true)
    {
        print_dbg("\n\rSUCCESS\n\r");
    }
    else
    {
        print_dbg("\n\rFAIL\n\r");
    }
}

bool test_new_rule_creation(void)
{
    //sequence:
    //throw our frames at the handle function
    //should see one working set created
    //store rule should trigger assemblage of working set to rule
    //save rule
    //print rule
    
    bool success = false;
    int count = 0;
    uint8_t prio_test;
    get_frame_prio(&msg_prep_01.data, &prio_test);
    #if DBG_RULES
    print_dbg("\n\rExpect Prio: ");
    print_dbg_char_hex(prio_test);
    print_dbg("\n\r\n\r");
    #endif
    
    success = handle_new_rule_data(&msg_prep_01.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_02.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_03.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_04.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_05.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_06.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_07.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_08.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_09.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    #endif
    
    success = handle_new_rule_data(&msg_prep_10.data);
    #if DBG_RULES
    print_success(success);
    print_dbg_hex(count += 1);
    
    print_rule(&flash_can_ruleset[prio_test]);
    #endif
    
    return success;
    
}