/*
 * filter.h
 *
 * Created: 10/8/2015 12:24:42 PM
 *  Author: smiller6
 */ 


#ifndef FILTER_H_
#define FILTER_H_

#include "asf.h"
#include "rules.h"
#include "conf_messages.h"

#define OVERRIDE_DETECTED_SHUNT   true
#define DETECTED_SHUNT            OVERRIDE_DETECTED_SHUNT

//physical security shunt, override to true during software testing
//if this is true, we can accept new rules
// TODO: detect actual pin state dedicated to shunt
static bool detected_shunt = DETECTED_SHUNT;

const enum Eval_t {
	DISCARD, NEW, FILTER
} Eval_t;

extern int operate_transform_id(volatile can_msg_t *msg, U32 *rule_operand, int xform);

extern int operate_transform_u64(U64 *data, U64 *rule_operand, int xform);

extern enum Eval_t evaluate(volatile can_mob_t *msg, rule_t *ruleset, rule_t **out_rule);

#endif /* FILTER_H_ */