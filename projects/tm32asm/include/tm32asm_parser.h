/**
 * @file    tm32asm_parser.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the parser structure and related definitions and prototypes.
 */

#ifndef TM32ASM_PARSER_H
#define TM32ASM_PARSER_H

/* Include Files **************************************************************/

#include <tm32asm_token.h>

/* Public Structures **********************************************************/

/**
 * @brief   Represents the state and context of the TM32 Assembler Tool's
 *          parser as it processes the token stream produced by the lexer and
 *          mutated by the preprocessor to produce an abstract syntax tree (AST).
 * 
 * The parser is the third step of the TM32 assembly process, following the
 * lexer and the preprocessor. It is responsible for analyzing the token stream
 * and constructing the AST, which represents the hierarchical structure of the
 * source code.
 */
typedef struct
{

} TM32ASM_Parser;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Assembly Tool's
 *          parser (henceforth, "the TM32ASM parser").
 * 
 * @return  A pointer to the newly-created parser instance if successful;
 *          `NULL` otherwise.
 */
TM32ASM_Parser* TM32ASM_CreateParser ();

/**
 * @brief   Destroys and deallocates the given TM32ASM parser.
 * 
 * @param   parser   A pointer to the TM32ASM parser to destroy.
 */
void TM32ASM_DestroyParser (
    TM32ASM_Parser* parser
);

#endif // TM32ASM_PARSER_H
