/*
 * hmac.c
 *
 * Created: 9/16/2015 2:52:37 PM
 *  Author: smiller6
 */ 

#include "hmac.h"

unsigned char hmac_key[HMAC_KEY_LEN] = "QbA8Q,&GM!#(Uiv'4b'6G^HCKL@Fi[@ZAU_C`qb8";

unsigned char payload_signature_buffer[PAYLOAD_SIG_BUF_LEN] = {0};

unsigned char hmac_sum[HMAC_SIZE] = {0};
unsigned char hmac_compare_buffer[HMAC_SIZE] = {0};