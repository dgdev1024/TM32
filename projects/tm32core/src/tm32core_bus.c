/**
 * @file    tm32core_bus.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implementation of the TM32 Core Bus interface structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_bus.h>

/* Private Structures *********************************************************/

struct TM32Core_Bus
{
    TM32CPU_Processor*  cpu;        /** @brief The TM32 CPU connected to this bus. */
    TM32Core_Program*   program;    /** @brief The TM32 Core program connected to this bus. */
    TM32Core_Timer*     timer;      /** @brief The TM32 Timer connected to this bus. */
    TM32Core_RTC*       rtc;        /** @brief The TM32 Real-Time Clock connected to this bus. */
    TM32Core_Serial*    serial;     /** @brief The TM32 Serial interface connected to this bus. */
    TM32Core_Joypad*    joypad;     /** @brief The TM32 Joypad interface connected to this bus. */
    TM32Core_APU*       apu;        /** @brief The TM32 Audio Processing Unit connected to this bus. */
    TM32Core_PPU*       ppu;        /** @brief The TM32 Picture Processing Unit connected to this bus. */
};

/* Private Function Prototypes ************************************************/

static bool TM32Core_BusRead (uint32_t address, uint8_t* data, void* userData);
static bool TM32Core_BusWrite (uint32_t address, uint8_t data, void* userData);
static bool TM32Core_ClockCallback (uint32_t cycles, void* userData);

/* Private Functions **********************************************************/

static bool TM32Core_BusRead (uint32_t address, uint8_t* data, void* userData)
{
    TM32Core_Bus* bus = (TM32Core_Bus*) userData;
    if (bus == NULL || data == NULL)
    {
        return false;
    }

    // Implement the read logic here
    return false;
}

static bool TM32Core_BusWrite (uint32_t address, uint8_t data, void* userData)
{
    TM32Core_Bus* bus = (TM32Core_Bus*) userData;
    if (bus == NULL)
    {
        return false;
    }

    // Implement the write logic here
    return false;
}

static bool TM32Core_ClockCallback (uint32_t cycles, void* userData)
{
    TM32Core_Bus* bus = (TM32Core_Bus*) userData;
    
    for (uint32_t i = 0; i < cycles; ++i)
    {
        if (
            TM32Core_ClockTimer(bus->timer) == false ||
            TM32Core_ClockSerial(bus->serial) == false ||
            TM32Core_ClockAPU(bus->apu) == false ||
            TM32Core_ClockPPU(bus->ppu) == false
        )
        {
            return false;
        }
    }
    
    return true;
}

/* Public Functions ***********************************************************/

TM32Core_Bus* TM32Core_CreateBus ()
{
    TM32Core_Bus* bus = TM32Core_CreateZero(1, TM32Core_Bus);
    if (bus == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core bus interface");
        return NULL;
    }

    bus->cpu = TM32CPU_CreateProcessor(
        TM32Core_BusRead,
        TM32Core_BusWrite,
        TM32Core_ClockCallback,
        bus
    );
    if (bus->cpu == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 CPU for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    bus->program = TM32Core_CreateProgram();
    if (bus->program == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 Core program interface for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    bus->timer = TM32Core_CreateTimer(bus);
    if (bus->timer == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 Timer for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    bus->rtc = TM32Core_CreateRTC(bus);
    if (bus->rtc == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 RTC for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    bus->serial = TM32Core_CreateSerial(bus);
    if (bus->serial == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 Serial interface for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    bus->joypad = TM32Core_CreateJoypad(bus);
    if (bus->joypad == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 Joypad interface for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    bus->apu = TM32Core_CreateAPU(bus);
    if (bus->apu == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 APU for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    bus->ppu = TM32Core_CreatePPU(bus);
    if (bus->ppu == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 PPU for the bus");
        TM32Core_Destroy(bus);
        return NULL;
    }

    TM32Core_InitializeBus(bus);
    return bus;
}

bool TM32Core_InitializeBus (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, false);

    return
        TM32CPU_InitializeProcessor(bus->cpu) &&
        TM32Core_InitializeTimer(bus->timer) &&
        TM32Core_InitializeRTC(bus->rtc) &&
        TM32Core_InitializeSerial(bus->serial) &&
        TM32Core_InitializeJoypad(bus->joypad) &&
        TM32Core_InitializeAPU(bus->apu) &&
        TM32Core_InitializePPU(bus->ppu);
}

void TM32Core_DestroyBus (
    TM32Core_Bus*   bus
)
{
    if (bus != NULL)
    {
        TM32CPU_DestroyProcessor(bus->cpu);
        TM32Core_DestroyProgram(bus->program);
        TM32Core_DestroyTimer(bus->timer);
        TM32Core_DestroyRTC(bus->rtc);
        TM32Core_DestroySerial(bus->serial);
        TM32Core_DestroyJoypad(bus->joypad);
        TM32Core_DestroyAPU(bus->apu);
        TM32Core_DestroyPPU(bus->ppu);
        TM32Core_Destroy(bus);
    }
}

TM32CPU_Processor* TM32Core_GetBusCPU (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->cpu;
}

TM32Core_Program* TM32Core_GetBusProgramInterface (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->program;
}

TM32Core_Timer* TM32Core_GetBusTimer (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->timer;
}

TM32Core_RTC* TM32Core_GetBusRTC (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->rtc;
}

TM32Core_Serial* TM32Core_GetBusSerial (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->serial;
}

TM32Core_Joypad* TM32Core_GetBusJoypad (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->joypad;
}

TM32Core_APU* TM32Core_GetBusAPU (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->apu;
}

TM32Core_PPU* TM32Core_GetBusPPU (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->ppu;
}
