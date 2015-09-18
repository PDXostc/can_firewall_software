/*
 * hmac.c
 *
 * Created: 9/16/2015 2:52:37 PM
 *  Author: smiller6
 */ 

#include "hmac.h"

unsigned char hmac_key[HMAC_KEY_LEN] = "qgu1lF7k3q4i2bpwS4NGA87Jh2PQMtvFu9k6X6lj";
unsigned char hmac_key_alternate[HMAC_KEY_LEN] = "NO! Trynot! Do,ordonot. There is no try.";
unsigned char hmac_key_empty[HMAC_KEY_LEN] = {0};
/*unsigned char hmac_key_zero[HMAC_KEY_LEN_ZERO] = {0};*/
unsigned char hmac_key_zero[HMAC_KEY_LEN_ZERO] = "";
const int hmac_keylen = HMAC_KEY_LEN;

unsigned char hmac_key_double[HMAC_KEY_LEN_DOUBLE] = "716775316C46376B337134693262707753344E474138374A683250514D74764675396B3658366C6A00\0";

unsigned char payload_signature_buffer[PAYLOAD_SIG_BUF_LEN_STAN_0] = {0};
unsigned char payload_signature_buffer_double[PAYLOAD_SIG_BUF_LEN_DOUBLE] = {0};
const int payload_signature_buffer_len = PAYLOAD_SIG_BUF_LEN_STAN_0;

extern unsigned char payload_signature_hard[PAYLOAD_SIG_BUF_LEN_STAN] = "040000FFFF01000000AAC10807060504030201FFFF0000000000010000";
extern unsigned char payload_signature_hard_double[PAYLOAD_SIG_BUF_LEN_DOUBLE_0] = {0};

unsigned char hmac_sum[32] = {0};
unsigned char hmac_compare_buffer[32] = {0};
unsigned int hmac_buffer_len = 32;