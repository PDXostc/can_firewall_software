/*
 * hmac.c
 *
 * Created: 9/16/2015 2:52:37 PM
 *  Author: smiller6
 */ 

#include "hmac.h"

unsigned char hmac_key[40] = "qgu1lF7k3q4i2bpwS4NGA87Jh2PQMtvFu9k6X6lj";
unsigned char hmac_key_alternate[40] = "NO! Trynot! Do,ordonot. There is no try.";
const int hmac_keylen = 40;

unsigned char payload_signature_buffer[29] = {0};
const int payload_signature_buffer_len = 29;

unsigned char hmac_sum[32] = {0};
unsigned char hmac_compare_buffer[32] = {0};
unsigned int hmac_buffer_len = 32;