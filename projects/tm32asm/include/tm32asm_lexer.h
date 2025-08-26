/**
 * @file    tm32asm_lexer.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the lexer structure and related definitions and prototypes.
 */

#ifndef TM32ASM_LEXER_H
#define TM32ASM_LEXER_H

/* Include Files **************************************************************/

#include <tm32asm_token.h>

/* Public Structures **********************************************************/

/**
 * @brief   Represents the state and context of the TM32 Assembler Tool's
 *          lexer as it extracts tokens from source files.
 * 
 * The lexical analyzer (henceforth, "the lexer") is the first step of the
 * TM32 assembly process. It is responsible for loading source code from files
 * and extracting tokens from that code.
 */
typedef struct
{

} TM32ASM_Lexer;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Assembly Tool's
 *          lexer (henceforth, "the TM32ASM lexer").
 * 
 * @return  A pointer to the newly-created lexer instance if successful;
 *          `NULL` otherwise.
 */
TM32ASM_Lexer* TM32ASM_CreateLexer ();

/**
 * @brief   Destroys and deallocates the given TM32ASM lexer.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer to destroy.
 */
void TM32ASM_DestroyLexer (
    TM32ASM_Lexer* lexer
);

#endif // TM32ASM_LEXER_H
