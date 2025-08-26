/**
 * @file    tm32core_program.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-26
 * 
 * @brief   Contains the opaque type and function prototypes for the TM32 Core's
 *          ROM program interface.
 */

#ifndef TM32CORE_PROGRAM_H
#define TM32CORE_PROGRAM_H

/* Include Files **************************************************************/

#include <tm32core_common.h>

/* Typedefs and Forward Declarations ******************************************/

/**
 * @brief   The opaque type representing a TM32 Core ROM program.
 */
typedef struct TM32Core_Program TM32Core_Program;

/* Public Structures **********************************************************/

/**
 * @brief   Structure representing the header and metadata of a TM32 Core ROM
 *          program.
 */
typedef struct
{
    char        magic[8];           /** @brief Magic string "TM32CORE" */
    uint32_t    version;            /** @brief Program version (MMmmpppp) */
    uint32_t    reserved1;          /** @brief Reserved field */
    uint32_t    romSize;            /** @brief Requested ROM size in bytes */
    uint32_t    wramSize;           /** @brief Requested WRAM size in bytes */
    uint32_t    sramSize;           /** @brief Requested SRAM size in bytes */
    uint32_t    reserved2;          /** @brief Reserved field */
    char        title[32];          /** @brief Program title (null-terminated) */
    char        author[32];         /** @brief Program author (null-terminated) */
    char        description[64];    /** @brief Program description (null-terminated) */
    uint8_t     reserved3[3968];    /** @brief Reserved space (total header = 4KB) */
} TM32Core_ProgramHeader;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of a TM32 Core ROM program.
 * 
 * @return  A pointer to the newly created TM32 Core ROM program;
 *          `NULL` on failure.
 */
TM32CORE_API TM32Core_Program* TM32Core_CreateProgram ();

/**
 * @brief   Loads a TM32 Core ROM program from the specified file path.
 * 
 * This function reads the ROM file from the given path and loads its contents
 * into the provided TM32 Core ROM program instance. If the program instance
 * already contains data, it will be replaced with the new data from the file.
 * 
 * @param   program     A pointer to the TM32 Core ROM program to load data into.
 * @param   path        The filesystem path to the ROM file to load.
 * 
 * @return  `true` if the ROM program was loaded successfully;
 *          `false` if there was an error (e.g., file not found, read error,
 *          invalid program instance).
 */
TM32CORE_API bool TM32Core_LoadProgram (
    TM32Core_Program*   program,
    const char*         path
);

/**
 * @brief   Destroys and deallocates the given TM32 Core ROM program.
 * 
 * @param   program     A pointer to the TM32 Core ROM program to destroy.
 */
TM32CORE_API void TM32Core_DestroyProgram (
    TM32Core_Program*   program
);

/* Program Memory Access Functions *******************************************/

/**
 * @brief   Reads data from the program ROM (metadata, interrupt vectors, or code).
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * @param   address     The address to read from (0x00000000 - 0x7FFFFFFF).
 * @param   data        Pointer to store the read data.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ReadProgramROM (
    TM32Core_Program*   program,
    uint32_t            address,
    uint8_t*            data
);

/**
 * @brief   Reads data from the program's SRAM (if allocated).
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * @param   address     The SRAM address to read from (0xA0000000 - 0xBFFFFFFF).
 * @param   data        Pointer to store the read data.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_ReadProgramSRAM (
    TM32Core_Program*   program,
    uint32_t            address,
    uint8_t*            data
);

/**
 * @brief   Writes data to the program's SRAM (if allocated).
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * @param   address     The SRAM address to write to (0xA0000000 - 0xBFFFFFFF).
 * @param   data        The data to write.
 * 
 * @return  `true` on success; `false` on failure.
 */
TM32CORE_API bool TM32Core_WriteProgramSRAM (
    TM32Core_Program*   program,
    uint32_t            address,
    uint8_t             data
);

/* Program Information Functions *********************************************/

/**
 * @brief   Gets the program header from a loaded ROM program.
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * 
 * @return  A pointer to the program header; `NULL` if not loaded.
 */
TM32CORE_API const TM32Core_ProgramHeader* TM32Core_GetProgramHeader (
    TM32Core_Program*   program
);

/**
 * @brief   Gets the actual size of the loaded ROM data.
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * 
 * @return  The size of the loaded ROM in bytes; 0 if not loaded.
 */
TM32CORE_API uint32_t TM32Core_GetProgramROMSize (
    TM32Core_Program*   program
);

/**
 * @brief   Gets the allocated SRAM size for the program.
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * 
 * @return  The allocated SRAM size in bytes; 0 if not allocated.
 */
TM32CORE_API uint32_t TM32Core_GetProgramSRAMSize (
    TM32Core_Program*   program
);

/**
 * @brief   Checks if a program is loaded and valid.
 * 
 * @param   program     A pointer to the TM32 Core ROM program.
 * 
 * @return  `true` if program is loaded and valid; `false` otherwise.
 */
TM32CORE_API bool TM32Core_IsProgramLoaded (
    TM32Core_Program*   program
);

#endif // TM32CORE_PROGRAM_H
