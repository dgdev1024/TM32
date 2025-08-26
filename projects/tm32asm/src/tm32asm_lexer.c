/**
 * @file    tm32asm_lexer.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the implementation of the TM32ASM lexer functions.
 */

/* Include Files **************************************************************/

#include <tm32asm_lexer.h>

/* Public Functions ***********************************************************/

TM32ASM_Lexer* TM32ASM_CreateLexer ()
{
    TM32ASM_Lexer* lexer = TM32ASM_CreateZero(1, TM32ASM_Lexer);
    if (lexer == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM lexer");
        return NULL;
    }
    
    return lexer;
}

void TM32ASM_DestroyLexer (TM32ASM_Lexer* lexer)
{
    if (lexer != NULL)
    {
        TM32ASM_Destroy(lexer);
    }
}
