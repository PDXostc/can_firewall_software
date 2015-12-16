/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * hmac.c
 *
 */ 

#include "hmac.h"

unsigned char hmac_key[HMAC_KEY_LEN] = "QbA8Q,&GM!#(Uiv'4b'6G^HCKL@Fi[@ZAU_C`qb8";

unsigned char payload_signature_buffer[PAYLOAD_SIG_BUF_LEN] = {0};

unsigned char hmac_sum[HMAC_SIZE] = {0};
unsigned char hmac_compare_buffer[HMAC_SIZE] = {0};