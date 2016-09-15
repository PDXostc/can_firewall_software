/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * filter.c
 *
 */ 

#include "filter.h"


// deprecated function for direct operation on an id contained in an Atmel storage structure
inline int operate_transform_id_atmel(volatile can_msg_t *msg, U32 *rule_operand, int xform)
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
		msg->id &= (~*rule_operand);
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

// transform a generic 32b id number according the the CAN firewall xform
inline int operate_transform_id(uint32_t *msg_id, uint32_t *rule_operand, int xform)
{
	
	
	switch(xform)
	{
		case XFORM_SET:
		*msg_id = *rule_operand;
		break;
		
		case XFORM_OR:
		*msg_id |= *rule_operand;
		break;
		
		case XFORM_AND:
		*msg_id &= *rule_operand;
		break;
		
		case XFORM_XOR:
		// toggle flagged bits
		*msg_id ^= *rule_operand;
		break;
		
		case XFORM_INV:
		// negate flagged bits, msg & inverse of rule operand to clear bits
		*msg_id &= (~*rule_operand);
		break;
		
		case XFORM_PASS:
		break;
		
		case XFORM_BLOCK:
		//return discard
		//wipe id so it is not transmitted
		*msg_id = 0;
		return 1;
		
		default:
		//encountered unhandled xform, return failure
		return -1;
		//break;
	}
	return 0;
}

// transform a generic 64b data var according to the CAN firewall xform
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

// deprecated function for evaluation of an Atmel stored CAN message
// inline enum Eval_t evaluate(volatile can_mob_t *msg, rule_t *ruleset, rule_t **out_rule){
// 	//note: does not handle extended CAN yet
// 	
// 	//if new rule case, check for shunt connection	
// 	if (msg->can_msg->id == msg_new_rule.id)
// 	{
// 		if (test_loopback() == true)
// 		{
// 			return NEW;
// 		} else {
// 			//new rule attempted without proper connection; no mercy.
// 			return DISCARD;
// 		}
// 	}
// 	
// 	int i = 0;
// 	while(i != SIZE_RULESET){
// 		//look for match
// 		//test of values:
// 		// 		int and_msg = msg->can_msg->id & ruleset[i].mask;
// 		// 		int filter = ruleset[i].filter;
// 		
// 		if((msg->can_msg->id & ruleset[i].mask) == ruleset[i].filter)
// 		{
// 			//match found, set out_rule and return evaluation case
// 			*out_rule = &ruleset[i];
// 			return FILTER;
// 		}
// 		
// 		i += 1;
// 	}
// 	
// 	//got here without any match
// 	return DISCARD;
// }

// evaluate a 32b message id against a provided CAN firewall ruleset. provide a
// pointer to applicable rule, if found
enum Eval_t evaluate_msg_id(uint32_t msg_id, rule_t *ruleset, rule_t **out_rule)
{
	//if new rule case, check for shunt connection
	if (msg_id == msg_new_rule.id)
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
			
