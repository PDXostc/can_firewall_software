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
#include "loopback.h"
#include "conf_messages.h"
#include "mcp_definitions.h"

/* Evaluation cases to be returned by evaluate function.
 * DISCARD: No matching rule was found, please discard message
 * NEW: Message matches hard coded id for new rules, please treat data as new rule frame
 * FILTER: Message matches rule found in set, please filter accordingly
 */
const enum Eval_t {
	DISCARD, NEW, FILTER
} Eval_t;

/* Global structure for temporary storage of message evaluation data. This is used
 * to hold the translated CAN id and/or CAN data for use during the ruleset compare
 * stage or rule ingestion.
 */
struct Eval_temp_t {
	uint32_t id;
	Union64 data;
} Eval_temp_t;

struct Eval_temp_t Eval_temp;

/**
 * \brief Operate on id field of provided CAN message using operand value and 
 * transform. Transforms are defined in rules and according to spec. Xform value
 * must be provided (masked and shifted) so that it corresponds with the correct
 * half byte value. For transforming an id, supply (xform = (xform_byte & 0xF0) >> 4)
 * 
 * \param msg Pointer to CAN message to be transformed
 * \param rule_operand Value to modify id by
 * \param xform Half byte value corresponding to predefined transform operation
 * 
 * \return extern int 0 == success, 1 == encountered block xform, -1 == encountered unexpected value
 */
extern int operate_transform_id_atmel(volatile can_msg_t *msg, U32 *rule_operand, int xform);

/**
 * \brief TODO
 * 
 * \param msg_id
 * \param rule_operand
 * \param xform
 * 
 * \return extern int
 */
extern int operate_transform_id(uint32_t *msg_id, uint32_t *rule_operand, int xform);

/**
 * \brief Operate on data field of provided CAN message using operand value and
 * transform. Transforms are defined in rules and according to spec. Xform value
 * must be provided (masked) so that it corresponds with the correct
 * half byte value. For transforming an id, supply (xform = (xform_byte & 0x0F))
 * 
 * \param data Pointer to data payload to be transformed
 * \param rule_operand Value to modify data by
 * \param xform Half byte value corresponding to predefined transform operation
 * 
 * \return extern int 0 == success, 1 == encountered block xform, -1 == encountered unexpected value
 */
extern int operate_transform_u64(U64 *data, U64 *rule_operand, int xform);

/**
 * \brief Evalutate CAN message against provided ruleset. Returns enumeration for
 * recommendation of how to handle message based on whether a match was found.
 * If a match exists, pointer value to rule will be written to the out_rule.
 * Note that if no match is found, the function will recommend DISCARD. For a NEW
 * rule match, the boolean to check the physical shunt must be in place, and an id
 * for new rules must be defined.
 * 
 * \param msg Pointer to message to evaluate
 * \param ruleset Pointer to ruleset array to iterate message against
 * \param out_rule Double pointer to hold address of matched rule, if found
 * 
 * \return extern enum Eval_t DISCARD == no match found, NEW == shunt connected and id matches new rule signature, FILTER == match found
 */
extern enum Eval_t evaluate(volatile can_mob_t *msg, rule_t *ruleset, rule_t **out_rule);

/**
 * \brief Evalutate CAN ID against provided ruleset. Returns enumeration for
 * recommendation of how to handle message based on whether a match was found.
 * If a match exists, pointer value to rule will be written to the out_rule.
 * Note that if no match is found, the function will recommend DISCARD. For a NEW
 * rule match, the boolean to check the physical shunt must be in place, and an id
 * for new rules must be defined.
 * \param msg_id 32b id number pulled from message
 * \param ruleset Pointer to ruleset array to iterate id against
 * \param out_rule Double pointer to hold address of matched rule from set, if found
 * 
 * \return extern enum Eval_t DISCARD == no match found, NEW == shunt connected and id matches new rule signature, FILTER == match found
 */
extern enum Eval_t evaluate_msg_id(uint32_t msg_id, rule_t *ruleset, rule_t **out_rule);


/**
 * \brief TODO
 * 
 * \param msg
 * \param out_id
 * 
 * \return extern void
 */
extern void translate_id_mcp_to_U32(volatile uint8_t *msg, uint32_t *out_id);
/**
 * \brief TODO
 * 
 * \param msg
 * \param in_id
 * 
 * \return extern void
 */
extern void translate_id_U32_to_mcp(volatile uint8_t *msg, uint32_t *in_id);
/**
 * \brief TODO
 * 
 * \param msg
 * \param out_data
 * 
 * \return extern void
 */
extern void translate_data_mcp_to_U64(volatile uint8_t *msg, U64 *out_data);
/**
 * \brief TODO
 * 
 * \param msg
 * \param in_data
 * 
 * \return extern void
 */
extern void translate_data_U64_to_mcp(volatile uint8_t *msg, U64 *in_data);
#endif /* FILTER_H_ */