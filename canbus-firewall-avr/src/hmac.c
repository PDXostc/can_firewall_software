/*
 * hmac.c
 *
 * Created: 9/16/2015 2:52:37 PM
 *  Author: smiller6
 */ 

#include "hmac.h"

unsigned char hmac_key[HMAC_KEY_LEN] = "qgu1lF7k3q4i2bpwS4NGA87Jh2PQMtvFu9k6X6lj";

unsigned char payload_signature_buffer[PAYLOAD_SIG_BUF_LEN] = {0};

unsigned char hmac_sum[HMAC_SIZE] = {0};
unsigned char hmac_compare_buffer[HMAC_SIZE] = {0};