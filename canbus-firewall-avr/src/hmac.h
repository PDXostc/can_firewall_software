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

#define HMAC_KEY_LEN_BASE               40
#define HMAC_KEY_LEN                    HMAC_KEY_LEN_BASE

#define PAYLOAD_SIG_BUF_LEN_STAN        29
#define PAYLOAD_SIG_BUF_LEN             PAYLOAD_SIG_BUF_LEN_STAN
#define PAYLOAD_SIG_BUF                 payload_signature_buffer

#define HMAC_SHA_256_BUFFER_SIZE        32
#define HMAC_SIZE                       HMAC_SHA_256_BUFFER_SIZE

//hmac keys live in flash
#if defined (__GNUC__)
__attribute__((__section__(".flash_rsvd")))
#endif
extern unsigned char hmac_key[HMAC_KEY_LEN]
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