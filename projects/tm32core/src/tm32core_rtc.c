/**
 * @file    tm32core_rtc.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Real-Time Clock (RTC) component structure
 *          and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_rtc.h>

/* Private Structures *********************************************************/

struct TM32Core_RTC
{
    TM32Core_Bus*   bus;        /** @brief The TM32 Bus to which this RTC is connected. */
};

/* Public Functions ***********************************************************/

TM32Core_RTC* TM32Core_CreateRTC (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_RTC* rtc = TM32Core_CreateZero(1, TM32Core_RTC);
    if (rtc == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core RTC component");
        return NULL;
    }

    rtc->bus = bus;

    if (!TM32Core_InitializeRTC(rtc))
    {
        TM32Core_LogError("Could not initialize the TM32 Core RTC component");
        TM32Core_Destroy(rtc);
        return NULL;
    }

    return rtc;
}

bool TM32Core_InitializeRTC (
    TM32Core_RTC*   rtc
)
{
    TM32Core_ReturnValueIfNull(rtc, false);

    return true;
}

void TM32Core_DestroyRTC (
    TM32Core_RTC*   rtc
)
{
    if (rtc != NULL)
    {
        TM32Core_Destroy(rtc);
    }
}
