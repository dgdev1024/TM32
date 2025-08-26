/**
 * @file    tm32asm_semantic_analyzer.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 *
 * @brief   Contains the implementation of the TM32 Assembler Tool's semantic 
 *          analyzer.
 */

/* Include Files **************************************************************/

#include <tm32asm_semantic_analyzer.h>

/* Public Functions ***********************************************************/

TM32ASM_SemanticAnalyzer* TM32ASM_CreateSemanticAnalyzer ()
{
    TM32ASM_SemanticAnalyzer* semantic = TM32ASM_CreateZero(1, TM32ASM_SemanticAnalyzer);
    if (semantic == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM semantic analyzer");
        return NULL;
    }

    return semantic;
}

void TM32ASM_DestroySemanticAnalyzer (
    TM32ASM_SemanticAnalyzer* semantic
)
{
    if (semantic != NULL)
    {
        TM32ASM_Destroy(semantic);
    }
}
