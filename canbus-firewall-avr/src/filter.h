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

extern int operate_transform_id(volatile can_msg_t *msg, U32 *rule_operand, int xform);

extern int operate_transform_u64(U64 *data, U64 *rule_operand, int xform);

#endif /* FILTER_H_ */