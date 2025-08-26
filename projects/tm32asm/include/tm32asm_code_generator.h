/**
 * @file    tm32asm_code_generator.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the codegen structure and related definitions and prototypes.
 */

#ifndef TM32ASM_CODE_GENERATOR_H
#define TM32ASM_CODE_GENERATOR_H

/* Include Files **************************************************************/

#include <tm32asm_token.h>

/* Public Structures **********************************************************/

/**
 * @brief   Represents the state and context of the TM32 Assembler Tool's
 *          code generator, which is responsible for generating the final
 *          machine code from the abstract syntax tree (AST) produced by
 *          the parser.
 * 
 * The code generator is the fifth and final step of the TM32 assembly process,
 * following the lexer, preprocessor, parser, and semantic analyzer. It is
 * responsible for walking, once again, through the AST, this time generating
 * the corresponding machine code for each node.
 */
typedef struct
{

} TM32ASM_CodeGenerator;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Assembly Tool's
 *          code generator (henceforth, "the TM32ASM code generator").
 * 
 * @return  A pointer to the newly-created code generator instance if successful;
 *          `NULL` otherwise.
 */
TM32ASM_CodeGenerator* TM32ASM_CreateCodegen ();

/**
 * @brief   Destroys and deallocates the given TM32ASM code generator.
 * 
 * @param   codegen   A pointer to the TM32ASM code generator to destroy.
 */
void TM32ASM_DestroyCodegen (
    TM32ASM_CodeGenerator* codegen
);

#endif // TM32ASM_CODE_GENERATOR_H
