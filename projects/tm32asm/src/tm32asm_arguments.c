/**
 * @file    tm32asm_arguments.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 *
 * @brief   Implements functions for handling command-line arguments.
 */

/* Include Files **************************************************************/

#include <tm32asm_arguments.h>

/* Private Variables **********************************************************/

static int          s_argCount = 0;
static const char** s_argValues = NULL;

/* Public Functions ***********************************************************/

void TM32ASM_CaptureArguments (
    int             argCount,
    const char**    argVector
)
{
    TM32ASM_ReleaseArguments();

    s_argCount = argCount;
    s_argValues = argVector;
}

void TM32ASM_ReleaseArguments ()
{
    s_argCount = 0;
    s_argValues = NULL;
}

int TM32ASM_HasArgumentKey (
    const char* longform,
    const char  shortform
)
{
    for (int i = 1; i < s_argCount; ++i)
    {
        const char* arg = s_argValues[i];

        if (
            longform != NULL &&
            strstr(arg, "--") == arg &&
            strcmp(arg + 2, longform) == 0
        )
        {
            return i;
        }
        else if (
            shortform != 0 &&
            strstr(arg, "-") == arg &&
            arg[1] == shortform &&
            arg[2] == '\0'
        )
        {
            return i;
        }
    }

    return 0;
}

const char* TM32ASM_GetArgumentValue (
    const char* longform,
    const char  shortform,
    int         index
)
{
    int keyIndex = TM32ASM_HasArgumentKey(longform, shortform);
    if (keyIndex == 0)
    {
        return NULL;
    }

    for (int i = keyIndex + 1; i < s_argCount; ++i)
    {
        const char* arg = s_argValues[i];
        if (arg[0] == '-')
        {
            return NULL;
        }

        if (index-- == 0)
        {
            return arg;
        }
    }

    return NULL;
}