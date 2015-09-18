/*
 * hmac.h
 *
 * Created: 9/16/2015 2:52:24 PM
 *  Author: smiller6
 */ 


#ifndef HMAC_H_
#define HMAC_H_

#include <asf.h>

//forty character string = qgu1lF7k3q4i2bpwS4NGA87Jh2PQMtvFu9k6X6lj

#define HMAC_KEY hmac_key
#define HMAC_KEY_LEN 41
#define PAYLOAD_SIG_BUF_LEN 30

//hmac keys live in flash
#if defined (__GNUC__)
__attribute__((__section__(".flash_rsvd")))
#endif
extern unsigned char hmac_key[HMAC_KEY_LEN];
extern unsigned char hmac_key_alternate[HMAC_KEY_LEN];
unsigned char hmac_key_empty[HMAC_KEY_LEN];
extern const int hmac_keylen;
#if defined (__ICAVR32__)
@ "FLASHRSVD"
#endif
;


extern unsigned char payload_signature_buffer[PAYLOAD_SIG_BUF_LEN];
extern const int payload_signature_buffer_len;

extern unsigned char hmac_sum[32];
extern unsigned char hmac_compare_buffer[32];
extern unsigned int hmac_buffer_len;


#endif /* HMAC_H_ */