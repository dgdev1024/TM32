/**
 * @file    tm32asm_semantic_analyzer.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the semantic analyzer structure and related definitions and 
 *          prototypes.
 */

#ifndef TM32ASM_SEMANTIC_ANALYZER_H
#define TM32ASM_SEMANTIC_ANALYZER_H

/* Include Files **************************************************************/

#include <tm32asm_token.h>

/* Public Structures **********************************************************/

/**
 * @brief   Represents the state and context of the TM32 Assembler Tool's
 *          semantic analyzer as it walks through the abstract syntax tree (AST)
 *          produced by the parser, checking for semantic errors and ensuring
 *          the correctness of the program.
 *
 * The semantic analyzer is the fourth step of the TM32 assembly process,
 * following the lexer, preprocessor, and parser. It is responsible for
 * checking the AST for semantic errors, such as type mismatches, undeclared
 * variables, and other issues that may affect the correctness of the program.
 */
typedef struct
{

} TM32ASM_SemanticAnalyzer;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Assembly Tool's
 *          semantic analyzer (henceforth, "the TM32ASM semantic analyzer").
 * 
 * @return  A pointer to the newly-created semantic analyzer instance if successful;
 *          `NULL` otherwise.
 */
TM32ASM_SemanticAnalyzer* TM32ASM_CreateSemanticAnalyzer ();

/**
 * @brief   Destroys and deallocates the given TM32ASM semantic analyzer.
 * 
 * @param   semantic   A pointer to the TM32ASM semantic analyzer to destroy.
 */
void TM32ASM_DestroySemanticAnalyzer (
    TM32ASM_SemanticAnalyzer* semantic
);

#endif // TM32ASM_SEMANTIC_ANALYZER_H
