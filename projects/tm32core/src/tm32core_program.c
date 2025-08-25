/**
 * @file    tm32core_program.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025-08-25
 * 
 * @brief   Implementation of the TM32 Core ROM program structure and functions.
 */

/* Include Files **************************************************************/

#include <tm32core_program.h>

/* Private Structures *********************************************************/

struct TM32Core_Program
{
    
};

/* Private Function Prototypes ************************************************/

static bool TM32Core_ValidateProgramHeader (const TM32Core_ProgramHeader* header);

/* Private Functions **********************************************************/

bool TM32Core_ValidateProgramHeader (
    const TM32Core_ProgramHeader*  header
)
{
    TM32Core_ReturnValueIfNull(header, false);

    // Implement the header validation logic here
    return false;
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

    return program;
}

bool TM32Core_LoadProgram (
    TM32Core_Program*   program,
    const char*         path
)
{
    TM32Core_ReturnValueIfNull(program, false);
    TM32Core_ReturnValueIfNull(path, false);
    

    // Implement the program loading logic here
    return false;
}

void TM32Core_DestroyProgram (
    TM32Core_Program*   program
)
{
    if (program != NULL)
    {
        TM32Core_Destroy(program);
    }
}
