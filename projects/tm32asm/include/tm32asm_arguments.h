/**
 * @file    tm32asm_arguments.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains function prototypes for handling command-line arguments.
 */

#ifndef TM32ASM_ARGUMENTS_H
#define TM32ASM_ARGUMENTS_H

/* Include Files **************************************************************/

#include <tm32asm_common.h>

/* Public Function Prototypes *************************************************/

/**
 * @brief   Captures command-line arguments for processing.
 * 
 * @param   argCount    The number of command-line arguments.
 * @param   argVector   The array of command-line argument strings.
 */
void TM32ASM_CaptureArguments (
    int             argCount,
    const char**    argVector
);

/**
 * @brief   Releases any resources held for command-line argument processing.
 */
void TM32ASM_ReleaseArguments ();

/**
 * @brief   Checks if a command-line argument key exists.
 *
 * Command-line argument keys exist in one of two forms: a longform key with
 * two dashes and multiple characters (`--my-argument`) or a shortform key
 * with a single dash and a single character (`-m`).
 *
 * @param   longform    The longform argument key to check for (without the leading dashes).
 * @param   shortform   The shortform argument key to check for.
 *
 * @return  The index of the argument key if found, or 0 (the index of the
 *          application's invocation name) if not found.
 */
int TM32ASM_HasArgumentKey (
    const char* longform,
    const char  shortform
);

/**
 * @brief   Retrieves the value of a command-line argument.
 * 
 * Immediately following a command-line argument key, any number of argument
 * values can follow.
 *
 * @param   longform    The longform argument key to check for (without the leading dashes).
 * @param   shortform   The shortform argument key to check for.
 * @param   index       The index of the argument value to retrieve.
 *
 * @return  The value of the argument if found, or `NULL` if not found.
 */
const char* TM32ASM_GetArgumentValue (
    const char* longform,
    const char  shortform,
    int         index
);

/**
 * @brief   Retrieves a positional argument (non-option argument).
 * 
 * Positional arguments are arguments that are not preceded by option flags
 * and are not values for options.
 *
 * @param   index   The index of the positional argument to retrieve (0-based).
 *
 * @return  The positional argument if found, or `NULL` if not found.
 */
const char* TM32ASM_GetPositionalArgument (
    int index
);

#endif // TM32ASM_ARGUMENTS_H
