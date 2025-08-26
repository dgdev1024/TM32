/**
 * @file    tm32core_bus.c
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
        return TM32Core_ReadProgramROM(bus->program, address, data);
    }

    // Work RAM
    else if (address >= TM32CORE_WORK_RAM_START && address <= TM32CORE_WORK_RAM_END)
    {
        return TM32Core_ReadRAMByte(bus->ram, address, data);
    }

    // Save RAM
    else if (address >= TM32CORE_SAVE_RAM_START && address <= TM32CORE_SAVE_RAM_END)
    {
        return TM32Core_ReadProgramSRAM(bus->program, address, data);
    }

    // Video RAM
    else if (address >= TM32CORE_VIDEO_RAM_START && address <= TM32CORE_VIDEO_RAM_END)
    {
        return TM32Core_ReadVRAM(bus->ppu, address - TM32CORE_VIDEO_RAM_START, data);
    }

    // Object Attribute Memory (OAM)
    else if (address >= TM32CORE_OAM_START && address <= TM32CORE_OAM_END)
    {
        return TM32Core_ReadOAM(bus->ppu, address - TM32CORE_OAM_START, data);
    }

    // Stack Memory
    else if (address >= TM32CORE_STACK_START && address <= TM32CORE_STACK_END)
    {
        return TM32Core_ReadRAMByte(bus->ram, address, data);
    }

    // Quick RAM
    else if (address >= TM32CORE_QUICK_RAM_START && address <= TM32CORE_QUICK_RAM_END)
    {
        return TM32Core_ReadRAMByte(bus->ram, address, data);
    }

    // I/O Ports
    switch (address)
    {
        case TM32CORE_PORT_JOYP:
            *data = TM32Core_ReadJOYP(bus->joypad);
            break;
        case TM32CORE_PORT_SB:
            *data = TM32Core_ReadSB(bus->serial);
            break;
        case TM32CORE_PORT_SC:
            *data = TM32Core_ReadSC(bus->serial);
            break;
        case TM32CORE_PORT_DIV:
            *data = TM32Core_ReadDIV(bus->timer);
            break;
        case TM32CORE_PORT_TIMA:
            *data = TM32Core_ReadTIMA(bus->timer);
            break;
        case TM32CORE_PORT_TMA:
            *data = TM32Core_ReadTMA(bus->timer);
            break;
        case TM32CORE_PORT_TAC:
            *data = TM32Core_ReadTAC(bus->timer);
            break;
            
        // APU Registers
        case TM32CORE_PORT_NR10:
            *data = TM32Core_ReadNR10(bus->apu);
            break;
        case TM32CORE_PORT_NR11:
            *data = TM32Core_ReadNR11(bus->apu);
            break;
        case TM32CORE_PORT_NR12:
            *data = TM32Core_ReadNR12(bus->apu);
            break;
        case TM32CORE_PORT_NR13:
            // NR13 is write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_NR14:
            *data = TM32Core_ReadNR14(bus->apu);
            break;
        case TM32CORE_PORT_NR21:
            *data = TM32Core_ReadNR21(bus->apu);
            break;
        case TM32CORE_PORT_NR22:
            *data = TM32Core_ReadNR22(bus->apu);
            break;
        case TM32CORE_PORT_NR23:
            // NR23 is write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_NR24:
            *data = TM32Core_ReadNR24(bus->apu);
            break;
        case TM32CORE_PORT_NR30:
            *data = TM32Core_ReadNR30(bus->apu);
            break;
        case TM32CORE_PORT_NR31:
            // NR31 is write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_NR32:
            *data = TM32Core_ReadNR32(bus->apu);
            break;
        case TM32CORE_PORT_NR33:
            // NR33 is write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_NR34:
            *data = TM32Core_ReadNR34(bus->apu);
            break;
        case TM32CORE_PORT_NR41:
            // NR41 is write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_NR42:
            *data = TM32Core_ReadNR42(bus->apu);
            break;
        case TM32CORE_PORT_NR43:
            *data = TM32Core_ReadNR43(bus->apu);
            break;
        case TM32CORE_PORT_NR44:
            *data = TM32Core_ReadNR44(bus->apu);
            break;
        case TM32CORE_PORT_NR50:
            *data = TM32Core_ReadNR50(bus->apu);
            break;
        case TM32CORE_PORT_NR51:
            *data = TM32Core_ReadNR51(bus->apu);
            break;
        case TM32CORE_PORT_NR52:
            *data = TM32Core_ReadNR52(bus->apu);
            break;
        case TM32CORE_PORT_PCM12:
            *data = TM32Core_ReadPCM12(bus->apu);
            break;
        case TM32CORE_PORT_PCM34:
            *data = TM32Core_ReadPCM34(bus->apu);
            break;
            
        // PPU Registers
        case TM32CORE_PORT_LCDC:
            *data = TM32Core_ReadLCDC(bus->ppu);
            break;
        case TM32CORE_PORT_STAT:
            *data = TM32Core_ReadSTAT(bus->ppu);
            break;
        case TM32CORE_PORT_SCY:
        case TM32CORE_PORT_SCX:
        case TM32CORE_PORT_WY:
        case TM32CORE_PORT_WX:
            *data = TM32Core_ReadScrollRegister(bus->ppu, address);
            break;
        case TM32CORE_PORT_LY:
            *data = TM32Core_ReadLY(bus->ppu);
            break;
        case TM32CORE_PORT_LYC:
            *data = TM32Core_ReadLYC(bus->ppu);
            break;
        case TM32CORE_PORT_DMA:
            // DMA register is write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_BGP:
        case TM32CORE_PORT_OBP0:
        case TM32CORE_PORT_OBP1:
            *data = TM32Core_ReadPaletteRegister(bus->ppu, address);
            break;
        case TM32CORE_PORT_VBK:
            *data = TM32Core_ReadVBK(bus->ppu);
            break;
        case TM32CORE_PORT_BGPI:
        case TM32CORE_PORT_BGPD:
        case TM32CORE_PORT_OBPI:
        case TM32CORE_PORT_OBPD:
            *data = TM32Core_ReadCGBPaletteRegister(bus->ppu, address);
            break;
        case TM32CORE_PORT_OPRI:
            *data = TM32Core_ReadOPRI(bus->ppu);
            break;
        case TM32CORE_PORT_GRPM:
            *data = TM32Core_ReadGRPM(bus->ppu);
            break;
            
        // OAM DMA Registers
        case TM32CORE_PORT_ODMAS3:
        case TM32CORE_PORT_ODMAS2:
        case TM32CORE_PORT_ODMAS1:
            // OAM DMA source registers are write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_ODMA:
            *data = TM32Core_ReadODMA(bus->ppu);
            break;
            
        // HDMA Registers
        case TM32CORE_PORT_HDMAS3:
        case TM32CORE_PORT_HDMAS2:
        case TM32CORE_PORT_HDMAS1:
        case TM32CORE_PORT_HDMAS0:
        case TM32CORE_PORT_HDMAD3:
        case TM32CORE_PORT_HDMAD2:
        case TM32CORE_PORT_HDMAD1:
        case TM32CORE_PORT_HDMAD0:
            // HDMA source and destination registers are write-only, reads return 0xFF
            *data = 0xFF;
            break;
        case TM32CORE_PORT_HDMA:
            *data = TM32Core_ReadHDMA(bus->ppu);
            break;
            
        // RTC Registers
        case TM32CORE_PORT_RTCS:
            *data = TM32Core_ReadRTCS(bus->rtc);
            break;
        case TM32CORE_PORT_RTCM:
            *data = TM32Core_ReadRTCM(bus->rtc);
            break;
        case TM32CORE_PORT_RTCH:
            *data = TM32Core_ReadRTCH(bus->rtc);
            break;
        case TM32CORE_PORT_RTCD1:
            *data = TM32Core_ReadRTCD1(bus->rtc);
            break;
        case TM32CORE_PORT_RTCD0:
            *data = TM32Core_ReadRTCD0(bus->rtc);
            break;
        case TM32CORE_PORT_RTCC:
            *data = TM32Core_ReadRTCC(bus->rtc);
            break;
        case TM32CORE_PORT_RTCL:
            // RTCL is write-only, reads return 0xFF
            *data = 0xFF;
            break;
            
        // Interrupt Registers
        case TM32CORE_PORT_IF3:
            *data = (TM32CPU_GetInterruptFlags(bus->cpu) >> 24) & 0xFF;
            break;
        case TM32CORE_PORT_IF2:
            *data = (TM32CPU_GetInterruptFlags(bus->cpu) >> 16) & 0xFF;
            break;
        case TM32CORE_PORT_IF1:
            *data = (TM32CPU_GetInterruptFlags(bus->cpu) >> 8) & 0xFF;
            break;
        case TM32CORE_PORT_IF0:
            *data = TM32CPU_GetInterruptFlags(bus->cpu) & 0xFF;
            break;
        case TM32CORE_PORT_IE3:
            *data = (TM32CPU_GetInterruptEnable(bus->cpu) >> 24) & 0xFF;
            break;
        case TM32CORE_PORT_IE2:
            *data = (TM32CPU_GetInterruptEnable(bus->cpu) >> 16) & 0xFF;
            break;
        case TM32CORE_PORT_IE1:
            *data = (TM32CPU_GetInterruptEnable(bus->cpu) >> 8) & 0xFF;
            break;
        case TM32CORE_PORT_IE0:
            *data = TM32CPU_GetInterruptEnable(bus->cpu) & 0xFF;
            break;
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
        return TM32Core_WriteRAMByte(bus->ram, address, data);
    }

    // Save RAM
    else if (address >= TM32CORE_SAVE_RAM_START && address <= TM32CORE_SAVE_RAM_END)
    {
        return TM32Core_WriteProgramSRAM(bus->program, address, data);
    }

    // Video RAM
    else if (address >= TM32CORE_VIDEO_RAM_START && address <= TM32CORE_VIDEO_RAM_END)
    {
        return TM32Core_WriteVRAM(bus->ppu, address - TM32CORE_VIDEO_RAM_START, data);
    }

    // Object Attribute Memory (OAM)
    else if (address >= TM32CORE_OAM_START && address <= TM32CORE_OAM_END)
    {
        return TM32Core_WriteOAM(bus->ppu, address - TM32CORE_OAM_START, data);
    }

    // Stack Memory
    else if (address >= TM32CORE_STACK_START && address <= TM32CORE_STACK_END)
    {
        return TM32Core_WriteRAMByte(bus->ram, address, data);
    }

    // Quick RAM
    else if (address >= TM32CORE_QUICK_RAM_START && address <= TM32CORE_QUICK_RAM_END)
    {
        return TM32Core_WriteRAMByte(bus->ram, address, data);
    }

    // I/O Ports
    switch (address)
    {
        case TM32CORE_PORT_JOYP:
            return TM32Core_WriteJOYP(bus->joypad, data);
        case TM32CORE_PORT_SB:
            return TM32Core_WriteSB(bus->serial, data);
        case TM32CORE_PORT_SC:
            return TM32Core_WriteSC(bus->serial, data);
        case TM32CORE_PORT_DIV:
            return TM32Core_WriteDIV(bus->timer, data);
        case TM32CORE_PORT_TIMA:
            return TM32Core_WriteTIMA(bus->timer, data);
        case TM32CORE_PORT_TMA:
            return TM32Core_WriteTMA(bus->timer, data);
        case TM32CORE_PORT_TAC:
            return TM32Core_WriteTAC(bus->timer, data);
            
        // APU Registers
        case TM32CORE_PORT_NR10:
            return TM32Core_WriteNR10(bus->apu, data);
        case TM32CORE_PORT_NR11:
            return TM32Core_WriteNR11(bus->apu, data);
        case TM32CORE_PORT_NR12:
            return TM32Core_WriteNR12(bus->apu, data);
        case TM32CORE_PORT_NR13:
            return TM32Core_WriteNR13(bus->apu, data);
        case TM32CORE_PORT_NR14:
            return TM32Core_WriteNR14(bus->apu, data);
        case TM32CORE_PORT_NR21:
            return TM32Core_WriteNR21(bus->apu, data);
        case TM32CORE_PORT_NR22:
            return TM32Core_WriteNR22(bus->apu, data);
        case TM32CORE_PORT_NR23:
            return TM32Core_WriteNR23(bus->apu, data);
        case TM32CORE_PORT_NR24:
            return TM32Core_WriteNR24(bus->apu, data);
        case TM32CORE_PORT_NR30:
            return TM32Core_WriteNR30(bus->apu, data);
        case TM32CORE_PORT_NR31:
            return TM32Core_WriteNR31(bus->apu, data);
        case TM32CORE_PORT_NR32:
            return TM32Core_WriteNR32(bus->apu, data);
        case TM32CORE_PORT_NR33:
            return TM32Core_WriteNR33(bus->apu, data);
        case TM32CORE_PORT_NR34:
            return TM32Core_WriteNR34(bus->apu, data);
        case TM32CORE_PORT_NR41:
            return TM32Core_WriteNR41(bus->apu, data);
        case TM32CORE_PORT_NR42:
            return TM32Core_WriteNR42(bus->apu, data);
        case TM32CORE_PORT_NR43:
            return TM32Core_WriteNR43(bus->apu, data);
        case TM32CORE_PORT_NR44:
            return TM32Core_WriteNR44(bus->apu, data);
        case TM32CORE_PORT_NR50:
            return TM32Core_WriteNR50(bus->apu, data);
        case TM32CORE_PORT_NR51:
            return TM32Core_WriteNR51(bus->apu, data);
        case TM32CORE_PORT_NR52:
            return TM32Core_WriteNR52(bus->apu, data);
        // PCM12 and PCM34 are read-only registers
            
        // PPU Registers
        case TM32CORE_PORT_LCDC:
            return TM32Core_WriteLCDC(bus->ppu, data);
        case TM32CORE_PORT_STAT:
            return TM32Core_WriteSTAT(bus->ppu, data);
        case TM32CORE_PORT_SCY:
        case TM32CORE_PORT_SCX:
        case TM32CORE_PORT_WY:
        case TM32CORE_PORT_WX:
            return TM32Core_WriteScrollRegister(bus->ppu, address, data);
        case TM32CORE_PORT_LYC:
            return TM32Core_WriteLYC(bus->ppu, data);
        case TM32CORE_PORT_DMA:
            // DMA register is an alias for ODMA 
            return TM32Core_WriteODMA(bus->ppu, data);
        case TM32CORE_PORT_BGP:
        case TM32CORE_PORT_OBP0:
        case TM32CORE_PORT_OBP1:
            return TM32Core_WritePaletteRegister(bus->ppu, address, data);
        case TM32CORE_PORT_VBK:
            return TM32Core_WriteVBK(bus->ppu, data);
        case TM32CORE_PORT_BGPI:
        case TM32CORE_PORT_BGPD:
        case TM32CORE_PORT_OBPI:
        case TM32CORE_PORT_OBPD:
            return TM32Core_WriteCGBPaletteRegister(bus->ppu, address, data);
        case TM32CORE_PORT_OPRI:
            return TM32Core_WriteOPRI(bus->ppu, data);
        case TM32CORE_PORT_GRPM:
            return TM32Core_WriteGRPM(bus->ppu, data);
            
        // OAM DMA Registers
        case TM32CORE_PORT_ODMAS3:
            return TM32Core_WriteODMASource(bus->ppu, 3, data);
        case TM32CORE_PORT_ODMAS2:
            return TM32Core_WriteODMASource(bus->ppu, 2, data);
        case TM32CORE_PORT_ODMAS1:
            return TM32Core_WriteODMASource(bus->ppu, 1, data);
        case TM32CORE_PORT_ODMA:
            return TM32Core_WriteODMA(bus->ppu, data);
            
        // HDMA Registers
        case TM32CORE_PORT_HDMAS3:
            return TM32Core_WriteHDMASource(bus->ppu, 3, data);
        case TM32CORE_PORT_HDMAS2:
            return TM32Core_WriteHDMASource(bus->ppu, 2, data);
        case TM32CORE_PORT_HDMAS1:
            return TM32Core_WriteHDMASource(bus->ppu, 1, data);
        case TM32CORE_PORT_HDMAS0:
            return TM32Core_WriteHDMASource(bus->ppu, 0, data);
        case TM32CORE_PORT_HDMAD3:
            return TM32Core_WriteHDMADestination(bus->ppu, 3, data);
        case TM32CORE_PORT_HDMAD2:
            return TM32Core_WriteHDMADestination(bus->ppu, 2, data);
        case TM32CORE_PORT_HDMAD1:
            return TM32Core_WriteHDMADestination(bus->ppu, 1, data);
        case TM32CORE_PORT_HDMAD0:
            return TM32Core_WriteHDMADestination(bus->ppu, 0, data);
        case TM32CORE_PORT_HDMA:
            return TM32Core_WriteHDMA(bus->ppu, data);
            
        // RTC Registers
        case TM32CORE_PORT_RTCS:
        case TM32CORE_PORT_RTCM:
        case TM32CORE_PORT_RTCH:
        case TM32CORE_PORT_RTCD1:
        case TM32CORE_PORT_RTCD0:
            // RTC time registers are read-only, writes are ignored
            break;
        case TM32CORE_PORT_RTCC:
            TM32Core_WriteRTCC(bus->rtc, data);
            break;
        case TM32CORE_PORT_RTCL:
            TM32Core_WriteRTCL(bus->rtc, data);
            break;
            
        // Interrupt Registers
        case TM32CORE_PORT_IF3:
            TM32CPU_SetInterruptFlags(bus->cpu,
                (TM32CPU_GetInterruptFlags(bus->cpu) & 0x00FFFFFF) | (data << 24));
            break;
        case TM32CORE_PORT_IF2:
            TM32CPU_SetInterruptFlags(bus->cpu,
                (TM32CPU_GetInterruptFlags(bus->cpu) & 0xFF00FFFF) | (data << 16));
            break;
        case TM32CORE_PORT_IF1:
            TM32CPU_SetInterruptFlags(bus->cpu,
                (TM32CPU_GetInterruptFlags(bus->cpu) & 0xFFFF00FF) | (data << 8));
            break;
        case TM32CORE_PORT_IF0:
            TM32CPU_SetInterruptFlags(bus->cpu,
                (TM32CPU_GetInterruptFlags(bus->cpu) & 0xFFFFFF00) | data);
            break;
        case TM32CORE_PORT_IE3:
            TM32CPU_SetInterruptEnable(bus->cpu,
                (TM32CPU_GetInterruptEnable(bus->cpu) & 0x00FFFFFF) | (data << 24));
            break;
        case TM32CORE_PORT_IE2:
            TM32CPU_SetInterruptEnable(bus->cpu,
                (TM32CPU_GetInterruptEnable(bus->cpu) & 0xFF00FFFF) | (data << 16));
            break;
        case TM32CORE_PORT_IE1:
            TM32CPU_SetInterruptEnable(bus->cpu,
                (TM32CPU_GetInterruptEnable(bus->cpu) & 0xFFFF00FF) | (data << 8));
            break;
        case TM32CORE_PORT_IE0:
            TM32CPU_SetInterruptEnable(bus->cpu,
                (TM32CPU_GetInterruptEnable(bus->cpu) & 0xFFFFFF00) | data);
            break;
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
        TM32Core_InitializeRAM(bus->ram, 0) &&  // Initialize with no WRAM for now
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

bool TM32Core_ReadBusData (
    TM32Core_Bus*   bus,
    uint32_t        address,
    uint8_t*        data
)
{
    TM32Core_ReturnValueIfNull(bus, false);
    TM32Core_ReturnValueIfNull(data, false);
    
    return TM32Core_BusRead(address, data, bus);
}

bool TM32Core_WriteBusData (
    TM32Core_Bus*   bus,
    uint32_t        address,
    uint8_t         data
)
{
    TM32Core_ReturnValueIfNull(bus, false);
    
    return TM32Core_BusWrite(address, data, bus);
}
