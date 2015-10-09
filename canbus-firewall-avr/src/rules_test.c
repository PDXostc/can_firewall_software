/*
 * rules_test.c
 *
 * Created: 10/8/2015 11:55:46 AM
 *  Author: smiller6
 */ 
#include "rules_test.h"

//test worst case pass through rule
rule_t rule_test_pass = {
	.dtoperand = 0,
	.filter = 0x000,
	.mask = 0x000,
	.idoperand = 0,
	.prio = SIZE_RULESET,
	.xform = 0
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
	.xform = 0
};

rule_t rule_test_inside_range_xform_id_set = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x7A5,
	.prio = SIZE_RULESET,
	.xform = XFORM_SET << 4
};

rule_t rule_test_inside_range_xform_id_or = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_OR << 4
};

rule_t rule_test_inside_range_xform_id_and = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_AND << 4
};

rule_t rule_test_inside_range_xform_id_xor = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_XOR << 4
};

rule_t rule_test_inside_range_xform_id_inv = {
	.dtoperand = 0,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_INV << 4
};

rule_t rule_test_inside_range_xform_id_block = {
	.dtoperand = 0xFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_BLOCK << 4
};

//dtoperand test rules
rule_t rule_test_inside_range_xform_data_set = {
	.dtoperand = 0xFFFFFFFFFFFFFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x7A5,
	.prio = SIZE_RULESET,
	.xform = XFORM_SET
};

rule_t rule_test_inside_range_xform_data_or = {
	.dtoperand = 0x0F0F0F0F0F0F0F0F,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_OR
};

rule_t rule_test_inside_range_xform_data_and = {
	.dtoperand = 0xFFFFFFFFFFFFFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_AND
};

rule_t rule_test_inside_range_xform_data_xor = {
	.dtoperand = 0xFFFFFFFFFFFFFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_XOR
};

rule_t rule_test_inside_range_xform_data_inv = {
	.dtoperand = 0xFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_INV
};

rule_t rule_test_inside_range_xform_data_block = {
	.dtoperand = 0xFFFF,
	.filter = 0x6F0,
	.mask = 0x7F0,
	.idoperand = 0x00F,
	.prio = SIZE_RULESET,
	.xform = XFORM_BLOCK
};