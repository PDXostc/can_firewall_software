/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * rules_test.c
 *
 */ 
#include "rules_test.h"

//test worst case pass through rule
rule_t rule_test_pass = {
	.dtoperand = 0,
	.filter = 0x000,
	.mask = 0x000,
	.idoperand = 0,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_PASS )
};

// test worst case modify both fields
rule_t rule_test_modify_id_and_data = {
	.dtoperand = 0xffffffffffffffff,
	.filter = 0x00000000,
	.mask = 0x00000000,
	.idoperand = 0xffffffff,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_SET << 4) | XFORM_SET)
	};

rule_t rule_test_block = {
	.dtoperand = 0,
	.filter = 0xFFF,
	.mask = 0xFFF,
	.idoperand = 0,
	.prio = SIZE_RULESET,
	.xform = 0
};

rule_t rule_test_inside_range_allow = {
	.dtoperand = 0xFFAA55,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x7FF,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_PASS)
};

rule_t rule_test_inside_range_xform_id_set = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x7A5,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_SET << 4) | XFORM_PASS)
};

rule_t rule_test_inside_range_xform_id_or = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_OR << 4) | XFORM_PASS)
};

rule_t rule_test_inside_range_xform_id_and = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_AND << 4) | XFORM_PASS)
};

rule_t rule_test_inside_range_xform_id_xor = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_XOR << 4) | XFORM_PASS)
};

rule_t rule_test_inside_range_xform_id_inv = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_INV << 4) | XFORM_PASS)
};

rule_t rule_test_inside_range_xform_id_block = {
	.dtoperand = 0xFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_BLOCK << 4) | XFORM_PASS)
};

//dtoperand test rules
rule_t rule_test_inside_range_xform_data_set = {
	.dtoperand = 0xFFFFFFFFFFFFFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x7A5,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_SET)
};

rule_t rule_test_inside_range_xform_data_or = {
	.dtoperand = 0x0F0F0F0F0F0F0F0F,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_OR)
};

rule_t rule_test_inside_range_xform_data_and = {
	.dtoperand = 0xa5a5a5a5a5a5a5a5,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_AND)
};

rule_t rule_test_inside_range_xform_data_xor = {
	.dtoperand = 0xFFFFFFFFFFFFFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_XOR)
};

rule_t rule_test_inside_range_xform_data_inv = {
	.dtoperand = 0xFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_INV)
};

rule_t rule_test_inside_range_xform_data_block = {
	.dtoperand = 0xFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = ((XFORM_PASS << 4) | XFORM_BLOCK)
};