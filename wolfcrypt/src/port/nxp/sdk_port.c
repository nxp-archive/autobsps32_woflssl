/* sdk_port.c
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(NXP_SDK)

#include <wolfssl/wolfcrypt/port/nxp/sdk_port.h>

#if defined(NXP_SDK_HSM)
#include "hsm_driver.h"
static hsm_state_t g_hsm_state;
#endif /* NXP_SDK_HSM */

#if defined(NXP_SDK_RTC_C55)
#include "lwip/def.h"
#include "rtc_c55_driver.h"
#define RTC_INSTANCE 0U
static rtc_state_t g_rtc_state;
static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
#endif /* NXP_SDK_RTC_C55 */

int nxp_sdk_port_init(void)
{
    status_t ret = STATUS_SUCCESS;

#if defined(NXP_SDK_HSM)
    ret = HSM_DRV_Init(&g_hsm_state);
    DEV_ASSERT(ret == STATUS_SUCCESS);
    ret = HSM_DRV_InitRNG(HSM_TIMEOUT);
    DEV_ASSERT(ret == STATUS_SUCCESS);
#endif /* NXP_SDK_HSM */

#if defined(NXP_SDK_RTC_C55)
    rtc_init_config_t rtcConfig;
    RTC_DRV_GetDefaultConfig(&rtcConfig);
    RTC_DRV_Init(RTC_INSTANCE, &g_rtc_state, &rtcConfig);

    /* Initialize Time & Date based on host system's clock at compile time.
       This may need to be initialized more accurately.
    */
    rtc_timedate_t timedate;
    char dbuff[] = __DATE__;
    char tbuff[] = __TIME__;
    
    timedate.year = (uint16_t)atoi((const char*)&dbuff[7]);
    dbuff[6] = '\0';
    timedate.day = (uint16_t)atoi((const char*)&dbuff[4]);
    dbuff[3] = '\0';
    timedate.month = (uint16_t)((lwip_strnstr(month_names, dbuff, sizeof(month_names)) - month_names) / 3 + 1);

    timedate.seconds = (uint8_t)atoi((const char*)&tbuff[6]);
    tbuff[5] = '\0';
    timedate.minutes = (uint16_t)atoi((const char*)&tbuff[3]);
    tbuff[2] = '\0';
    timedate.hour = (uint16_t)atoi((const char*)&tbuff[0]);
    
    ret = RTC_DRV_SetTimeDate(RTC_INSTANCE, &timedate);
    DEV_ASSERT(ret == STATUS_SUCCESS);
    ret = RTC_DRV_StartCounter(RTC_INSTANCE);
    DEV_ASSERT(ret == STATUS_SUCCESS);
#endif /* NXP_SDK_RTC_C55 */

    return ret;
}

#if defined(NXP_SDK_RTC_C55)
uint32_t nxp_sdk_rtc_time(void)
{
    uint32_t secs;
    rtc_timedate_t crt;
    RTC_DRV_GetTimeDate(RTC_INSTANCE, &crt);
    RTC_DRV_ConvertTimeDateToSeconds(&crt, &secs);
    return secs;
}
#endif /* NXP_SDK_RTC_C55 */

void nxp_sdk_port_cleanup(void)
{
#if defined(NXP_SDK_HSM)
    HSM_DRV_Deinit();
#endif /* NXP_SDK_HSM */

#if defined(NXP_SDK_RTC_C55)
    RTC_DRV_Deinit(RTC_INSTANCE);
#endif /* NXP_SDK_RTC_C55 */
}

#endif /* NXP_SDK */
