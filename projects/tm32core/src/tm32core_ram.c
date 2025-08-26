/**
 * @file    tm32core_ram.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implementation of the TM32 Core RAM module.
 */

/* Include Files **************************************************************/

#include <tm32core_ram.h>

/* Private Structures *********************************************************/

struct TM32Core_RAM
{
    // Working RAM (WRAM) - Allocated based on program requirements
    uint8_t*        wram;           /** @brief Working RAM buffer */
    uint32_t        wramSize;       /** @brief Allocated WRAM size */
    
    // Stack Memory - Fixed 64KB
    uint8_t         stack[TM32CORE_STACK_SIZE];     /** @brief Stack memory buffer */
    
    // Quick RAM (QRAM) - Fixed 65280 bytes (64KB minus port registers)
    uint8_t         qram[TM32CORE_QRAM_SIZE];       /** @brief Quick RAM buffer */
    
    // Initialization state
    bool            initialized;    /** @brief Whether RAM is initialized */
};

/* Private Function Prototypes ************************************************/

/**
 * @brief   Determines which RAM region contains the given address.
 * 
 * @param   address     The memory address to check.
 * 
 * @return  0 for WRAM, 1 for Stack, 2 for QRAM, -1 for invalid address.
 */
static int TM32Core_GetRAMRegion (uint32_t address);

/**
 * @brief   Calculates the offset within a RAM region for the given address.
 * 
 * @param   address     The memory address.
 * @param   region      The RAM region (0=WRAM, 1=Stack, 2=QRAM).
 * 
 * @return  The offset within the region.
 */
static uint32_t TM32Core_GetRAMOffset (uint32_t address, int region);

/* Private Functions **********************************************************/

static int TM32Core_GetRAMRegion (uint32_t address)
{
    if (address >= TM32CORE_WRAM_BASE_ADDRESS && address <= TM32CORE_WRAM_END_ADDRESS)
    {
        return 0; // WRAM
    }
    else if (address >= TM32CORE_STACK_BASE_ADDRESS && address <= TM32CORE_STACK_END_ADDRESS)
    {
        return 1; // Stack
    }
    else if (address >= TM32CORE_QRAM_BASE_ADDRESS && address <= TM32CORE_QRAM_END_ADDRESS)
    {
        return 2; // QRAM
    }
    
    return -1; // Invalid address
}

static uint32_t TM32Core_GetRAMOffset (uint32_t address, int region)
{
    switch (region)
    {
    case 0: // WRAM
        return address - TM32CORE_WRAM_BASE_ADDRESS;
        
    case 1: // Stack
        return address - TM32CORE_STACK_BASE_ADDRESS;
        
    case 2: // QRAM
        return address - TM32CORE_QRAM_BASE_ADDRESS;
        
    default:
        return 0;
    }
}

/* Public Functions ***********************************************************/

TM32Core_RAM* TM32Core_CreateRAM ()
{
    TM32Core_RAM* ram = TM32Core_CreateZero(1, TM32Core_RAM);
    if (ram == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core RAM module");
        return NULL;
    }

    // Initialize all fields to safe defaults
    ram->wram = NULL;
    ram->wramSize = 0;
    ram->initialized = false;
    
    TM32Core_LogInfo("Created TM32 Core RAM module");
    return ram;
}

