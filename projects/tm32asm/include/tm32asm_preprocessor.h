/**
 * @file    tm32asm_preprocessor.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/30
 *
 * @brief   Contains the definitions and prototypes for the TM32 assembler
 *          tool's preprocessor component.
 */

#ifndef TM32ASM_PREPROCESSOR_H
#define TM32ASM_PREPROCESSOR_H

/* Include Files **************************************************************/

#include <tm32asm_lexer.h>

/* Public Constants ***********************************************************/



/* Public Enumerations ********************************************************/



/* Public Structures **********************************************************/

/**
 * @brief   The TM32 assembler tool's preprocessor component structure.
 */
typedef struct
{

} TM32ASM_Preprocessor;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates an instance of the TM32 assembler tool's
 *          preprocessor component.
 *
 * @return  A pointer to the newly created TM32ASM preprocessor component, or 
 *          `NULL` if the allocation fails.
 */
TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ();

/**
 * @brief   Destroys and deallocates an instance of the TM32 assembler tool's
 *          preprocessor component.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance to
 *                          destroy. If `NULL`, no action is taken.
 */
void TM32ASM_DestroyPreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

#endif // TM32ASM_PREPROCESSOR_H
