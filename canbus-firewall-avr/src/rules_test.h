/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * rules_test.h
 *
 */ 


#ifndef RULES_TEST_H_
#define RULES_TEST_H_

#include "rules.h"

//test pass all rule
rule_t rule_test_pass;
rule_t rule_test_modify_id_and_data;
rule_t rule_test_block;
rule_t rule_test_inside_range_allow;
rule_t rule_test_inside_range_xform_id_set;
rule_t rule_test_inside_range_xform_id_or;
rule_t rule_test_inside_range_xform_id_and;
rule_t rule_test_inside_range_xform_id_xor;
rule_t rule_test_inside_range_xform_id_inv;
rule_t rule_test_inside_range_xform_id_block;
rule_t rule_test_inside_range_xform_id_pass;
rule_t rule_test_inside_range_xform_data_set;
rule_t rule_test_inside_range_xform_data_or;
rule_t rule_test_inside_range_xform_data_and;
rule_t rule_test_inside_range_xform_data_xor;
rule_t rule_test_inside_range_xform_data_inv;
rule_t rule_test_inside_range_xform_data_block;
rule_t rule_test_inside_range_xform_data_pass;

#endif /* RULES_TEST_H_ */