bool TM32Core_InitializeRAM (
    TM32Core_RAM*   ram,
    uint32_t        wramSize
)
{
    TM32Core_ReturnValueIfNull(ram, false);
    
    if (ram->initialized)
    {
        TM32Core_LogError("RAM module is already initialized");
        return false;
    }
    
    // Validate WRAM size
    if (wramSize > TM32CORE_WRAM_MAX_SIZE)
    {
        TM32Core_LogError("Requested WRAM size 0x%08X exceeds maximum 0x%08X", 
                          wramSize, TM32CORE_WRAM_MAX_SIZE);
        return false;
    }
    
    // Allocate Working RAM if requested
    if (wramSize > 0)
    {
        ram->wram = TM32Core_CreateZero(wramSize, uint8_t);
        if (ram->wram == NULL)
        {
            TM32Core_LogErrno("Could not allocate Working RAM (%u bytes)", wramSize);
            return false;
        }
        
        ram->wramSize = wramSize;
        TM32Core_LogInfo("Allocated Working RAM: %u bytes (0x%08X)", wramSize, wramSize);
    }
    else
    {
        ram->wram = NULL;
        ram->wramSize = 0;
        TM32Core_LogInfo("No Working RAM allocated (size = 0)");
    }
    
    // Clear stack memory
    memset(ram->stack, 0, TM32CORE_STACK_SIZE);
    TM32Core_LogInfo("Initialized Stack Memory: %u bytes", TM32CORE_STACK_SIZE);
    
    // Clear quick RAM
    memset(ram->qram, 0, TM32CORE_QRAM_SIZE);
    TM32Core_LogInfo("Initialized Quick RAM: %u bytes", TM32CORE_QRAM_SIZE);
    
    ram->initialized = true;
    TM32Core_LogInfo("TM32 Core RAM module initialized successfully");
    
    return true;
}

bool TM32Core_ReadRAMByte (
    TM32Core_RAM*   ram,
    uint32_t        address,
    uint8_t*        data
)
{
    TM32Core_ReturnValueIfNull(ram, false);
    TM32Core_ReturnValueIfNull(data, false);
    
    if (!ram->initialized)
    {
        TM32Core_LogError("RAM module not initialized");
        return false;
    }
    
    int region = TM32Core_GetRAMRegion(address);
    if (region == -1)
    {
        TM32Core_LogError("Invalid RAM address: 0x%08X", address);
        return false;
    }
    
    uint32_t offset = TM32Core_GetRAMOffset(address, region);
    
    switch (region)
    {
    case 0: // WRAM
        if (ram->wram == NULL)
        {
            TM32Core_LogError("Working RAM not allocated");
            return false;
        }
        
        if (offset >= ram->wramSize)
        {
            TM32Core_LogError("WRAM offset 0x%08X exceeds allocated size 0x%08X", 
                              offset, ram->wramSize);
            return false;
        }
        
        *data = ram->wram[offset];
        break;
        
    case 1: // Stack
        *data = ram->stack[offset];
        break;
        
    case 2: // QRAM
        *data = ram->qram[offset];
        break;
        
    default:
        TM32Core_LogError("Unknown RAM region: %d", region);
        return false;
    }
    
    return true;
}

bool TM32Core_WriteRAMByte (
    TM32Core_RAM*   ram,
    uint32_t        address,
    uint8_t         data
)
{
    TM32Core_ReturnValueIfNull(ram, false);
    
    if (!ram->initialized)
    {
        TM32Core_LogError("RAM module not initialized");
        return false;
    }
    
    int region = TM32Core_GetRAMRegion(address);
    if (region == -1)
    {
        TM32Core_LogError("Invalid RAM address: 0x%08X", address);
        return false;
    }
    
    uint32_t offset = TM32Core_GetRAMOffset(address, region);
    
    switch (region)
    {
    case 0: // WRAM
        if (ram->wram == NULL)
        {
            TM32Core_LogError("Working RAM not allocated");
            return false;
        }
        
        if (offset >= ram->wramSize)
        {
            TM32Core_LogError("WRAM offset 0x%08X exceeds allocated size 0x%08X", 
                              offset, ram->wramSize);
            return false;
        }
        
        ram->wram[offset] = data;
        break;
        
    case 1: // Stack
        ram->stack[offset] = data;
        break;
        
    case 2: // QRAM
        ram->qram[offset] = data;
        break;
        
    default:
        TM32Core_LogError("Unknown RAM region: %d", region);
        return false;
    }
    
    return true;
}

