/**
 * @file    tm32asm_preprocessor.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the preprocessor structure and related definitions and prototypes.
 */

#ifndef TM32ASM_PREPROCESSOR_H
#define TM32ASM_PREPROCESSOR_H

/* Include Files **************************************************************/

#include <tm32asm_token.h>
#include <tm32asm_token_stream.h>

/* Public Enumerations ********************************************************/



/* Public Structures **********************************************************/

/**
 * @brief   Represents the state and context of the TM32 Assembler Tool's
 *          preprocessor as it processes preprocessor directives to mutate the
 *          token stream produced by the lexer.
 * 
 * The preprocessor is the second step of the TM32 assembly process, following
 * the lexer. It is responsible for handling directives such as macros, file
 * inclusions, and conditional compilation. It is also responsible for instructing
 * the lexer to extract tokens from source files included with the `.include`
 * preprocessor directive.
 */
typedef struct
{

} TM32ASM_Preprocessor;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Assembly Tool's
 *          preprocessor (henceforth, "the TM32ASM preprocessor").
 * 
 * @return  A pointer to the newly-created preprocessor instance if successful;
 *          `NULL` otherwise.
 */
TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ();

/**
 * @brief   Destroys and deallocates the given TM32ASM preprocessor.
 * 
 * @param   preprocessor   A pointer to the TM32ASM preprocessor to destroy.
 */
void TM32ASM_DestroyPreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

#endif // TM32ASM_PREPROCESSOR_H
