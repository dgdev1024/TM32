/**
 * @file    tm32core_bus.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implementation of the TM32 Core's Pixel Processing Unit (PPU)
 *          component structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>
#include <tm32core_ppu.h>

/* Private Structures *********************************************************/

struct TM32Core_PPU
{
    TM32Core_Bus*   bus;        /** @brief The TM32 Bus to which this PPU is connected. */
};

/* Public Functions ***********************************************************/

TM32Core_PPU* TM32Core_CreatePPU (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);

    TM32Core_PPU* ppu = TM32Core_CreateZero(1, TM32Core_PPU);
    if (ppu == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core PPU component");
        return NULL;
    }

    ppu->bus = bus;

    if (!TM32Core_InitializePPU(ppu))
    {
        TM32Core_LogError("Could not initialize the TM32 Core PPU component");
        TM32Core_Destroy(ppu);
        return NULL;
    }

    return ppu;
}

bool TM32Core_InitializePPU (
    TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);

    return true;
}

bool TM32Core_ClockPPU (
    TM32Core_PPU*   ppu
)
{
    TM32Core_ReturnValueIfNull(ppu, false);

    return true;
}

void TM32Core_DestroyPPU (
    TM32Core_PPU*   ppu
)
{
    if (ppu != NULL)
    {
        TM32Core_Destroy(ppu);
    }
}