bool TM32Core_ClearRAM (
    TM32Core_RAM*   ram
)
{
    TM32Core_ReturnValueIfNull(ram, false);
    
    if (!ram->initialized)
    {
        TM32Core_LogError("RAM module not initialized");
        return false;
    }
    
    // Clear WRAM if allocated
    if (ram->wram != NULL && ram->wramSize > 0)
    {
        memset(ram->wram, 0, ram->wramSize);
        TM32Core_LogInfo("Cleared Working RAM (%u bytes)", ram->wramSize);
    }
    
    // Clear stack memory
    memset(ram->stack, 0, TM32CORE_STACK_SIZE);
    TM32Core_LogInfo("Cleared Stack Memory (%u bytes)", TM32CORE_STACK_SIZE);
    
    // Clear quick RAM
    memset(ram->qram, 0, TM32CORE_QRAM_SIZE);
    TM32Core_LogInfo("Cleared Quick RAM (%u bytes)", TM32CORE_QRAM_SIZE);
    
    return true;
}

bool TM32Core_ReinitializeRAM (
    TM32Core_RAM*   ram,
    uint32_t        wramSize
)
{
    TM32Core_ReturnValueIfNull(ram, false);
    
    if (!ram->initialized)
    {
        TM32Core_LogError("RAM module not initialized");
        return false;
    }
    
    // Validate new WRAM size
    if (wramSize > TM32CORE_WRAM_MAX_SIZE)
    {
        TM32Core_LogError("Requested WRAM size 0x%08X exceeds maximum 0x%08X", 
                          wramSize, TM32CORE_WRAM_MAX_SIZE);
        return false;
    }
    
    // Free existing WRAM if any
    if (ram->wram != NULL)
    {
        TM32Core_LogInfo("Freeing existing Working RAM (%u bytes)", ram->wramSize);
        TM32Core_Destroy(ram->wram);
        ram->wram = NULL;
        ram->wramSize = 0;
    }
    
    // Allocate new WRAM if requested
    if (wramSize > 0)
    {
        ram->wram = TM32Core_CreateZero(wramSize, uint8_t);
        if (ram->wram == NULL)
        {
            TM32Core_LogErrno("Could not allocate new Working RAM (%u bytes)", wramSize);
            return false;
        }
        
        ram->wramSize = wramSize;
        TM32Core_LogInfo("Reallocated Working RAM: %u bytes (0x%08X)", wramSize, wramSize);
    }
    else
    {
        TM32Core_LogInfo("Working RAM deallocated (size = 0)");
    }
    
    // Clear stack and QRAM during reinitialization
    memset(ram->stack, 0, TM32CORE_STACK_SIZE);
    memset(ram->qram, 0, TM32CORE_QRAM_SIZE);
    TM32Core_LogInfo("Cleared Stack and Quick RAM during reinitialization");
    
    return true;
}

uint32_t TM32Core_GetWRAMSize (
    TM32Core_RAM*   ram
)
{
    if (ram == NULL || !ram->initialized)
    {
        return 0;
    }
    
    return ram->wramSize;
}

bool TM32Core_IsRAMInitialized (
    TM32Core_RAM*   ram
)
{
    return (ram != NULL && ram->initialized);
}

void TM32Core_DestroyRAM (
    TM32Core_RAM*  ram
)
{
    if (ram != NULL)
    {
        // Free Working RAM if allocated
        if (ram->wram != NULL)
        {
            TM32Core_LogInfo("Freeing Working RAM (%u bytes)", ram->wramSize);
            TM32Core_Destroy(ram->wram);
            ram->wram = NULL;
            ram->wramSize = 0;
        }
        
        // Clear initialization state
        ram->initialized = false;
        
        // Free the RAM structure itself
        TM32Core_LogInfo("Destroyed TM32 Core RAM module");
        TM32Core_Destroy(ram);
    }
}
