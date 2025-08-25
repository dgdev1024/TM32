/**
 * @file    tm32core_bus.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Timer component structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_timer.h>

/* Private Structures *********************************************************/

struct TM32Core_Timer
{
    TM32Core_Bus*   bus;        /** @brief The TM32 Bus to which this timer is connected. */
};

/* Public Functions ***********************************************************/

TM32Core_Timer* TM32Core_CreateTimer (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_Timer* timer = TM32Core_CreateZero(1, TM32Core_Timer);
    if (timer == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core timer component");
        return NULL;
    }

    timer->bus = bus;

    if (!TM32Core_InitializeTimer(timer))
    {
        TM32Core_LogError("Could not initialize the TM32 Core timer component");
        TM32Core_Destroy(timer);
        return NULL;
    }

    return timer;
}

bool TM32Core_InitializeTimer (
    TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, false);

    return true;
}

bool TM32Core_ClockTimer (
    TM32Core_Timer* timer
)
{
    TM32Core_ReturnValueIfNull(timer, false);

    return true;
}

void TM32Core_DestroyTimer (
    TM32Core_Timer* timer
)
{
    if (timer != NULL)
    {
        TM32Core_Destroy(timer);
    }
}
