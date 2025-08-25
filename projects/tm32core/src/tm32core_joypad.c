/**
 * @file    tm32core_joypad.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Joypad component structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_joypad.h>

/* Private Structures *********************************************************/

struct TM32Core_Joypad
{
    TM32Core_Bus*   bus;        /** @brief The TM32 Bus to which this joypad is connected. */
};

/* Public Functions ***********************************************************/

TM32Core_Joypad* TM32Core_CreateJoypad (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_Joypad* joypad = TM32Core_CreateZero(1, TM32Core_Joypad);
    if (joypad == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core joypad component");
        return NULL;
    }

    joypad->bus = bus;

    if (!TM32Core_InitializeJoypad(joypad))
    {
        TM32Core_LogError("Could not initialize the TM32 Core joypad component");
        TM32Core_Destroy(joypad);
        return NULL;
    }

    return joypad;
}

bool TM32Core_InitializeJoypad (
    TM32Core_Joypad* joypad
)
{
    TM32Core_ReturnValueIfNull(joypad, false);

    return true;
}

void TM32Core_DestroyJoypad (
    TM32Core_Joypad* joypad
)
{
    if (joypad != NULL)
    {
        TM32Core_Destroy(joypad);
    }
}
