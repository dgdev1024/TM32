/**
 * @file    tm32core_program.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implementation of the TM32 Core ROM program structure and functions.
 */

/* Include Files **************************************************************/

#include <stdio.h>
#include <string.h>

#include <tm32core_program.h>

/* Private Constants **********************************************************/

#define TM32CORE_MAGIC_STRING               "TM32CORE"
#define TM32CORE_MIN_ROM_SIZE               0x00004000      // 16 KB minimum
#define TM32CORE_MAX_ROM_SIZE               0x80000000      // 2 GB maximum  
#define TM32CORE_MIN_WRAM_SIZE              0x00001000      // 4 KB minimum
#define TM32CORE_MAX_WRAM_SIZE              0x20000000      // 512 MB maximum
#define TM32CORE_MIN_SRAM_SIZE              0x00001000      // 4 KB minimum
#define TM32CORE_MAX_SRAM_SIZE              0x20000000      // 512 MB maximum

/* Private Structures *********************************************************/

/**
 * @brief   The TM32 Core Program structure.
 */
struct TM32Core_Program
{
    TM32Core_ProgramHeader  header;         /** @brief Program header and metadata. */
    uint8_t*                romData;        /** @brief Program ROM data buffer. */
    uint32_t                romSize;        /** @brief Actual ROM data size. */
    uint8_t*                sramData;       /** @brief Program SRAM data buffer. */
    uint32_t                sramSize;       /** @brief Allocated SRAM size. */
    bool                    isLoaded;       /** @brief Whether program is loaded. */
};

/* Private Function Prototypes ************************************************/

/**
 * @brief   Validates a program header for correctness.
 * 
 * @param   header      A pointer to the program header to validate.
 * 
 * @return  `true` if valid; `false` otherwise.
 */
static bool TM32Core_ValidateProgramHeader (
    const TM32Core_ProgramHeader*   header
);

/**
 * @brief   Allocates SRAM for the program based on header requirements.
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * 
 * @return  `true` on success; `false` on failure.
 */
static bool TM32Core_AllocateProgramSRAM (
    TM32Core_Program*   program
);

/**
 * @brief   Deallocates program memory (ROM and SRAM).
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 */
static void TM32Core_DeallocateProgramMemory (
    TM32Core_Program*   program
);

/* Private Functions **********************************************************/

static bool TM32Core_ValidateProgramHeader (
    const TM32Core_ProgramHeader*   header
)
{
    TM32Core_ReturnValueIfNull(header, false);

    // Check magic string
    if (memcmp(header->magic, TM32CORE_MAGIC_STRING, 8) != 0)
    {
        TM32Core_LogError("Invalid magic string in program header");
        return false;
    }
    
    // Validate ROM size
    if (header->romSize < TM32CORE_MIN_ROM_SIZE || header->romSize > TM32CORE_MAX_ROM_SIZE)
    {
        TM32Core_LogError("Invalid ROM size: 0x%08X (must be between 0x%08X and 0x%08X)",
                         header->romSize, TM32CORE_MIN_ROM_SIZE, TM32CORE_MAX_ROM_SIZE);
        return false;
    }
    
    // Validate WRAM size
    if (header->wramSize < TM32CORE_MIN_WRAM_SIZE || header->wramSize > TM32CORE_MAX_WRAM_SIZE)
    {
        TM32Core_LogError("Invalid WRAM size: 0x%08X (must be between 0x%08X and 0x%08X)",
                         header->wramSize, TM32CORE_MIN_WRAM_SIZE, TM32CORE_MAX_WRAM_SIZE);
        return false;
    }
    
    // Validate SRAM size (0 is allowed for no SRAM)
    if (header->sramSize > 0 && 
        (header->sramSize < TM32CORE_MIN_SRAM_SIZE || header->sramSize > TM32CORE_MAX_SRAM_SIZE))
    {
        TM32Core_LogError("Invalid SRAM size: 0x%08X (must be 0 or between 0x%08X and 0x%08X)",
                         header->sramSize, TM32CORE_MIN_SRAM_SIZE, TM32CORE_MAX_SRAM_SIZE);
        return false;
    }

    return true;
}

static bool TM32Core_AllocateProgramSRAM (
    TM32Core_Program*   program
)
{
    TM32Core_ReturnValueIfNull(program, false);
    
    // Deallocate existing SRAM if any
    if (program->sramData != NULL)
    {
        TM32Core_Destroy(program->sramData);
        program->sramData = NULL;
        program->sramSize = 0;
    }
    
    // Check if SRAM is requested
    if (program->header.sramSize == 0)
    {
        return true; // No SRAM requested, success
    }
    
    // Allocate SRAM
    program->sramData = TM32Core_CreateZero(program->header.sramSize, uint8_t);
    if (program->sramData == NULL)
    {
        TM32Core_LogErrno("Could not allocate SRAM of size 0x%08X", program->header.sramSize);
        return false;
    }
    
    program->sramSize = program->header.sramSize;
    TM32Core_LogInfo("Allocated SRAM: %u bytes (0x%08X)", program->sramSize, program->sramSize);
    
    return true;
}

