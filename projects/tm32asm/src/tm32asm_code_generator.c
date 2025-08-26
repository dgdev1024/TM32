/**
 * @file    tm32asm_code_generator.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 *
 * @brief   Contains the implementation of the TM32 Assembler Tool's code generator.
 */

/* Include Files **************************************************************/

#include <tm32asm_code_generator.h>

/* Public Functions ***********************************************************/

TM32ASM_CodeGenerator* TM32ASM_CreateCodeGenerator ()
{
    TM32ASM_CodeGenerator* codegen = TM32ASM_CreateZero(1, TM32ASM_CodeGenerator);
    if (codegen == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM code generator");
        return NULL;
    }

    return codegen;
}

void TM32ASM_DestroyCodeGenerator (
    TM32ASM_CodeGenerator* codegen
)
{
    if (codegen != NULL)
    {
        TM32ASM_Destroy(codegen);
    }
}
