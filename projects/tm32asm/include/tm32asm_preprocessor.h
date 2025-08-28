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

/* Public Structures **********************************************************/

/**
 * @brief   Represents a symbol table entry for preprocessor variables/constants.
 */
typedef struct
{
    char*               name;           /** @brief Symbol name */
    TM32ASM_Token*      value;          /** @brief Token representing the symbol's value */
    bool                isConstant;     /** @brief Whether the symbol is constant (immutable) */
    uint32_t            line;           /** @brief Line where symbol was defined */
    const char*         filename;       /** @brief File where symbol was defined */
} TM32ASM_Symbol;

/**
 * @brief   Represents a macro definition.
 */
typedef struct
{
    char*               name;           /** @brief Macro name */
    char**              parameters;     /** @brief Parameter names */
    uint32_t            paramCount;     /** @brief Number of parameters */
    TM32ASM_TokenStream* body;          /** @brief Token stream containing macro body */
    uint32_t            line;           /** @brief Line where macro was defined */
    const char*         filename;       /** @brief File where macro was defined */
} TM32ASM_Macro;

/**
 * @brief   Represents a conditional block state.
 */
typedef struct
{
    bool                active;         /** @brief Whether this conditional block is active */
    bool                taken;          /** @brief Whether any branch has been taken */
    bool                inElse;         /** @brief Whether we're in an else block */
    uint32_t            line;           /** @brief Line where conditional started */
    const char*         filename;       /** @brief File where conditional started */
} TM32ASM_ConditionalState;

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
    // Symbol table for variables and constants
    TM32ASM_Symbol*     symbols;        /** @brief Dynamic array of symbols */
    uint32_t            symbolCount;    /** @brief Number of symbols */
    uint32_t            symbolCapacity; /** @brief Capacity of symbols array */
    
    // Macro table
    TM32ASM_Macro*      macros;         /** @brief Dynamic array of macros */
    uint32_t            macroCount;     /** @brief Number of macros */
    uint32_t            macroCapacity;  /** @brief Capacity of macros array */
    
    // Conditional stack
    TM32ASM_ConditionalState* conditionals; /** @brief Stack of conditional states */
    uint32_t            conditionalCount;    /** @brief Number of active conditionals */
    uint32_t            conditionalCapacity; /** @brief Capacity of conditionals stack */
    
    // Include path stack for error reporting
    const char**        includeStack;   /** @brief Stack of included file names */
    uint32_t            includeDepth;   /** @brief Current include depth */
    uint32_t            includeCapacity; /** @brief Capacity of include stack */
    
    // Include search paths
    const char**        includePaths;   /** @brief Array of include search directories */
    uint32_t            includePathCount; /** @brief Number of include paths */
    uint32_t            includePathCapacity; /** @brief Capacity of include paths array */
    
    // Included files tracking (to prevent circular includes)
    const char**        includedFiles;  /** @brief Array of already included files */
    uint32_t            includedFileCount; /** @brief Number of included files */
    uint32_t            includedFileCapacity; /** @brief Capacity of included files array */
    
    // Processing state
    bool                processingActive; /** @brief Whether preprocessor is currently active */
    uint32_t            macroDepth;     /** @brief Current macro expansion depth */
    uint32_t            maxMacroDepth;  /** @brief Maximum allowed macro expansion depth */

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

/**
 * @brief   Processes a token stream through the preprocessor, applying directives
 *          and transformations.
 * 
 * @param   preprocessor    The preprocessor instance to use.
 * @param   inputStream    The input token stream to process.
 * @param   outputStream   The output token stream to write processed tokens to.
 * 
 * @return  `true` if preprocessing was successful, `false` if errors occurred.
 */
bool TM32ASM_ProcessTokenStream (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream
);