static void TM32Core_DeallocateProgramMemory (
    TM32Core_Program*   program
)
{
    TM32Core_ReturnIfNull(program);
    
    // Deallocate ROM data
    if (program->romData != NULL)
    {
        TM32Core_Destroy(program->romData);
        program->romData = NULL;
        program->romSize = 0;
    }
    
    // Deallocate SRAM data
    if (program->sramData != NULL)
    {
        TM32Core_Destroy(program->sramData);
        program->sramData = NULL;
        program->sramSize = 0;
    }
    
    // Clear header and loaded flag
    memset(&program->header, 0, sizeof(TM32Core_ProgramHeader));
    program->isLoaded = false;
}

/* Public Functions ***********************************************************/

TM32Core_Program* TM32Core_CreateProgram ()
{
    TM32Core_Program* program = TM32Core_CreateZero(1, TM32Core_Program);
    if (program == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core program");
        return NULL;
    }

    // Initialize program state
    program->romData = NULL;
    program->romSize = 0;
    program->sramData = NULL;
    program->sramSize = 0;
    program->isLoaded = false;
    memset(&program->header, 0, sizeof(TM32Core_ProgramHeader));

    return program;
}

bool TM32Core_LoadProgram (
    TM32Core_Program*   program,
    const char*         path
)
{
    TM32Core_ReturnValueIfNull(program, false);
    TM32Core_ReturnValueIfNull(path, false);
    
    // Clear any existing program data
    TM32Core_DeallocateProgramMemory(program);
    
    // Open ROM file
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        TM32Core_LogErrno("Could not open ROM file: %s", path);
        return false;
    }
    
    // Get file size
    if (fseek(file, 0, SEEK_END) != 0)
    {
        TM32Core_LogErrno("Could not seek to end of ROM file");
        fclose(file);
        return false;
    }
    
    long fileSize = ftell(file);
    if (fileSize < 0)
    {
        TM32Core_LogErrno("Could not get ROM file size");
        fclose(file);
        return false;
    }
    
    if (fileSize < sizeof(TM32Core_ProgramHeader))
    {
        TM32Core_LogError("ROM file too small (needs at least %zu bytes for header)",
                         sizeof(TM32Core_ProgramHeader));
        fclose(file);
        return false;
    }
    
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        TM32Core_LogErrno("Could not seek to beginning of ROM file");
        fclose(file);
        return false;
    }
    
    // Read and validate header
    if (fread(&program->header, sizeof(TM32Core_ProgramHeader), 1, file) != 1)
    {
        TM32Core_LogErrno("Could not read ROM file header");
        fclose(file);
        return false;
    }
    
    if (!TM32Core_ValidateProgramHeader(&program->header))
    {
        TM32Core_LogError("Invalid program header in ROM file");
        fclose(file);
        return false;
    }
    
    // Validate file size against header requirements
    if ((uint32_t)fileSize < program->header.romSize)
    {
        TM32Core_LogError("ROM file size (%ld bytes) smaller than header ROM size (0x%08X bytes)",
                         fileSize, program->header.romSize);
        fclose(file);
        return false;
    }
    
    // Allocate ROM buffer
    program->romData = TM32Core_Create((uint32_t)fileSize, uint8_t);
    if (program->romData == NULL)
    {
        TM32Core_LogErrno("Could not allocate ROM buffer of size %ld", fileSize);
        fclose(file);
        return false;
    }
    
    // Read entire ROM file
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        TM32Core_LogErrno("Could not seek to beginning of ROM file for full read");
        TM32Core_Destroy(program->romData);
        program->romData = NULL;
        fclose(file);
        return false;
    }
    
    if (fread(program->romData, 1, (uint32_t)fileSize, file) != (uint32_t)fileSize)
    {
        TM32Core_LogErrno("Could not read complete ROM file");
        TM32Core_Destroy(program->romData);
        program->romData = NULL;
        fclose(file);
        return false;
    }
    
    fclose(file);
    program->romSize = (uint32_t)fileSize;
    
    // Allocate SRAM if requested
    if (!TM32Core_AllocateProgramSRAM(program))
    {
        TM32Core_LogError("Could not allocate SRAM for program");
        TM32Core_DeallocateProgramMemory(program);
        return false;
    }
    
    program->isLoaded = true;
    
    TM32Core_LogInfo("Successfully loaded ROM program: %s", program->header.title);
    TM32Core_LogInfo("  Author: %s", program->header.author);
    TM32Core_LogInfo("  Description: %s", program->header.description);
    TM32Core_LogInfo("  Version: %d.%d.%d", 
                    (program->header.version >> 24) & 0xFF,
                    (program->header.version >> 16) & 0xFF,
                    program->header.version & 0xFFFF);
    TM32Core_LogInfo("  ROM Size: %u bytes (0x%08X)", program->romSize, program->romSize);
    TM32Core_LogInfo("  WRAM Size: %u bytes (0x%08X)", program->header.wramSize, program->header.wramSize);
    TM32Core_LogInfo("  SRAM Size: %u bytes (0x%08X)", program->sramSize, program->sramSize);
    
    return true;
}

