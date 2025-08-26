/**
 * @file    tm32asm_parser.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 *
 * @brief   Contains the implementation of the TM32 Assembler Tool's parser.
 */

/* Include Files **************************************************************/

#include <tm32asm_parser.h>

/* Public Functions ***********************************************************/

TM32ASM_Parser* TM32ASM_CreateParser ()
{
    TM32ASM_Parser* parser = TM32ASM_CreateZero(1, TM32ASM_Parser);
    if (parser == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM parser");
        return NULL;
    }

    return parser;
}

void TM32ASM_DestroyParser (
    TM32ASM_Parser* parser
)
{
    if (parser != NULL)
    {
        TM32ASM_Destroy(parser);
    }
}