/**
 * @brief   Processes a token stream through the preprocessor and returns a new
 *          processed token stream.
 * 
 * @param   preprocessor    The preprocessor instance to use.
 * @param   inputStream    The input token stream to process.
 * 
 * @return  A new token stream containing the processed tokens, or `NULL` on error.
 */
TM32ASM_TokenStream* TM32ASM_ProcessTokens (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream
);

/**
 * @brief   Adds an include search path to the preprocessor.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   path            The directory path to add for include file searching.
 * 
 * @return  `true` if the path was added successfully, `false` on error.
 */
bool TM32ASM_AddIncludePath (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             path
);

/**
 * @brief   Searches for an include file in the current directory and include paths.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   filename        The filename to search for.
 * @param   currentFile    The current file being processed (for relative searches).
 * 
 * @return  The full path to the found file, or `NULL` if not found.
 *          The returned string must be freed by the caller.
 */
char* TM32ASM_FindIncludeFile (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filename,
    const char*             currentFile
);

/**
 * @brief   Defines a symbol in the preprocessor's symbol table.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   name            The symbol name.
 * @param   value           The token representing the symbol's value.
 * @param   isConstant     Whether the symbol is constant (immutable).
 * @param   filename        The file where the symbol is defined.
 * @param   line            The line where the symbol is defined.
 * 
 * @return  `true` if the symbol was defined successfully, `false` otherwise.
 */
bool TM32ASM_DefineSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    TM32ASM_Token*          value,
    bool                    isConstant,
    const char*             filename,
    uint32_t                line
);

/**
 * @brief   Looks up a symbol in the preprocessor's symbol table.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   name            The symbol name to look up.
 * 
 * @return  A pointer to the symbol if found, or `NULL` if not found.
 */
TM32ASM_Symbol* TM32ASM_LookupSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
);

/**
 * @brief   Defines a macro in the preprocessor's macro table.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   name            The macro name.
 * @param   parameters      Array of parameter names.
 * @param   paramCount     Number of parameters.
 * @param   body            Token stream containing the macro body.
 * @param   filename        The file where the macro is defined.
 * @param   line            The line where the macro is defined.
 * 
 * @return  `true` if the macro was defined successfully, `false` otherwise.
 */
bool TM32ASM_DefineMacro (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    char**                  parameters,
    uint32_t                paramCount,
    TM32ASM_TokenStream*    body,
    const char*             filename,
    uint32_t                line
);

/**
 * @brief   Looks up a macro in the preprocessor's macro table.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   name            The macro name to look up.
 * 
 * @return  A pointer to the macro if found, or `NULL` if not found.
 */
TM32ASM_Macro* TM32ASM_LookupMacro (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
);

/**
 * @brief   Pushes a new conditional state onto the conditional stack.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   active          Whether the conditional block is active.
 * @param   filename        The file where the conditional starts.
 * @param   line            The line where the conditional starts.
 * 
 * @return  `true` if the conditional was pushed successfully, `false` otherwise.
 */
bool TM32ASM_PushConditional (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    active,
    const char*             filename,
    uint32_t                line
);

/**
 * @brief   Pops the top conditional state from the conditional stack.
 * 
 * @param   preprocessor    The preprocessor instance.
 * 
 * @return  `true` if the conditional was popped successfully, `false` if stack is empty.
 */
bool TM32ASM_PopConditional (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Gets the current conditional state.
 * 
 * @param   preprocessor    The preprocessor instance.
 * 
 * @return  A pointer to the current conditional state, or `NULL` if no conditionals are active.
 */
TM32ASM_ConditionalState* TM32ASM_GetCurrentConditional (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Checks if the preprocessor should currently process tokens (i.e., not in a false conditional).
 * 
 * @param   preprocessor    The preprocessor instance.
 * 
 * @return  `true` if tokens should be processed, `false` if they should be skipped.
 */
bool TM32ASM_ShouldProcessTokens (
    TM32ASM_Preprocessor*   preprocessor
);

#endif // TM32ASM_PREPROCESSOR_H
