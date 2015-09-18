/*
 * hmac.c
 *
 * Created: 9/16/2015 2:52:37 PM
 *  Author: smiller6
 */ 

#include "hmac.h"

unsigned char hmac_key[HMAC_KEY_LEN] = "qgu1lF7k3q4i2bpwS4NGA87Jh2PQMtvFu9k6X6lj\0";
unsigned char hmac_key_alternate[HMAC_KEY_LEN] = "NO! Trynot! Do,ordonot. There is no try.";
unsigned char hmac_key_empty[HMAC_KEY_LEN] = {0};
const int hmac_keylen = HMAC_KEY_LEN;



unsigned char payload_signature_buffer[PAYLOAD_SIG_BUF_LEN] = {0};
const int payload_signature_buffer_len = PAYLOAD_SIG_BUF_LEN;

unsigned char hmac_sum[32] = {0};
unsigned char hmac_compare_buffer[32] = {0};
unsigned int hmac_buffer_len = 32;