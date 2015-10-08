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
