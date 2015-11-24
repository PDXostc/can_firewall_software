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

// to build an id from bits
#define OFFSET_IN_EID_TO_32_17_16	(27)
#define OFFSET_IN_EID_TO_32_15_8	(19)
#define OFFSET_IN_EID_TO_32_7_0		(11)
#define OFFSET_IN_STD_TO_32_10_3	(3)
#define OFFSET_IN_STD_TO_32_2_0		(0)

void translate_id_mcp_to_U32(uint8_t *msg, uint32_t *out_id)
{
	// IDE bit is << 3 in component [1] of MCP msg
	uint8_t extid = ((msg[MCP_BYTE_DLC] & MCP_MASK_IDE_BIT) >> MCP_OFFSET_OUT_IDE_BIT);
	
	if (extid > 0)
	{
		// treat as extended id and copy out 4 bytes to 29 bit id
		// start with most significant, shift in
		*out_id |= ((U32)(msg[MCP_BYTE_SIDL] & MCP_MASK_EID_BITS_17_16)) << OFFSET_IN_EID_TO_32_17_16;
		
		*out_id |= ((U32)msg[MCP_BYTE_EID8] & MCP_MASK_EID_BITS_15_8) << OFFSET_IN_EID_TO_32_15_8;
		
		*out_id |= ((U32)msg[MCP_BYTE_EID0] & MCP_MASK_EID_BITS_7_0) << OFFSET_IN_EID_TO_32_7_0;
		
		*out_id |= ((U32)msg[MCP_BYTE_SIDH] & MCP_MASK_STD_BITS_10_3) << OFFSET_IN_STD_TO_32_10_3;
		
		*out_id |= ((U32)msg[MCP_BYTE_SIDL] & MCP_MASK_STD_BITS_2_0) << OFFSET_IN_STD_TO_32_2_0;
	} 
	else
	{
		//treat as standard id and copy out 2 bytes to 11 bit id
		*out_id |= ((U32)msg[MCP_BYTE_SIDH] & MCP_MASK_STD_BITS_10_3) << OFFSET_IN_STD_TO_32_10_3;
		
		*out_id |= ((U32)msg[MCP_BYTE_SIDL] & MCP_MASK_STD_BITS_2_0) << OFFSET_IN_STD_TO_32_2_0;
	}
}