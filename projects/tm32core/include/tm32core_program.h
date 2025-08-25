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

#endif // TM32CORE_PROGRAM_H
