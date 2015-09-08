/*
 * conf_rules.h
 *
 * Created: 8/31/2015 1:12:49 PM
 *  Author: smiller6
 */ 


#ifndef CONF_RULES_H_
#define CONF_RULES_H_

//Memory structures, proposed

//+ New Rule Working Set
//typedef unsigned char uint8_t;   //(1 byte, 8 bit)
//typedef unsigned short uint16_t; //(2 byte, 16 bit)
//typedef unsigned int uint32_t;     //(4 byte, 32 bit)


//prep rule 01 frame
typedef struct {
    uint32_t mask;// = 0x04030201;
    uint8_t xform;// = 0x01;
    //rsdv 0x01
} rule_prep_01;

//prep rule 02 frame
typedef struct {
    uint32_t filter;// = 0x04030201;
    uint16_t dtoper01;// = 0x0201;
} rule_prep_02;

//prep rule 03 frame
typedef struct {
    uint16_t dtoper02[3];// = {0x0605,0x0403,0x0201};
} rule_prep_03;

//prep rule 04 frame
typedef struct {
    uint32_t idoper;// = 0x04030201;
    uint16_t hmac[1];// = 0x0201;
} rule_prep_04;

//prep rule 05 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_05;

//prep rule 06 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_06;

//prep rule 07 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_07;

//prep rule 08 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_08;

//prep rule 09 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
} rule_prep_09;

//prep rule 10 frame
typedef struct {
    uint16_t hmac[3];// = {0x0605,0x0403,0x0201};
    //unused 0x0201
} rule_prep_10;

typedef struct {
    uint32_t sequence;// = 0x000000; //init to zero so it is never higher than we expect
    //unused 0x0201
} rule_store;

//working set memory storage
typedef struct {
    uint8_t Prio;
    uint16_t bitfield_completed;
    //structs for rules in progress
    rule_prep_01 prep_01;
    rule_prep_02 prep_02;
    rule_prep_03 prep_03;
    rule_prep_04 prep_04;
    rule_prep_05 prep_05;
    rule_prep_06 prep_06;
    rule_prep_07 prep_07;
    rule_prep_08 prep_08;
    rule_prep_09 prep_09;
    rule_prep_10 prep_10;
    rule_store store;
} rule_working_t;



#endif /* CONF_RULES_H_ */