void TM32Core_DestroyProgram (
    TM32Core_Program*   program
)
{
    if (program != NULL)
    {
        TM32Core_DeallocateProgramMemory(program);
        TM32Core_Destroy(program);
    }
}

/* Program Memory Access Functions *******************************************/

bool TM32Core_ReadProgramROM (
    TM32Core_Program*   program,
    uint32_t            address,
    uint8_t*            data
)
{
    TM32Core_ReturnValueIfNull(program, false);
    TM32Core_ReturnValueIfNull(data, false);
    
    if (!program->isLoaded)
    {
        TM32Core_LogError("No program loaded");
        return false;
    }
    
    // Check address bounds (program ROM space: 0x00000000 - 0x7FFFFFFF)
    if (address > TM32CORE_PROGRAM_END)
    {
        TM32Core_LogError("ROM address 0x%08X out of bounds", address);
        return false;
    }
    
    // Check if address is within loaded ROM data
    if (address >= program->romSize)
    {
        // Return 0xFF for unloaded ROM space (common for emulators)
        *data = 0xFF;
        return true;
    }
    
    *data = program->romData[address];
    return true;
}

bool TM32Core_ReadProgramSRAM (
    TM32Core_Program*   program,
    uint32_t            address,
    uint8_t*            data
)
{
    TM32Core_ReturnValueIfNull(program, false);
    TM32Core_ReturnValueIfNull(data, false);
    
    if (!program->isLoaded)
    {
        TM32Core_LogError("No program loaded");
        return false;
    }
    
    if (program->sramData == NULL)
    {
        TM32Core_LogError("No SRAM allocated for this program");
        return false;
    }
    
    // Convert absolute address to SRAM offset
    if (address < TM32CORE_SAVE_RAM_START || address > TM32CORE_SAVE_RAM_END)
    {
        TM32Core_LogError("SRAM address 0x%08X out of bounds", address);
        return false;
    }
    
    uint32_t offset = address - TM32CORE_SAVE_RAM_START;
    if (offset >= program->sramSize)
    {
        TM32Core_LogError("SRAM offset 0x%08X exceeds allocated size 0x%08X", 
                         offset, program->sramSize);
        return false;
    }
    
    *data = program->sramData[offset];
    return true;
}

bool TM32Core_WriteProgramSRAM (
    TM32Core_Program*   program,
    uint32_t            address,
    uint8_t             data
)
{
    TM32Core_ReturnValueIfNull(program, false);
    
    if (!program->isLoaded)
    {
        TM32Core_LogError("No program loaded");
        return false;
    }
    
    if (program->sramData == NULL)
    {
        TM32Core_LogError("No SRAM allocated for this program");
        return false;
    }
    
    // Convert absolute address to SRAM offset
    if (address < TM32CORE_SAVE_RAM_START || address > TM32CORE_SAVE_RAM_END)
    {
        TM32Core_LogError("SRAM address 0x%08X out of bounds", address);
        return false;
    }
    
    uint32_t offset = address - TM32CORE_SAVE_RAM_START;
    if (offset >= program->sramSize)
    {
        TM32Core_LogError("SRAM offset 0x%08X exceeds allocated size 0x%08X", 
                         offset, program->sramSize);
        return false;
    }
    
    program->sramData[offset] = data;
    return true;
}

/* Program Information Functions *********************************************/

const TM32Core_ProgramHeader* TM32Core_GetProgramHeader (
    TM32Core_Program*   program
)
{
    TM32Core_ReturnValueIfNull(program, NULL);
    
    if (!program->isLoaded)
    {
        return NULL;
    }
    
    return &program->header;
}

uint32_t TM32Core_GetProgramROMSize (
    TM32Core_Program*   program
)
{
    TM32Core_ReturnValueIfNull(program, 0);
    
    return program->romSize;
}

uint32_t TM32Core_GetProgramSRAMSize (
    TM32Core_Program*   program
)
{
    TM32Core_ReturnValueIfNull(program, 0);
    
    return program->sramSize;
}

bool TM32Core_IsProgramLoaded (
    TM32Core_Program*   program
)
{
    TM32Core_ReturnValueIfNull(program, false);
    
    return program->isLoaded;
}
