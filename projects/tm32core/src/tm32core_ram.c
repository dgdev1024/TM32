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

};

/* Public Functions ***********************************************************/

TM32Core_RAM* TM32Core_CreateRAM ()
{
    TM32Core_RAM* ram = TM32Core_CreateZero(1, TM32Core_RAM);
    if (ram == NULL)
    {
        TM32Core_LogErrno("Could not allocate the TM32 Core RAM module");
        return NULL;
    }

    return ram;
}

bool TM32Core_InitializeRAM (
    TM32Core_RAM*  ram
)
{
    TM32Core_ReturnValueIfNull(ram, false);

    // Initialize RAM resources here

    return true;
}

void TM32Core_DestroyRAM (
    TM32Core_RAM*  ram
)
{
    if (ram != NULL)
    {
        // Free RAM resources here
        TM32Core_Destroy(ram);
    }
}
