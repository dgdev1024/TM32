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
    TM32Core_RAM*       ram;        /** @brief The TM32 RAM connected to this bus. */
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

    // Program ROM (metadata, interrupt vectors, code)
    if (address <= TM32CORE_PROGRAM_END)
    {
        // return TM32Core_ReadProgramROM(bus->program, address, data);
        return false;
    }

    // Work RAM
    else if (address >= TM32CORE_WORK_RAM_START && address <= TM32CORE_WORK_RAM_END)
    {
        // return TM32Core_ReadWorkRAM(bus->ram, address - TM32CORE_WORK_RAM_START, 
        //     data);
        return false;
    }

    // Save RAM
    else if (address >= TM32CORE_SAVE_RAM_START && address <= TM32CORE_SAVE_RAM_END)
    {
        // return TM32Core_ReadSaveRAM(bus->ram, address - TM32CORE_SAVE_RAM_START, 
        //     data);
        return false;
    }

    // Video RAM
    else if (address >= TM32CORE_VIDEO_RAM_START && address <= TM32CORE_VIDEO_RAM_END)
    {
        // return TM32Core_ReadVideoRAM(bus->ppu, 
        //     address - TM32CORE_VIDEO_RAM_START, data);
        return false;
    }

    // Object Attribute Memory (OAM)
    else if (address >= TM32CORE_OAM_START && address <= TM32CORE_OAM_END)
    {
        // return TM32Core_ReadOAM(bus->ppu, address - TM32CORE_OAM_START, data);
        return false;
    }

    // Stack Memory
    else if (address >= TM32CORE_STACK_START && address <= TM32CORE_STACK_END)
    {
        // return TM32Core_ReadStackRAM(bus->ram, address - TM32CORE_STACK_START, 
        //     data);
        return false;
    }

    // Quick RAM
    else if (address >= TM32CORE_QUICK_RAM_START && address <= TM32CORE_QUICK_RAM_END)
    {
        // return TM32Core_ReadQuickRAM(bus->ram, address - TM32CORE_QUICK_RAM_START, 
        //     data);
    }

    // I/O Ports
    switch (address)
    {
        // case 0xFFFFFF??:
        //     *data = TM32Core_ReadSomePort(bus->someComponent);
        //     break;
        default:
            TM32Core_LogError("Read from unmapped address 0x%08X", address);
            return false;
    }

    return true;
}

static bool TM32Core_BusWrite (uint32_t address, uint8_t data, void* userData)
{
    TM32Core_Bus* bus = (TM32Core_Bus*) userData;
    if (bus == NULL)
    {
        return false;
    }

    // Program ROM (metadata, interrupt vectors, code)
    if (address <= TM32CORE_PROGRAM_END)
    {
        // Not writable.
        return true;
    }

    // Work RAM
    else if (address >= TM32CORE_WORK_RAM_START && address <= TM32CORE_WORK_RAM_END)
    {
        // return TM32Core_WriteWorkRAM(bus->ram, address - TM32CORE_WORK_RAM_START, 
        //     data);
        return false;
    }

    // Save RAM
    else if (address >= TM32CORE_SAVE_RAM_START && address <= TM32CORE_SAVE_RAM_END)
    {
        // return TM32Core_WriteSaveRAM(bus->ram, address - TM32CORE_SAVE_RAM_START, 
        //     data);
        return false;
    }

    // Video RAM
    else if (address >= TM32CORE_VIDEO_RAM_START && address <= TM32CORE_VIDEO_RAM_END)
    {
        // return TM32Core_WriteVideoRAM(bus->ppu, 
        //     address - TM32CORE_VIDEO_RAM_START, data);
        return false;
    }

    // Object Attribute Memory (OAM)
    else if (address >= TM32CORE_OAM_START && address <= TM32CORE_OAM_END)
    {
        // return TM32Core_WriteOAM(bus->ppu, address - TM32CORE_OAM_START, data);
        return false;
    }

    // Stack Memory
    else if (address >= TM32CORE_STACK_START && address <= TM32CORE_STACK_END)
    {
        // return TM32Core_WriteStackRAM(bus->ram, address - TM32CORE_STACK_START, 
        //     data);
        return false;
    }

    // Quick RAM
    else if (address >= TM32CORE_QUICK_RAM_START && address <= TM32CORE_QUICK_RAM_END)
    {
        // return TM32Core_WriteQuickRAM(bus->ram, address - TM32CORE_QUICK_RAM_START, 
        //     data);
        return false;
    }

    // I/O Ports
    switch (address)
    {
        // case 0xFFFFFF??:
        //     TM32Core_WriteSomePort(bus->someComponent, data);
        //     break;
        default:
            TM32Core_LogError("Write to unmapped address 0x%08X", address);
            return false;
    }

    return true;
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

    bus->ram = TM32Core_CreateRAM();
    if (bus->ram == NULL)
    {
        TM32Core_LogErrno("Could not create the TM32 RAM for the bus");
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
        TM32Core_InitializeRAM(bus->ram) &&
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
        TM32Core_DestroyRAM(bus->ram);
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

TM32Core_RAM* TM32Core_GetBusRAM (
    TM32Core_Bus*   bus
)
{
    TM32Core_ReturnValueIfNull(bus, NULL);
    return bus->ram;
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