		if((msg_id & ruleset[i].mask) == ruleset[i].filter)
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

// to build a 32b id from MCP bits
#define OFFSET_IN_EID_TO_32_17_16	(27)
#define OFFSET_IN_EID_TO_32_15_8	(19)
#define OFFSET_IN_EID_TO_32_7_0		(11)
#define OFFSET_IN_STD_TO_32_10_3	(3)
#define OFFSET_IN_STD_TO_32_2_0		(0)

void translate_id_mcp_to_U32(volatile uint8_t *msg, uint32_t *out_id)
{
	//zero the id destination before use
	*out_id = 0;
	
	// IDE bit is << 3 in component [1] of MCP msg
	uint8_t extid = ((msg[MCP_BYTE_SIDL] & MCP_MASK_IDE_BIT) >> MCP_OFFSET_OUT_IDE_BIT);
	
	if (extid > 0)
	{
		// treat as extended id and copy out 4 bytes to 29 bit id
		// start with most significant, shift in
		*out_id |= ((U32)(msg[MCP_BYTE_SIDL] & MCP_MASK_EID_BITS_17_16)) << OFFSET_IN_EID_TO_32_17_16;
		
		*out_id |= ((U32)msg[MCP_BYTE_EID8] & MCP_MASK_EID_BITS_15_8) << OFFSET_IN_EID_TO_32_15_8;
		
		*out_id |= ((U32)msg[MCP_BYTE_EID0] & MCP_MASK_EID_BITS_7_0) << OFFSET_IN_EID_TO_32_7_0;
		
		*out_id |= ((U32)msg[MCP_BYTE_SIDH] & MCP_MASK_STD_BITS_10_3) << OFFSET_IN_STD_TO_32_10_3;
		
		*out_id |= ((U32)((msg[MCP_BYTE_SIDL] & MCP_MASK_STD_BITS_2_0) >> MCP_OFFSET_OUT_STD_BITS_2_0))<< OFFSET_IN_STD_TO_32_2_0;
	} 
	else
	{
		//treat as standard id and copy out 2 bytes to 11 bit id
		*out_id |= ((U32)msg[MCP_BYTE_SIDH] & MCP_MASK_STD_BITS_10_3) << OFFSET_IN_STD_TO_32_10_3;
		
		*out_id |= ((U32)((msg[MCP_BYTE_SIDL] & MCP_MASK_STD_BITS_2_0) >> MCP_OFFSET_OUT_STD_BITS_2_0)) << OFFSET_IN_STD_TO_32_2_0;
	}
}

// defines for translating a 32b to MCP standard
#define CAN_ID_STD_MASK				0x7FF
#define CAN_ID_STD_BITS_10_3_MASK	0x7F8
#define CAN_ID_EXT_BITS_17_16_MASK	0x1800000
#define CAN_ID_STD_BITS_2_0_MASK	0x07
#define CAN_ID_EXT_BITS_15_8_MASK	0x07F80000
#define CAN_ID_EXT_BITS_7_0_MASK	0x0007F800
// copy from an unsigned 32b int to the MCP id message byte format
// TODO: provide definitions for the hard coded numbers (masks) in this function
void translate_id_U32_to_mcp(volatile uint8_t *msg, uint32_t *in_id)
{
	//if id is greater than 11b, it is in extended format
	uint8_t extid = 0x00;
	if (*in_id > CAN_ID_STD_MASK)
	{
		extid = 0x01;
	}
	
	if (extid > 0)
	{
		// treat as extended, copy the 29b into the MCP byte format
		*in_id &= 0x1FFFFFFF;
		// most significant bits of std go in SIDH
		msg[MCP_BYTE_SIDH] = (uint8_t)((*in_id & CAN_ID_STD_BITS_10_3_MASK) >> 3);
		// most significant bits of eid, and least of standard go in SIDL register
		msg[MCP_BYTE_SIDL] = ((uint8_t)((*in_id & CAN_ID_EXT_BITS_17_16_MASK) >> 27)) |\
							 ((uint8_t)((extid & 0x01) << 3)) |\
							 ((uint8_t)((*in_id & CAN_ID_STD_BITS_2_0_MASK) << 5));
		// bits 15:8 (26:19) of eid go in EID8 register, can be shifted out by the same offset used to create the U32
		msg[MCP_BYTE_EID8] = (uint8_t)((*in_id & CAN_ID_EXT_BITS_15_8_MASK) >> OFFSET_IN_EID_TO_32_15_8);
		// bits 7:0 (18:11) of eid go in EID0 register, can be shifted out by the same offset used to create the U32
		msg[MCP_BYTE_EID0] = (uint8_t)((*in_id & CAN_ID_EXT_BITS_7_0_MASK) >> OFFSET_IN_EID_TO_32_7_0);
	} 
	else
	{
		// treat as standard format, copy only 11b into the MCP byte format
		*in_id &= 0x7FF;
		// consider only SIDH and SIDL and care only about the standard id
		msg[MCP_BYTE_SIDH] = (uint8_t)((*in_id & CAN_ID_STD_BITS_10_3_MASK) >> 3);
		msg[MCP_BYTE_SIDL] = ((uint8_t)((*in_id & CAN_ID_STD_BITS_2_0_MASK) << 5) |\
							 ((uint8_t)(extid)));
	}
}

extern void translate_data_mcp_to_U64(volatile uint8_t *msg, U64 *out_data)
{
	// zero the data destination before use
	*out_data = 0;
	
	//get dlc length of message (from mcp byte format) and use to inform copy length
	uint8_t length = msg[MCP_BYTE_DLC] & MCP_MASK_DLC;
	
	//ensure data length not greater than 8 bytes
	if (length > 0x08)
	{
		length = 0x08;
	}
	
	// our shift factor needs to preserve most significant bit order. 
	// data lengths should still fill the most significant bits first.
	uint8_t shift_factor = MCP_CAN_MSG_DATA_SIZE - 1;
	
	// build the 64b number from the unsigned 8b data bytes in mcp format
	// from most to least significant
	for(int i = 0; i < length; i++)
	{
		*out_data |= (((U64)msg[MCP_BYTE_D0 + i]) << shift_factor * MCP_CAN_MSG_DATA_SIZE);
		
		//decrement shift count
		shift_factor--;	
	}
}

extern void translate_data_U64_to_mcp(volatile uint8_t *msg, U64 *in_data)
{
	//shift factor for most significant bits to be moved and masked
	uint8_t shift_factor = 7;
	
	// move data from 64b temp to MCP byte format
	for (int i = 0; i < 8; i++)
	{
		msg[MCP_BYTE_D0 + i] = (uint8_t)(*in_data >> (shift_factor * 8));
		
		shift_factor--;
	}
}
