/**
 * @file    tm32core_apu.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implements the TM32 Core Audio Processing Unit (APU) component
 *          structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h> 
#include <tm32core_apu.h>

/* Private Structures *********************************************************/

struct TM32Core_APU
{
    TM32Core_Bus*   bus;        /** @brief The TM32 Bus to which this APU is connected. */
};

/* Public Functions ***********************************************************/

TM32Core_APU* TM32Core_CreateAPU (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_APU* apu = TM32Core_CreateZero(1, TM32Core_APU);
    if (apu == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core APU component");
        return NULL;
    }

    apu->bus = bus;

    if (!TM32Core_InitializeAPU(apu))
    {
        TM32Core_LogError("Could not initialize the TM32 Core APU component");
        TM32Core_Destroy(apu);
        return NULL;
    }

    return apu;
}

bool TM32Core_InitializeAPU (
    TM32Core_APU*   apu
)
{
    TM32Core_ReturnValueIfNull(apu, false);

    return true;
}

bool TM32Core_ClockAPU (
    TM32Core_APU*   apu
)
{
    TM32Core_ReturnValueIfNull(apu, false);

    return true;
}

void TM32Core_DestroyAPU (
    TM32Core_APU*   apu
)
{
    if (apu != NULL)
    {
        TM32Core_Destroy(apu);
    }
}
