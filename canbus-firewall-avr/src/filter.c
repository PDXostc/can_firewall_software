/*
 * filter.c
 *
 * Created: 10/8/2015 12:26:49 PM
 *  Author: smiller6
 */ 

#include "filter.h"



inline int operate_transform_id(volatile can_msg_t *msg, U32 *rule_operand, int xform)
{
	
	
	switch(xform)
	{
		case XFORM_SET:
		msg->id = *rule_operand;
		break;
		
		case XFORM_OR:
		msg->id |= *rule_operand;
		break;
		
		case XFORM_AND:
		msg->id &= *rule_operand;
		break;
		
		case XFORM_XOR:
		msg->id ^= *rule_operand;
		break;
		
		case XFORM_INV:
		msg->id = ~msg->id;
		break;
		
		case XFORM_PASS:
		break;
		
		case XFORM_BLOCK:
		//return discard
		//wipe id so it is not transmitted
		msg->id = 0;
		return 1;
		
		default:
		//encountered unhandled xform, return failure
		return -1;
		//break;
	}
	return 0;
}

inline int operate_transform_u64(U64 *data, U64 *rule_operand, int xform)
{
	switch(xform)
	{
		case XFORM_SET:
		*data = *rule_operand;
		break;
		
		case XFORM_OR:
		*data |= *rule_operand;
		break;
		
		case XFORM_AND:
		*data &= *rule_operand;
		break;
		
		case XFORM_XOR:
		*data ^= *rule_operand;
		break;
		
		case XFORM_INV:
		*data = ~*data;
		break;
		
		case XFORM_PASS:
		break;
		
		case XFORM_BLOCK:
		//return discard
		//wipe id so it is not transmitted
		*data = 0;
		return 1;
		
		default:
		//encountered unhandled xform, return failure
		return -1;
		break;
	}
	return 0;
}

inline enum Eval_t evaluate(volatile can_mob_t *msg, rule_t *ruleset, rule_t **out_rule){
	//note: does not handle extended CAN yet
	
	//if new rule case, check for shunt connection	
	if (msg->can_msg->id == msg_new_rule.id)
	{
		if (test_loopback() == true)
		{
			return NEW;
		} else {
			//new rule attempted without proper connection; no mercy.
			return DISCARD;
		}
	}
	
	int i = 0;
	while(i != SIZE_RULESET){
		//look for match
		//test of values:
		// 		int and_msg = msg->can_msg->id & ruleset[i].mask;
		// 		int filter = ruleset[i].filter;
		
		if((msg->can_msg->id & ruleset[i].mask) == ruleset[i].filter)
		{
			//match found, set out_rule and return evaluation case
			*out_rule = &ruleset[i];
			return FILTER;
		}
		
		i += 1;
	}
	
	//got here without any match
	return DISCARD;
}