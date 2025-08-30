/**
 * @file    tm32asm_preprocessor.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/30
 *
 * @brief   Contains the implementation of the TM32 assembler tool's
 *          preprocessor component.
 */

/* Include Files **************************************************************/

#include <tm32asm_preprocessor.h>

/* Private Function Prototypes ************************************************/



/* Private Functions **********************************************************/



/* Public Functions ***********************************************************/

TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ()
{
    TM32ASM_Preprocessor* preprocessor =
        TM32ASM_CreateZero(1, TM32ASM_Preprocessor);
    if (preprocessor == NULL)
    {
        TM32ASM_LogErrno("Could not allocate TM32ASM preprocessor");
        return NULL;
    }

    return preprocessor;
}

void TM32ASM_DestroyPreprocessor (
    TM32ASM_Preprocessor* preprocessor
)
{
    if (preprocessor != NULL)
    {
        TM32ASM_Destroy(preprocessor);
    }
}
