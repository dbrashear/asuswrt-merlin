/*
 * prf_vectors.h
 * PRF test vectors
 *
 * Copyright (C) 2011, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: prf_vectors.h 241182 2011-02-17 21:50:03Z $
 */

#include <typedefs.h>

/* test vectors from rfc2202 */
uint8 key_0[] = {
	0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	0x0b, 0x0b, 0x0b, 0x0b
	};

uint8 prefix_0[6] = "prefix";

uint8 data_0[8] = "Hi There";

uint8 digest_0[] = {
	0x92, 0x94, 0x72, 0x7a, 0x36, 0x38, 0xbb, 0x1c,
	0x13, 0xf4, 0x8e, 0xf8, 0x15, 0x8b, 0xfc, 0x9d
	};

uint8 digest1_0[] = {
	0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64,
	0xe2, 0x8b, 0xc0, 0xb6, 0xfb, 0x37, 0x8c, 0x8e,
	0xf1, 0x46, 0xbe, 0x00
	};

uint8 prf_0[] = {
	0xbc, 0xd4, 0xc6, 0x50, 0xb3, 0x0b, 0x96, 0x84,
	0x95, 0x18, 0x29, 0xe0, 0xd7, 0x5f, 0x9d, 0x54,
	0xb8, 0x62, 0x17, 0x5e, 0xd9, 0xf0, 0x06, 0x06,
	0xe1, 0x7d, 0x8d, 0xa3, 0x54, 0x02, 0xff, 0xee,
	0x75, 0xdf, 0x78, 0xc3, 0xd3, 0x1e, 0x0f, 0x88,
	0x9f, 0x01, 0x21, 0x20, 0xc0, 0x86, 0x2b, 0xeb,
	0x67, 0x75, 0x3e, 0x74, 0x39, 0xae, 0x24, 0x2e,
	0xdb, 0x83, 0x73, 0x69, 0x83, 0x56, 0xcf, 0x5a
	};


uint8 key_1[4] = "Jefe";

uint8 prefix_1[6] = "prefix";

uint8 data_1[28] = "what do ya want for nothing?";

uint8 digest_1[] = {
	0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03,
	0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38
	};

uint8 digest1_1[] = {
	0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2,
	0xd2, 0x74, 0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c,
	0x25, 0x9a, 0x7c, 0x79
	};

uint8 prf_1[] = {
	0x51, 0xf4, 0xde, 0x5b, 0x33, 0xf2, 0x49, 0xad,
	0xf8, 0x1a, 0xeb, 0x71, 0x3a, 0x3c, 0x20, 0xf4,
	0xfe, 0x63, 0x14, 0x46, 0xfa, 0xbd, 0xfa, 0x58,
	0x24, 0x47, 0x59, 0xae, 0x58, 0xef, 0x90, 0x09,
	0xa9, 0x9a, 0xbf, 0x4e, 0xac, 0x2c, 0xa5, 0xfa,
	0x87, 0xe6, 0x92, 0xc4, 0x40, 0xeb, 0x40, 0x02,
	0x3e, 0x7b, 0xab, 0xb2, 0x06, 0xd6, 0x1d, 0xe7,
	0xb9, 0x2f, 0x41, 0x52, 0x90, 0x92, 0xb8, 0xfc
	};


uint8 key_2[] = {
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA
	};

uint8 prefix_2[6] = "prefix";

uint8 data_2[] = {
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
	0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD
	};

uint8 digest_2[] = {
	0x56, 0xbe, 0x34, 0x52, 0x1d, 0x14, 0x4c, 0x88,
	0xdb, 0xb8, 0xc7, 0x33, 0xf0, 0xe8, 0xb3, 0xf6
	};

uint8 digest1_2[] = {
	0x12, 0x5d, 0x73, 0x42, 0xb9, 0xac, 0x11, 0xcd,
	0x91, 0xa3, 0x9a, 0xf4, 0x8a, 0xa1, 0x7b, 0x4f,
	0x63, 0xf1, 0x75, 0xd3
	};

uint8 prf_2[] = {
	0xe1, 0xac, 0x54, 0x6e, 0xc4, 0xcb, 0x63, 0x6f,
	0x99, 0x76, 0x48, 0x7b, 0xe5, 0xc8, 0x6b, 0xe1,
	0x7a, 0x02, 0x52, 0xca, 0x5d, 0x8d, 0x8d, 0xf1,
	0x2c, 0xfb, 0x04, 0x73, 0x52, 0x52, 0x49, 0xce,
	0x9d, 0xd8, 0xd1, 0x77, 0xea, 0xd7, 0x10, 0xbc,
	0x9b, 0x59, 0x05, 0x47, 0x23, 0x91, 0x07, 0xae,
	0xf7, 0xb4, 0xab, 0xd4, 0x3d, 0x87, 0xf0, 0xa6,
	0x8f, 0x1c, 0xbd, 0x9e, 0x2b, 0x6f, 0x76, 0x07
	};

typedef struct {
	unsigned char *key;
	int key_len;
	unsigned char *prefix;
	int prefix_len;
	unsigned char *data;
	int data_len;
	unsigned char* digest;
	unsigned char* digest1;
	unsigned char* prf;
} prf_vector_t;

#define PRF_VECTOR_ENTRY(x)    \
{ \
	key_##x, sizeof(key_##x), prefix_##x, sizeof(prefix_##x), \
	data_##x, sizeof(data_##x), digest_##x, digest1_##x, prf_##x \
}

prf_vector_t prf_vec[] = {
    PRF_VECTOR_ENTRY(0),
    PRF_VECTOR_ENTRY(1),
    PRF_VECTOR_ENTRY(2)
};

#define NUM_VECTORS  (sizeof(prf_vec)/sizeof(prf_vec[0]))
