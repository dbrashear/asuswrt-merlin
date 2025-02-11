/*
 * WPS Utils
 *
 * Copyright (C) 2011, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: $
 */

#ifndef _WPS_UTILS_
#define _WPS_UTILS_

#include <stdio.h>

#if defined(WIN32) || defined(WINCE)
#define WPS_snprintf	_snprintf
#else
#define WPS_snprintf	snprintf
#endif

char *wps_strncpy(char *dest, const char *src, size_t n);
bool wps_validate_pin(char *pinStr);
uint32 wps_gen_pin(char *devPwd, int devPwd_len);
bool wps_gen_ssid(char *ssid, int ssid_len, char *prefix, char *hwaddr);
bool wps_gen_key(char *key, int key_len);

#endif /* _WPS_UTILS_ */
