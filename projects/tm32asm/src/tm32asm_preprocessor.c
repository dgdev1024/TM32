/**
 * @file    tm32asm_preprocessor.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 *
 * @brief   Contains the implementation of the TM32 Assembler Tool's preprocessor.
 */

/* Include Files **************************************************************/

#include <tm32asm_preprocessor.h>
#include <tm32asm_lexer.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* Private Macros *************************************************************/

#define INITIAL_SYMBOL_CAPACITY     32
#define INITIAL_MACRO_CAPACITY      16
#define INITIAL_CONDITIONAL_CAPACITY 8
#define INITIAL_REPEAT_CAPACITY     8
#define INITIAL_WHILE_CAPACITY      8
#define INITIAL_FOR_CAPACITY        8
#define INITIAL_INCLUDE_CAPACITY    16
#define MAX_MACRO_DEPTH             64
#define MAX_WHILE_ITERATIONS        10000  // Safety limit to prevent infinite loops

/* Private Function Prototypes ************************************************/

/**
 * @brief   Grows a dynamic array to accommodate more elements.
 * 
 * @param   array       Pointer to the array pointer.
 * @param   capacity    Pointer to the capacity variable.
 * @param   element_size Size of each element in bytes.
 * 
 * @return  `true` if successful, `false` if memory allocation fails.
 */
static bool TM32ASM_GrowArray (
    void**      array,
    uint32_t*   capacity,
    size_t      element_size
);

/**
 * @brief   Destroys a symbol and frees its memory.
 * 
 * @param   symbol  The symbol to destroy.
 */
static void TM32ASM_DestroySymbol (
    TM32ASM_Symbol* symbol
);

/**
 * @brief   Destroys a macro and frees its memory.
 * 
 * @param   macro   The macro to destroy.
 */
static void TM32ASM_DestroyMacro (
    TM32ASM_Macro* macro
);

/**
 * @brief   Processes a single token from the input stream.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The token to process.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_ProcessToken (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles a directive token.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .include directive to include other source files.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .include directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleIncludeDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .let directive to define mutable variables.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .let directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleLetDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .const directive to define immutable constants.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .const directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleConstDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .if directive for conditional assembly.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .if directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleIfDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .elseif/.elif directive for conditional assembly.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .elseif/.elif directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleElseifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .else directive for conditional assembly.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .else directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleElseDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .endif/.endc directive for conditional assembly.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .endif/.endc directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleEndifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .repeat/.rept directive for repetition.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .repeat/.rept directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleRepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .endrepeat/.endr directive for repetition.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .endrepeat/.endr directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleEndrepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .while directive for conditional looping.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .while directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleWhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .endwhile/.endw directive for conditional looping.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .endwhile/.endw directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleEndwhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .for directive for counted loops.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .for directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleForDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .endfor/.endf directive for counted loops.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream    The input token stream.
 * @param   outputStream   The output token stream.
 * @param   token           The .endfor/.endf directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleEndforDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .continue directive to skip the rest of the current loop iteration.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream     The input token stream.
 * @param   outputStream    The output token stream.
 * @param   token           The .continue directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleContinueDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .break directive to exit the current loop early.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream     The input token stream.
 * @param   outputStream    The output token stream.
 * @param   token           The .break directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleBreakDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .warn directive to emit a warning message.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream     The input token stream.
 * @param   outputStream    The output token stream.
 * @param   token           The .warn directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleWarnDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .error directive to emit an error message and halt assembly.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream     The input token stream.
 * @param   outputStream    The output token stream.
 * @param   token           The .error directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleErrorDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .assert directive to check a condition and emit an error if false.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream     The input token stream.
 * @param   outputStream    The output token stream.
 * @param   token           The .assert directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleAssertDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .file directive to set the current filename for debug info.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream     The input token stream.
 * @param   outputStream    The output token stream.
 * @param   token           The .file directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleFileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles the .line directive to set the current line number for debug info.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   inputStream     The input token stream.
 * @param   outputStream    The output token stream.
 * @param   token           The .line directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleLineDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles symbol substitution for identifier tokens.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   outputStream   The output token stream.
 * @param   token           The identifier token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleSymbolSubstitution (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
);

/**
 * @brief   Checks if a file has already been included to prevent circular includes.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   filepath        The full path of the file to check.
 * 
 * @return  `true` if the file was already included, `false` otherwise.
 */
static bool TM32ASM_IsFileAlreadyIncluded (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filepath
);

/**
 * @brief   Adds a file to the included files list.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   filepath        The full path of the file to add.
 * 
 * @return  `true` if the file was added successfully, `false` on error.
 */
static bool TM32ASM_AddIncludedFile (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filepath
);

/* Public Functions ***********************************************************/

TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ()
{
    TM32ASM_Preprocessor* preprocessor = TM32ASM_Create(1, TM32ASM_Preprocessor);
    if (preprocessor == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM preprocessor");
        return NULL;
    }
    
    // Initialize symbol table
    preprocessor->symbols = TM32ASM_Create(INITIAL_SYMBOL_CAPACITY, TM32ASM_Symbol);
    if (preprocessor->symbols == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for symbol table");
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->symbolCount = 0;
    preprocessor->symbolCapacity = INITIAL_SYMBOL_CAPACITY;
    
    // Initialize macro table
    preprocessor->macros = TM32ASM_Create(INITIAL_MACRO_CAPACITY, TM32ASM_Macro);
    if (preprocessor->macros == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for macro table");
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->macroCount = 0;
    preprocessor->macroCapacity = INITIAL_MACRO_CAPACITY;
    
    // Initialize conditional stack
    preprocessor->conditionals = TM32ASM_Create(INITIAL_CONDITIONAL_CAPACITY, TM32ASM_ConditionalState);
    if (preprocessor->conditionals == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for conditional stack");
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->conditionalCount = 0;
    preprocessor->conditionalCapacity = INITIAL_CONDITIONAL_CAPACITY;
    
    // Initialize repetition stack
    preprocessor->repeats = TM32ASM_Create(INITIAL_REPEAT_CAPACITY, TM32ASM_RepeatState);
    if (preprocessor->repeats == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for repetition stack");
        TM32ASM_Destroy(preprocessor->conditionals);
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->repeatCount = 0;
    preprocessor->repeatCapacity = INITIAL_REPEAT_CAPACITY;
    
    // Initialize while loop stack
    preprocessor->whiles = TM32ASM_Create(INITIAL_WHILE_CAPACITY, TM32ASM_WhileState);
    if (preprocessor->whiles == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for while loop stack");
        TM32ASM_Destroy(preprocessor->repeats);
        TM32ASM_Destroy(preprocessor->conditionals);
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->whileCount = 0;
    preprocessor->whileCapacity = INITIAL_WHILE_CAPACITY;
    
    // Initialize for loop stack
    preprocessor->fors = TM32ASM_Create(INITIAL_FOR_CAPACITY, TM32ASM_ForState);
    if (preprocessor->fors == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for for loop stack");
        TM32ASM_Destroy(preprocessor->whiles);
        TM32ASM_Destroy(preprocessor->repeats);
        TM32ASM_Destroy(preprocessor->conditionals);
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->forCount = 0;
    preprocessor->forCapacity = INITIAL_FOR_CAPACITY;
    
    // Initialize loop control state
    preprocessor->loopControl = TM32ASM_LOOP_CONTROL_NONE;
    
    // Initialize include stack
    preprocessor->includeStack = TM32ASM_Create(INITIAL_INCLUDE_CAPACITY, const char*);
    if (preprocessor->includeStack == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for include stack");
        TM32ASM_Destroy(preprocessor->fors);
        TM32ASM_Destroy(preprocessor->whiles);
        TM32ASM_Destroy(preprocessor->repeats);
        TM32ASM_Destroy(preprocessor->conditionals);
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->includeDepth = 0;
    preprocessor->includeCapacity = INITIAL_INCLUDE_CAPACITY;
    
    // Initialize include paths
    preprocessor->includePaths = TM32ASM_Create(INITIAL_INCLUDE_CAPACITY, const char*);
    if (preprocessor->includePaths == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for include paths");
        TM32ASM_Destroy(preprocessor->includeStack);
        TM32ASM_Destroy(preprocessor->fors);
        TM32ASM_Destroy(preprocessor->whiles);
        TM32ASM_Destroy(preprocessor->repeats);
        TM32ASM_Destroy(preprocessor->conditionals);
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->includePathCount = 0;
    preprocessor->includePathCapacity = INITIAL_INCLUDE_CAPACITY;
    
    // Initialize included files tracking
    preprocessor->includedFiles = TM32ASM_Create(INITIAL_INCLUDE_CAPACITY, const char*);
    if (preprocessor->includedFiles == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for included files tracking");
        TM32ASM_Destroy(preprocessor->includePaths);
        TM32ASM_Destroy(preprocessor->includeStack);
        TM32ASM_Destroy(preprocessor->fors);
        TM32ASM_Destroy(preprocessor->whiles);
        TM32ASM_Destroy(preprocessor->repeats);
        TM32ASM_Destroy(preprocessor->conditionals);
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->includedFileCount = 0;
    preprocessor->includedFileCapacity = INITIAL_INCLUDE_CAPACITY;
    
    // Initialize processing state
    preprocessor->processingActive = true;
    preprocessor->macroDepth = 0;
    preprocessor->maxMacroDepth = MAX_MACRO_DEPTH;
    
    // Initialize debug information state
    preprocessor->debugFilename = NULL;
    preprocessor->debugLine = 0;
    
    TM32ASM_LogDebug("Created preprocessor instance");
    return preprocessor;
}

void TM32ASM_DestroyPreprocessor (
    TM32ASM_Preprocessor* preprocessor
)
{
    if (preprocessor != NULL)
    {
        // Destroy all symbols
        if (preprocessor->symbols != NULL) {
            for (uint32_t i = 0; i < preprocessor->symbolCount; ++i) {
                TM32ASM_DestroySymbol(&preprocessor->symbols[i]);
            }
            TM32ASM_Destroy(preprocessor->symbols);
        }
        
        // Destroy all macros
        if (preprocessor->macros != NULL) {
            for (uint32_t i = 0; i < preprocessor->macroCount; ++i) {
                TM32ASM_DestroyMacro(&preprocessor->macros[i]);
            }
            TM32ASM_Destroy(preprocessor->macros);
        }
        
        // Destroy conditional stack
        TM32ASM_Destroy(preprocessor->conditionals);
        
        // Destroy repetition stack
        if (preprocessor->repeats != NULL) {
            for (uint32_t i = 0; i < preprocessor->repeatCount; ++i) {
                if (preprocessor->repeats[i].body != NULL) {
                    TM32ASM_DestroyTokenStream(preprocessor->repeats[i].body);
                }
            }
            TM32ASM_Destroy(preprocessor->repeats);
        }
        
        // Destroy while loop stack
        if (preprocessor->whiles != NULL) {
            for (uint32_t i = 0; i < preprocessor->whileCount; ++i) {
                if (preprocessor->whiles[i].body != NULL) {
                    TM32ASM_DestroyTokenStream(preprocessor->whiles[i].body);
                }
                if (preprocessor->whiles[i].condition != NULL) {
                    TM32ASM_DestroyTokenStream(preprocessor->whiles[i].condition);
                }
            }
            TM32ASM_Destroy(preprocessor->whiles);
        }
        
        // Destroy for loop stack
        if (preprocessor->fors != NULL) {
            for (uint32_t i = 0; i < preprocessor->forCount; ++i) {
                if (preprocessor->fors[i].body != NULL) {
                    TM32ASM_DestroyTokenStream(preprocessor->fors[i].body);
                }
                if (preprocessor->fors[i].variable != NULL) {
                    free(preprocessor->fors[i].variable);
                }
            }
            TM32ASM_Destroy(preprocessor->fors);
        }
        
        // Destroy include stack
        TM32ASM_Destroy(preprocessor->includeStack);
        
        // Destroy include paths
        if (preprocessor->includePaths != NULL) {
            for (uint32_t i = 0; i < preprocessor->includePathCount; ++i) {
                free((char*)preprocessor->includePaths[i]);
            }
            TM32ASM_Destroy(preprocessor->includePaths);
        }
        
        // Destroy included files tracking
        if (preprocessor->includedFiles != NULL) {
            for (uint32_t i = 0; i < preprocessor->includedFileCount; ++i) {
                free((char*)preprocessor->includedFiles[i]);
            }
            TM32ASM_Destroy(preprocessor->includedFiles);
        }
        
        // Clean up debug information
        if (preprocessor->debugFilename != NULL) {
            TM32ASM_Destroy(preprocessor->debugFilename);
        }
        
        TM32ASM_Destroy(preprocessor);
        TM32ASM_LogDebug("Destroyed preprocessor instance");
    }
}

bool TM32ASM_ProcessTokenStream (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    
    TM32ASM_LogDebug("Processing token stream");
    
    // Process each token in the input stream
    TM32ASM_Token* token;
    while ((token = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        if (!TM32ASM_ProcessToken(preprocessor, inputStream, outputStream, token)) {
            TM32ASM_LogError("Failed to process token: %s", token->lexeme);
            return false;
        }
    }
    
    // Check for unclosed conditionals
    if (preprocessor->conditionalCount > 0) {
        TM32ASM_ConditionalState* current = &preprocessor->conditionals[preprocessor->conditionalCount - 1];
        TM32ASM_LogError("Unclosed conditional directive at %s:%u", 
                         current->filename, current->line);
        return false;
    }
    
    TM32ASM_LogDebug("Token stream processing completed successfully");
    return true;
}

bool TM32ASM_DefineSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    TM32ASM_Token*          value,
    bool                    isConstant,
    const char*             filename,
    uint32_t                line
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(name, false);
    TM32ASM_ReturnValueIfNull(value, false);
    
    // Check if symbol already exists
    TM32ASM_Symbol* existing = TM32ASM_LookupSymbol(preprocessor, name);
    if (existing != NULL) {
        if (existing->isConstant) {
            TM32ASM_LogError("Cannot redefine constant symbol '%s' (originally defined at %s:%u)",
                             name, existing->filename, existing->line);
            return false;
        }
        
        // Update existing mutable symbol
        TM32ASM_DestroyToken(existing->value);
        existing->value = value;
        existing->line = line;
        existing->filename = filename;
        TM32ASM_LogDebug("Updated symbol '%s'", name);
        return true;
    }
    
    // Grow array if needed
    if (preprocessor->symbolCount >= preprocessor->symbolCapacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->symbols, 
                               &preprocessor->symbolCapacity, 
                               sizeof(TM32ASM_Symbol))) {
            TM32ASM_LogError("Failed to grow symbol table");
            return false;
        }
    }
    
    // Add new symbol
    TM32ASM_Symbol* symbol = &preprocessor->symbols[preprocessor->symbolCount++];
    symbol->name = TM32ASM_Create(strlen(name) + 1, char);
    if (symbol->name == NULL) {
        TM32ASM_LogError("Failed to allocate memory for symbol name");
        --preprocessor->symbolCount;
        return false;
    }
    strcpy(symbol->name, name);
    symbol->value = value;
    symbol->isConstant = isConstant;
    symbol->line = line;
    symbol->filename = filename;
    
    TM32ASM_LogDebug("Defined %s symbol '%s'", isConstant ? "constant" : "variable", name);
    return true;
}

TM32ASM_Symbol* TM32ASM_LookupSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(name, NULL);
    
    for (uint32_t i = 0; i < preprocessor->symbolCount; ++i) {
        if (strcmp(preprocessor->symbols[i].name, name) == 0) {
            return &preprocessor->symbols[i];
        }
    }
    
    return NULL;
}

bool TM32ASM_DefineMacro (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    char**                  parameters,
    uint32_t                paramCount,
    TM32ASM_TokenStream*    body,
    const char*             filename,
    uint32_t                line
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(name, false);
    TM32ASM_ReturnValueIfNull(body, false);
    
    // Check if macro already exists
    TM32ASM_Macro* existing = TM32ASM_LookupMacro(preprocessor, name);
    if (existing != NULL) {
        TM32ASM_LogError("Macro '%s' already defined at %s:%u",
                         name, existing->filename, existing->line);
        return false;
    }
    
    // Grow array if needed
    if (preprocessor->macroCount >= preprocessor->macroCapacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->macros, 
                               &preprocessor->macroCapacity, 
                               sizeof(TM32ASM_Macro))) {
            TM32ASM_LogError("Failed to grow macro table");
            return false;
        }
    }
    
    // Add new macro
    TM32ASM_Macro* macro = &preprocessor->macros[preprocessor->macroCount++];
    macro->name = TM32ASM_Create(strlen(name) + 1, char);
    if (macro->name == NULL) {
        TM32ASM_LogError("Failed to allocate memory for macro name");
        --preprocessor->macroCount;
        return false;
    }
    strcpy(macro->name, name);
    
    macro->parameters = parameters;
    macro->paramCount = paramCount;
    macro->body = body;
    macro->line = line;
    macro->filename = filename;
    
    TM32ASM_LogDebug("Defined macro '%s' with %u parameters", name, paramCount);
    return true;
}

TM32ASM_Macro* TM32ASM_LookupMacro (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(name, NULL);
    
    for (uint32_t i = 0; i < preprocessor->macroCount; ++i) {
        if (strcmp(preprocessor->macros[i].name, name) == 0) {
            return &preprocessor->macros[i];
        }
    }
    
    return NULL;
}

bool TM32ASM_PushConditional (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    active,
    const char*             filename,
    uint32_t                line
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    // Grow array if needed
    if (preprocessor->conditionalCount >= preprocessor->conditionalCapacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->conditionals, 
                               &preprocessor->conditionalCapacity, 
                               sizeof(TM32ASM_ConditionalState))) {
            TM32ASM_LogError("Failed to grow conditional stack");
            return false;
        }
    }
    
    // Push new conditional state
    TM32ASM_ConditionalState* state = &preprocessor->conditionals[preprocessor->conditionalCount++];
    state->active = active;
    state->taken = active;
    state->inElse = false;
    state->line = line;
    state->filename = filename;
    
    TM32ASM_LogDebug("Pushed conditional (active=%s) at %s:%u", 
                     active ? "true" : "false", filename, line);
    return true;
}

bool TM32ASM_PopConditional (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->conditionalCount == 0) {
        TM32ASM_LogError("No conditional to pop");
        return false;
    }
    
    --preprocessor->conditionalCount;
    TM32ASM_LogDebug("Popped conditional from stack");
    return true;
}

TM32ASM_ConditionalState* TM32ASM_GetCurrentConditional (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    
    if (preprocessor->conditionalCount == 0) {
        return NULL;
    }
    
    return &preprocessor->conditionals[preprocessor->conditionalCount - 1];
}

bool TM32ASM_ShouldProcessTokens (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, true);
    
    if (!preprocessor->processingActive) {
        return false;
    }
    
    // Check all conditional states - all must be active
    for (uint32_t i = 0; i < preprocessor->conditionalCount; ++i) {
        if (!preprocessor->conditionals[i].active) {
            return false;
        }
    }
    
    return true;
}

/* Private Function Implementations *******************************************/

static bool TM32ASM_GrowArray (
    void**      array,
    uint32_t*   capacity,
    size_t      element_size
)
{
    uint32_t new_capacity = (*capacity) * 2;
    void* new_array = realloc(*array, new_capacity * element_size);
    if (new_array == NULL) {
        return false;
    }
    
    *array = new_array;
    *capacity = new_capacity;
    return true;
}

static void TM32ASM_DestroySymbol (
    TM32ASM_Symbol* symbol
)
{
    if (symbol == NULL) return;
    
    TM32ASM_Destroy(symbol->name);
    TM32ASM_DestroyToken(symbol->value);
}

static void TM32ASM_DestroyMacro (
    TM32ASM_Macro* macro
)
{
    if (macro == NULL) return;
    
    TM32ASM_Destroy(macro->name);
    
    // Destroy parameter names
    if (macro->parameters != NULL) {
        for (uint32_t i = 0; i < macro->paramCount; ++i) {
            TM32ASM_Destroy(macro->parameters[i]);
        }
        TM32ASM_Destroy(macro->parameters);
    }
    
    // Destroy macro body
    TM32ASM_DestroyTokenStream(macro->body);
}

static bool TM32ASM_ProcessToken (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    // Handle directive tokens (always process directives as they control conditional state)
    if (token->type == TM32ASM_TT_DIRECTIVE) {
        return TM32ASM_HandleDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // If we're not processing tokens (e.g., in a false conditional), skip them
    if (!TM32ASM_ShouldProcessTokens(preprocessor)) {
        return true;
    }
    
    // Handle identifier tokens for symbol substitution
    if (token->type == TM32ASM_TT_IDENTIFIER) {
        return TM32ASM_HandleSymbolSubstitution(preprocessor, outputStream, token);
    }
    
    // Pass through all other tokens - create a copy to avoid double-free
    TM32ASM_Token* tokenCopy = TM32ASM_CreateToken(token->lexeme, token->type);
    if (tokenCopy == NULL) {
        TM32ASM_LogError("Failed to create token copy");
        return false;
    }
    tokenCopy->param = token->param;
    tokenCopy->filename = token->filename;
    tokenCopy->line = token->line;
    
    TM32ASM_InsertToken(outputStream, tokenCopy);
    return true;
}

static bool TM32ASM_HandleIncludeDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .include directive at %s:%u", token->filename, token->line);
    
    // Get the next token, which should be a string literal containing the filename
    TM32ASM_Token* filenameToken = TM32ASM_ConsumeNextToken(inputStream);
    if (filenameToken == NULL) {
        TM32ASM_LogError("Expected filename after .include directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    if (filenameToken->type != TM32ASM_TT_STRING) {
        TM32ASM_LogError("Expected string literal filename after .include "
                         "directive at %s:%u, got %s",
                         token->filename, token->line, 
                         filenameToken->lexeme);
        return false;
    }
    
    // Remove quotes from filename
    const char* filename = filenameToken->lexeme;
    size_t filenameLen = strlen(filename);
    if (filenameLen < 2 || filename[0] != '"' || 
        filename[filenameLen - 1] != '"') {
        TM32ASM_LogError("Malformed string literal in .include directive "
                         "at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    // Extract filename without quotes
    char* includeFilename = TM32ASM_Create(filenameLen - 1, char);
    if (includeFilename == NULL) {
        TM32ASM_LogError("Failed to allocate memory for filename from "
                         ".include directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    strncpy(includeFilename, filename + 1, filenameLen - 2);
    includeFilename[filenameLen - 2] = '\0';
    
    // Find the include file
    char* fullPath = TM32ASM_FindIncludeFile(preprocessor, 
                                              includeFilename, 
                                              token->filename);
    if (fullPath == NULL) {
        TM32ASM_LogError("Cannot find include file '%s' at %s:%u", 
                         includeFilename, token->filename, token->line);
        TM32ASM_Destroy(includeFilename);
        return false;
    }
    
    // Check if file was already included
    if (TM32ASM_IsFileAlreadyIncluded(preprocessor, fullPath)) {
        TM32ASM_LogDebug("File '%s' already included, skipping", fullPath);
        TM32ASM_Destroy(includeFilename);
        TM32ASM_Destroy(fullPath);
        return true;
    }
    
    // Add to included files list
    if (!TM32ASM_AddIncludedFile(preprocessor, fullPath)) {
        TM32ASM_LogError("Failed to track included file '%s'", fullPath);
        TM32ASM_Destroy(includeFilename);
        TM32ASM_Destroy(fullPath);
        return false;
    }
    
    // Check include depth limit
    const uint32_t MAX_INCLUDE_DEPTH = 64;
    if (preprocessor->includeDepth >= MAX_INCLUDE_DEPTH) {
        TM32ASM_LogError("Maximum include depth (%u) exceeded at %s:%u", 
                         MAX_INCLUDE_DEPTH, token->filename, token->line);
        TM32ASM_Destroy(includeFilename);
        TM32ASM_Destroy(fullPath);
        return false;
    }
    
    // Push current file onto include stack
    if (preprocessor->includeDepth >= preprocessor->includeCapacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->includeStack,
                               &preprocessor->includeCapacity,
                               sizeof(const char*))) {
            TM32ASM_LogError("Failed to grow include stack");
            TM32ASM_Destroy(includeFilename);
            TM32ASM_Destroy(fullPath);
            return false;
        }
    }
    
    preprocessor->includeStack[preprocessor->includeDepth++] = 
        token->filename;
    
    // Lex the included file
    TM32ASM_Lexer* lexer = TM32ASM_CreateLexer();
    if (lexer == NULL) {
        TM32ASM_LogError("Failed to create lexer for included file '%s'", 
                         fullPath);
        preprocessor->includeDepth--; // Pop include stack
        TM32ASM_Destroy(includeFilename);
        TM32ASM_Destroy(fullPath);
        return false;
    }
    
    if (!TM32ASM_LoadSourceFile(lexer, fullPath)) {
        TM32ASM_LogError("Failed to load included file '%s'", fullPath);
        preprocessor->includeDepth--; // Pop include stack
        TM32ASM_DestroyLexer(lexer);
        TM32ASM_Destroy(includeFilename);
        TM32ASM_Destroy(fullPath);
        return false;
    }
    
    if (!TM32ASM_TokenizeSource(lexer)) {
        TM32ASM_LogError("Failed to tokenize included file '%s'", fullPath);
        preprocessor->includeDepth--; // Pop include stack
        TM32ASM_DestroyLexer(lexer);
        TM32ASM_Destroy(includeFilename);
        TM32ASM_Destroy(fullPath);
        return false;
    }
    
    TM32ASM_TokenStream* includedStream = TM32ASM_GetTokenStream(lexer);
    if (includedStream == NULL) {
        TM32ASM_LogError("Failed to get token stream from included file '%s'",
                         fullPath);
        preprocessor->includeDepth--; // Pop include stack
        TM32ASM_DestroyLexer(lexer);
        TM32ASM_Destroy(includeFilename);
        TM32ASM_Destroy(fullPath);
        return false;
    }
    
    TM32ASM_LogDebug("Successfully tokenized included file '%s' "
                     "(%zu tokens)", 
                     fullPath, includedStream->tokenCount);
    
    // Pop include stack
    preprocessor->includeDepth--;
    
    // Insert all tokens from the included file directly into the output 
    // stream (without recursive preprocessing for now to avoid ownership 
    // issues)
    for (size_t i = 0; i < includedStream->tokenCount; ++i) {
        TM32ASM_Token* sourceToken = includedStream->tokens[i];
        if (sourceToken != NULL) {
            // Create a completely independent copy with duplicate lexeme
            char* lexemeCopy = TM32ASM_Create(strlen(sourceToken->lexeme) + 1,
                                               char);
            if (lexemeCopy == NULL) {
                TM32ASM_LogError("Failed to duplicate lexeme for included "
                                 "token");
                TM32ASM_DestroyLexer(lexer);
                TM32ASM_Destroy(includeFilename);
                TM32ASM_Destroy(fullPath);
                return false;
            }
            strcpy(lexemeCopy, sourceToken->lexeme);
            
            TM32ASM_Token* tokenCopy = TM32ASM_CreateToken(lexemeCopy, 
                                                            sourceToken->type);
            TM32ASM_Destroy(lexemeCopy); // CreateToken should make its own copy
            
            if (tokenCopy == NULL) {
                TM32ASM_LogError("Failed to copy token from included file");
                TM32ASM_DestroyLexer(lexer);
                TM32ASM_Destroy(includeFilename);
                TM32ASM_Destroy(fullPath);
                return false;
            }
            
            // Copy token metadata
            tokenCopy->param = sourceToken->param;
            tokenCopy->filename = sourceToken->filename;
            tokenCopy->line = sourceToken->line;
            
            // The tokenCopy is now owned by the outputStream
            if (TM32ASM_InsertToken(outputStream, tokenCopy) == NULL) {
                TM32ASM_LogError("Failed to insert token into output stream");
                TM32ASM_DestroyToken(tokenCopy);
                TM32ASM_DestroyLexer(lexer);
                TM32ASM_Destroy(includeFilename);
                TM32ASM_Destroy(fullPath);
                return false;
            }
        }
    }
    
    TM32ASM_LogDebug("Successfully included file '%s'", fullPath);
    
    // Clean up
    TM32ASM_DestroyLexer(lexer);
    TM32ASM_Destroy(includeFilename);
    TM32ASM_Destroy(fullPath);
    // Note: token and filenameToken are consumed from input stream,
    // their cleanup is handled by the stream itself
    
    return true;
}

static bool TM32ASM_HandleDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    // Conditional directives are always processed (they control processing state)
    if (strcmp(token->lexeme, ".if") == 0) {
        return TM32ASM_HandleIfDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".elseif") == 0 || strcmp(token->lexeme, ".elif") == 0) {
        return TM32ASM_HandleElseifDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".else") == 0) {
        return TM32ASM_HandleElseDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".endif") == 0 || strcmp(token->lexeme, ".endc") == 0) {
        return TM32ASM_HandleEndifDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // Repetition directives are also always processed (they control processing flow)
    if (strcmp(token->lexeme, ".repeat") == 0 || strcmp(token->lexeme, ".rept") == 0) {
        return TM32ASM_HandleRepeatDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".endrepeat") == 0 || strcmp(token->lexeme, ".endr") == 0) {
        return TM32ASM_HandleEndrepeatDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // While loop directives are also always processed (they control processing flow)
    if (strcmp(token->lexeme, ".while") == 0) {
        return TM32ASM_HandleWhileDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".endwhile") == 0 || strcmp(token->lexeme, ".endw") == 0) {
        return TM32ASM_HandleEndwhileDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // For loop directives are also always processed (they control processing flow)
    if (strcmp(token->lexeme, ".for") == 0) {
        return TM32ASM_HandleForDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".endfor") == 0 || strcmp(token->lexeme, ".endf") == 0) {
        return TM32ASM_HandleEndforDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // Other directives are only processed if we're in an active context
    if (!TM32ASM_ShouldProcessTokens(preprocessor)) {
        return true; // Skip processing but don't error
    }
    
    // Loop control directives
    if (strcmp(token->lexeme, ".continue") == 0) {
        return TM32ASM_HandleContinueDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".break") == 0) {
        return TM32ASM_HandleBreakDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // Debugging directives are only processed in active contexts
    if (strcmp(token->lexeme, ".warn") == 0) {
        return TM32ASM_HandleWarnDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".error") == 0) {
        return TM32ASM_HandleErrorDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".assert") == 0) {
        return TM32ASM_HandleAssertDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".file") == 0) {
        return TM32ASM_HandleFileDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".line") == 0) {
        return TM32ASM_HandleLineDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // Check for specific directives
    if (strcmp(token->lexeme, ".include") == 0) {
        return TM32ASM_HandleIncludeDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".let") == 0) {
        return TM32ASM_HandleLetDirective(preprocessor, inputStream, outputStream, token);
    } else if (strcmp(token->lexeme, ".const") == 0) {
        return TM32ASM_HandleConstDirective(preprocessor, inputStream, outputStream, token);
    }
    
    // For other directives, just pass through for now
    // (will be implemented in subsequent prompts)
    TM32ASM_Token* tokenCopy = TM32ASM_CreateToken(token->lexeme, token->type);
    if (tokenCopy == NULL) {
        TM32ASM_LogError("Failed to create token copy for directive");
        return false;
    }
    tokenCopy->param = token->param;
    tokenCopy->filename = token->filename;
    tokenCopy->line = token->line;
    
    TM32ASM_InsertToken(outputStream, tokenCopy);
    return true;
}

static bool TM32ASM_HandleSymbolSubstitution (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    // Look up the symbol
    TM32ASM_Symbol* symbol = TM32ASM_LookupSymbol(preprocessor, token->lexeme);
    if (symbol != NULL) {
        // Create a copy of the symbol's value token
        TM32ASM_Token* value_token = TM32ASM_CreateToken(symbol->value->lexeme, symbol->value->type);
        if (value_token == NULL) {
            TM32ASM_LogError("Failed to create token for symbol substitution");
            return false;
        }
        
        value_token->param = symbol->value->param;
        value_token->filename = token->filename;
        value_token->line = token->line;
        
        TM32ASM_InsertToken(outputStream, value_token);
        
        TM32ASM_LogDebug("Substituted symbol '%s' with value '%s'", 
                         symbol->name, symbol->value->lexeme);
    } else {
        // Not a symbol, pass through as-is - create a copy to avoid double-free
        TM32ASM_Token* tokenCopy = TM32ASM_CreateToken(token->lexeme, token->type);
        if (tokenCopy == NULL) {
            TM32ASM_LogError("Failed to create token copy for identifier");
            return false;
        }
        tokenCopy->param = token->param;
        tokenCopy->filename = token->filename;
        tokenCopy->line = token->line;
        
        TM32ASM_InsertToken(outputStream, tokenCopy);
    }
    
    return true;
}

TM32ASM_TokenStream* TM32ASM_ProcessTokens (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream
)
{
    if (preprocessor == NULL || inputStream == NULL) {
        return NULL;
    }
    
    // Create a new output stream
    TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
    if (outputStream == NULL) {
        return NULL;
    }
    
    // Process the input stream into the output stream
    if (!TM32ASM_ProcessTokenStream(preprocessor, inputStream, outputStream)) {
        TM32ASM_DestroyTokenStream(outputStream);
        return NULL;
    }
    
    return outputStream;
}

bool TM32ASM_AddIncludePath (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             path
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(path, false);
    
    // Grow array if needed
    if (preprocessor->includePathCount >= preprocessor->includePathCapacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->includePaths,
                               &preprocessor->includePathCapacity,
                               sizeof(const char*))) {
            TM32ASM_LogError("Failed to grow include paths array");
            return false;
        }
    }
    
    // Duplicate the path string
    char* pathCopy = TM32ASM_Create(strlen(path) + 1, char);
    if (pathCopy == NULL) {
        TM32ASM_LogError("Failed to duplicate include path: %s", path);
        return false;
    }
    strcpy(pathCopy, path);
    
    preprocessor->includePaths[preprocessor->includePathCount++] = pathCopy;
    TM32ASM_LogDebug("Added include path: %s", path);
    return true;
}

char* TM32ASM_FindIncludeFile (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filename,
    const char*             currentFile
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(filename, NULL);
    
    char* fullPath = NULL;
    struct stat fileStat;
    
    // First, try the current directory or relative to current file
    if (currentFile != NULL) {
        // Get directory of current file
        char* currentDir = TM32ASM_Create(strlen(currentFile) + 1, char);
        if (currentDir != NULL) {
            strcpy(currentDir, currentFile);
            char* lastSlash = strrchr(currentDir, '/');
            if (lastSlash != NULL) {
                *lastSlash = '\0';
                // Try relative to current file's directory
                size_t dirLen = strlen(currentDir);
                size_t fileLen = strlen(filename);
                fullPath = TM32ASM_Create(dirLen + 1 + fileLen + 1, char);
                if (fullPath != NULL) {
                    sprintf(fullPath, "%s/%s", currentDir, filename);
                    if (stat(fullPath, &fileStat) == 0 && 
                        S_ISREG(fileStat.st_mode)) {
                        TM32ASM_Destroy(currentDir);
                        return fullPath;
                    }
                    TM32ASM_Destroy(fullPath);
                    fullPath = NULL;
                }
            }
            TM32ASM_Destroy(currentDir);
        }
    }
    
    // Try current working directory
    if (stat(filename, &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
        fullPath = TM32ASM_Create(strlen(filename) + 1, char);
        if (fullPath != NULL) {
            strcpy(fullPath, filename);
        }
        return fullPath;
    }
    
    // Try each include path
    for (uint32_t i = 0; i < preprocessor->includePathCount; ++i) {
        size_t pathLen = strlen(preprocessor->includePaths[i]);
        size_t fileLen = strlen(filename);
        fullPath = TM32ASM_Create(pathLen + 1 + fileLen + 1, char);
        if (fullPath != NULL) {
            sprintf(fullPath, "%s/%s", 
                    preprocessor->includePaths[i], filename);
            if (stat(fullPath, &fileStat) == 0 && 
                S_ISREG(fileStat.st_mode)) {
                return fullPath;
            }
            TM32ASM_Destroy(fullPath);
            fullPath = NULL;
        }
    }
    
    return NULL;
}

static bool TM32ASM_IsFileAlreadyIncluded (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filepath
)
{
    for (uint32_t i = 0; i < preprocessor->includedFileCount; ++i) {
        if (strcmp(preprocessor->includedFiles[i], filepath) == 0) {
            return true;
        }
    }
    return false;
}

static bool TM32ASM_AddIncludedFile (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filepath
)
{
    // Grow array if needed
    if (preprocessor->includedFileCount >= preprocessor->includedFileCapacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->includedFiles,
                               &preprocessor->includedFileCapacity,
                               sizeof(const char*))) {
            TM32ASM_LogError("Failed to grow included files array");
            return false;
        }
    }
    
    // Duplicate the filepath
    char* filepathCopy = TM32ASM_Create(strlen(filepath) + 1, char);
    if (filepathCopy == NULL) {
        TM32ASM_LogError("Failed to duplicate included file path: %s", 
                         filepath);
        return false;
    }
    strcpy(filepathCopy, filepath);
    
    preprocessor->includedFiles[preprocessor->includedFileCount++] = filepathCopy;
    return true;
}

static bool TM32ASM_HandleLetDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .let directive at %s:%u", token->filename, token->line);
    
    // Get the next token, which should be an identifier (variable name)
    TM32ASM_Token* nameToken = TM32ASM_ConsumeNextToken(inputStream);
    if (nameToken == NULL) {
        TM32ASM_LogError("Expected variable name after .let directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    if (nameToken->type != TM32ASM_TT_IDENTIFIER) {
        TM32ASM_LogError("Expected identifier for variable name in .let directive at %s:%u, got %s", 
                         token->filename, token->line, nameToken->lexeme);
        return false;
    }
    
    // Get the next token, which should be an assignment operator
    TM32ASM_Token* assignToken = TM32ASM_ConsumeNextToken(inputStream);
    if (assignToken == NULL) {
        TM32ASM_LogError("Expected assignment operator after variable name in .let directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    if (assignToken->type != TM32ASM_TT_ASSIGN_EQUAL) {
        TM32ASM_LogError("Expected '=' after variable name in .let directive at %s:%u, got %s", 
                         token->filename, token->line, assignToken->lexeme);
        return false;
    }
    
    // Get the value token (for now, just support simple literals and identifiers)
    TM32ASM_Token* valueToken = TM32ASM_ConsumeNextToken(inputStream);
    if (valueToken == NULL) {
        TM32ASM_LogError("Expected value after '=' in .let directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    // Create a copy of the value token for storage
    TM32ASM_Token* storedValue = TM32ASM_CreateToken(valueToken->lexeme, valueToken->type);
    if (storedValue == NULL) {
        TM32ASM_LogError("Failed to create value token for .let directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    storedValue->param = valueToken->param;
    storedValue->filename = token->filename;
    storedValue->line = token->line;
    
    // Define or redefine the symbol
    if (!TM32ASM_DefineSymbol(preprocessor, nameToken->lexeme, storedValue, false, token->filename, token->line)) {
        TM32ASM_LogError("Failed to define variable '%s' in .let directive at %s:%u", 
                         nameToken->lexeme, token->filename, token->line);
        TM32ASM_DestroyToken(storedValue);
        return false;
    }
    
    TM32ASM_LogDebug("Defined variable '%s' with value '%s'", 
                     nameToken->lexeme, storedValue->lexeme);
    
    return true;
}

static bool TM32ASM_HandleConstDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .const directive at %s:%u", token->filename, token->line);
    
    // Get the next token, which should be an identifier (constant name)
    TM32ASM_Token* nameToken = TM32ASM_ConsumeNextToken(inputStream);
    if (nameToken == NULL) {
        TM32ASM_LogError("Expected constant name after .const directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    if (nameToken->type != TM32ASM_TT_IDENTIFIER) {
        TM32ASM_LogError("Expected identifier for constant name in .const directive at %s:%u, got %s", 
                         token->filename, token->line, nameToken->lexeme);
        return false;
    }
    
    // Check if constant already exists
    TM32ASM_Symbol* existingSymbol = TM32ASM_LookupSymbol(preprocessor, nameToken->lexeme);
    if (existingSymbol != NULL && existingSymbol->isConstant) {
        TM32ASM_LogError("Constant '%s' is already defined at %s:%u", 
                         nameToken->lexeme, token->filename, token->line);
        return false;
    }
    
    // Get the next token, which should be an assignment operator
    TM32ASM_Token* assignToken = TM32ASM_ConsumeNextToken(inputStream);
    if (assignToken == NULL) {
        TM32ASM_LogError("Expected assignment operator after constant name in .const directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    if (assignToken->type != TM32ASM_TT_ASSIGN_EQUAL) {
        TM32ASM_LogError("Expected '=' after constant name in .const directive at %s:%u, got %s", 
                         token->filename, token->line, assignToken->lexeme);
        return false;
    }
    
    // Get the value token (for now, just support simple literals and identifiers)
    TM32ASM_Token* valueToken = TM32ASM_ConsumeNextToken(inputStream);
    if (valueToken == NULL) {
        TM32ASM_LogError("Expected value after '=' in .const directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    // Create a copy of the value token for storage
    TM32ASM_Token* storedValue = TM32ASM_CreateToken(valueToken->lexeme, valueToken->type);
    if (storedValue == NULL) {
        TM32ASM_LogError("Failed to create value token for .const directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    storedValue->param = valueToken->param;
    storedValue->filename = token->filename;
    storedValue->line = token->line;
    
    TM32ASM_LogDebug("Created constant token: lexeme='%s', type=%d, param=%d", 
                     storedValue->lexeme, storedValue->type, storedValue->param);
    
    // Define the constant symbol (immutable)
    if (!TM32ASM_DefineSymbol(preprocessor, nameToken->lexeme, storedValue, true, token->filename, token->line)) {
        TM32ASM_LogError("Failed to define constant '%s' in .const directive at %s:%u", 
                         nameToken->lexeme, token->filename, token->line);
        TM32ASM_DestroyToken(storedValue);
        return false;
    }
    
    TM32ASM_LogDebug("Defined constant '%s' with value '%s'", 
                     nameToken->lexeme, storedValue->lexeme);
    
    return true;
}

static bool TM32ASM_HandleIfDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .if directive at %s:%u", token->filename, token->line);
    
    // Get the condition token (for now, just support simple values)
    TM32ASM_Token* conditionToken = TM32ASM_ConsumeNextToken(inputStream);
    if (conditionToken == NULL) {
        TM32ASM_LogError("Expected condition after .if directive at %s:%u", 
                         token->filename, token->line);
        return false;
    }
    
    // Evaluate simple condition (integer literals, identifiers)
    bool conditionTrue = false;
    
    if (conditionToken->type == TM32ASM_TT_DECIMAL ||
        conditionToken->type == TM32ASM_TT_HEXADECIMAL ||
        conditionToken->type == TM32ASM_TT_BINARY ||
        conditionToken->type == TM32ASM_TT_OCTAL) {
        // For numeric literals, non-zero is true
        uint32_t value = TM32ASM_GetIntegerLexeme(conditionToken);
        conditionTrue = (value != 0);
    } else if (conditionToken->type == TM32ASM_TT_IDENTIFIER) {
        // Look up the symbol
        TM32ASM_Symbol* symbol = TM32ASM_LookupSymbol(preprocessor, conditionToken->lexeme);
        TM32ASM_LogDebug("Looking up symbol '%s': %s", conditionToken->lexeme, symbol ? "found" : "not found");
        if (symbol != NULL) {
            TM32ASM_LogDebug("Symbol value type: %d, lexeme: '%s', param: %d", 
                             symbol->value->type, symbol->value->lexeme, symbol->value->param);
            // Check the symbol's value
            if (symbol->value->type == TM32ASM_TT_DECIMAL ||
                symbol->value->type == TM32ASM_TT_HEXADECIMAL ||
                symbol->value->type == TM32ASM_TT_BINARY ||
                symbol->value->type == TM32ASM_TT_OCTAL) {
                // Use TM32ASM_GetIntegerLexeme for proper numeric parsing
                uint32_t value = TM32ASM_GetIntegerLexeme(symbol->value);
                conditionTrue = (value != 0);
            } else {
                // For non-numeric types, consider them false
                conditionTrue = false;
            }
        } else {
            TM32ASM_LogError("Undefined symbol '%s' in .if condition at %s:%u",
                             conditionToken->lexeme, token->filename, token->line);
            return false;
        }
    } else {
        TM32ASM_LogError("Unsupported condition type in .if directive at %s:%u (got %d)", 
                         token->filename, token->line, conditionToken->type);
        return false;
    }
    
    // Push new conditional state
    // The condition is active if current context allows it AND the condition is true
    bool currentlyActive = TM32ASM_ShouldProcessTokens(preprocessor);
    bool newActive = currentlyActive && conditionTrue;
    
    if (!TM32ASM_PushConditional(preprocessor, newActive, token->filename, token->line)) {
        TM32ASM_LogError("Failed to push conditional state for .if directive at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_LogDebug("Condition evaluated to %s, block is %s",
                     conditionTrue ? "true" : "false",
                     newActive ? "active" : "inactive");
    
    return true;
}

static bool TM32ASM_HandleElseifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .elseif/.elif directive at %s:%u", token->filename, token->line);
    
    // Check if we're in a conditional block
    if (preprocessor->conditionalCount == 0) {
        TM32ASM_LogError(".elseif/.elif directive without matching .if at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_ConditionalState* current = &preprocessor->conditionals[preprocessor->conditionalCount - 1];
    
    // Check if we're already in an else block
    if (current->inElse) {
        TM32ASM_LogError(".elseif/.elif directive after .else at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    // Only evaluate condition if no previous branch has been taken
    bool conditionTrue = false;
    if (!current->taken) {
        // Get the condition token (for now, just support simple values)
        TM32ASM_Token* conditionToken = TM32ASM_ConsumeNextToken(inputStream);
        if (conditionToken == NULL) {
            TM32ASM_LogError("Expected condition after .elseif/.elif directive at %s:%u", 
                             token->filename, token->line);
            return false;
        }
        
        // Evaluate simple condition (integer literals, identifiers)
        if (conditionToken->type == TM32ASM_TT_DECIMAL ||
            conditionToken->type == TM32ASM_TT_HEXADECIMAL ||
            conditionToken->type == TM32ASM_TT_BINARY ||
            conditionToken->type == TM32ASM_TT_OCTAL) {
            // For numeric literals, non-zero is true
            conditionTrue = (conditionToken->param != 0);
        } else if (conditionToken->type == TM32ASM_TT_IDENTIFIER) {
            // Look up the symbol
            TM32ASM_Symbol* symbol = TM32ASM_LookupSymbol(preprocessor, conditionToken->lexeme);
            if (symbol != NULL) {
                // Check the symbol's value
                if (symbol->value->type == TM32ASM_TT_DECIMAL ||
                    symbol->value->type == TM32ASM_TT_HEXADECIMAL ||
                    symbol->value->type == TM32ASM_TT_BINARY ||
                    symbol->value->type == TM32ASM_TT_OCTAL) {
                    // Use param if available, otherwise parse the lexeme
                    int32_t value = symbol->value->param;
                    if (value == 0 && symbol->value->lexeme != NULL) {
                        // Try to parse the lexeme as a fallback
                        value = atoi(symbol->value->lexeme);
                    }
                    conditionTrue = (value != 0);
                }
            } else {
                TM32ASM_LogError("Undefined symbol '%s' in .elseif/.elif condition at %s:%u",
                                 conditionToken->lexeme, token->filename, token->line);
                return false;
            }
        } else {
            TM32ASM_LogError("Unsupported condition type in .elseif/.elif directive at %s:%u", 
                             token->filename, token->line);
            return false;
        }
    }
    
    // Update conditional state
    // Only activate if no previous branch has been taken AND condition is true
    // Check if parent context allows processing (for nested conditionals)
    bool parentActive = true;
    if (preprocessor->conditionalCount > 1) {
        // Temporarily remove current conditional to check parent context
        preprocessor->conditionalCount--;
        parentActive = TM32ASM_ShouldProcessTokens(preprocessor);
        preprocessor->conditionalCount++;
    }
    
    current->active = parentActive && !current->taken && conditionTrue;
    if (conditionTrue && !current->taken) {
        current->taken = true;
    }
    
    TM32ASM_LogDebug("Elseif condition evaluated to %s, block is %s",
                     conditionTrue ? "true" : "false",
                     current->active ? "active" : "inactive");
    
    return true;
}

static bool TM32ASM_HandleElseDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .else directive at %s:%u", token->filename, token->line);
    
    // Check if we're in a conditional block
    if (preprocessor->conditionalCount == 0) {
        TM32ASM_LogError(".else directive without matching .if at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_ConditionalState* current = &preprocessor->conditionals[preprocessor->conditionalCount - 1];
    
    // Check if we're already in an else block
    if (current->inElse) {
        TM32ASM_LogError("Multiple .else directives in same conditional block at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    // Update conditional state
    // Only activate if no previous branch has been taken
    // Check if parent context allows processing (for nested conditionals)
    bool parentActive = true;
    if (preprocessor->conditionalCount > 1) {
        // Temporarily remove current conditional to check parent context
        preprocessor->conditionalCount--;
        parentActive = TM32ASM_ShouldProcessTokens(preprocessor);
        preprocessor->conditionalCount++;
    }
    
    current->active = parentActive && !current->taken;
    current->inElse = true;
    if (!current->taken) {
        current->taken = true;
    }
    
    TM32ASM_LogDebug("Else block is %s", current->active ? "active" : "inactive");
    
    return true;
}

static bool TM32ASM_HandleEndifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .endif/.endc directive at %s:%u", token->filename, token->line);
    
    // Check if we're in a conditional block
    if (preprocessor->conditionalCount == 0) {
        TM32ASM_LogError(".endif/.endc directive without matching .if at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    // Pop the conditional state
    if (!TM32ASM_PopConditional(preprocessor)) {
        TM32ASM_LogError("Failed to pop conditional state for .endif/.endc directive at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_LogDebug("Conditional block ended");
    
    return true;
}

static bool TM32ASM_HandleRepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .repeat/.rept directive at %s:%u", token->filename, token->line);
    
    // Get the repeat count token
    TM32ASM_Token* countToken = TM32ASM_ConsumeNextToken(inputStream);
    if (countToken == NULL) {
        TM32ASM_LogError(".repeat/.rept directive missing count argument at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    // Parse the repeat count
    uint32_t repeatCount = 0;
    if (countToken->type == TM32ASM_TT_DECIMAL || 
        countToken->type == TM32ASM_TT_BINARY ||
        countToken->type == TM32ASM_TT_OCTAL ||
        countToken->type == TM32ASM_TT_HEXADECIMAL) {
        // Parse integer directly
        repeatCount = (uint32_t)strtoul(countToken->lexeme, NULL, 0);
    } else if (countToken->type == TM32ASM_TT_IDENTIFIER) {
        // Try to substitute with a symbol
        TM32ASM_Symbol* symbol = TM32ASM_LookupSymbol(preprocessor, countToken->lexeme);
        if (symbol != NULL && (symbol->value->type == TM32ASM_TT_DECIMAL ||
                               symbol->value->type == TM32ASM_TT_BINARY ||
                               symbol->value->type == TM32ASM_TT_OCTAL ||
                               symbol->value->type == TM32ASM_TT_HEXADECIMAL)) {
            repeatCount = (uint32_t)strtoul(symbol->value->lexeme, NULL, 0);
        } else {
            TM32ASM_LogError(".repeat/.rept directive count must be an integer or integer symbol at %s:%u",
                             token->filename, token->line);
            return false;
        }
    } else {
        TM32ASM_LogError(".repeat/.rept directive count must be an integer or integer symbol at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    if (repeatCount == 0) {
        TM32ASM_LogError(".repeat/.rept directive count must be greater than 0 at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_LogDebug("Repeat count: %u", repeatCount);
    
    // Create a new token stream to collect the repeat body
    TM32ASM_TokenStream* bodyStream = TM32ASM_CreateTokenStream();
    if (bodyStream == NULL) {
        TM32ASM_LogError("Failed to create token stream for repeat body");
        return false;
    }
    
    // Collect tokens until we find .endrepeat/.endr
    uint32_t nestedRepeats = 0;
    TM32ASM_Token* bodyToken;
    
    while ((bodyToken = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        if (bodyToken->type == TM32ASM_TT_DIRECTIVE) {
            if (strcmp(bodyToken->lexeme, ".repeat") == 0 || strcmp(bodyToken->lexeme, ".rept") == 0) {
                nestedRepeats++;
            } else if (strcmp(bodyToken->lexeme, ".endrepeat") == 0 || strcmp(bodyToken->lexeme, ".endr") == 0) {
                if (nestedRepeats == 0) {
                    // This is our matching .endrepeat/.endr
                    break;
                }
                nestedRepeats--;
            }
        }
        
        // Add token to body (create a copy to avoid double-free)
        TM32ASM_Token* tokenCopy = TM32ASM_CreateToken(bodyToken->lexeme, bodyToken->type);
        if (tokenCopy == NULL) {
            TM32ASM_LogError("Failed to create token copy for repeat body");
            TM32ASM_DestroyTokenStream(bodyStream);
            return false;
        }
        tokenCopy->param = bodyToken->param;
        tokenCopy->filename = bodyToken->filename;
        tokenCopy->line = bodyToken->line;
        
        TM32ASM_InsertToken(bodyStream, tokenCopy);
    }
    
    if (bodyToken == NULL) {
        TM32ASM_LogError(".repeat/.rept directive without matching .endrepeat/.endr at %s:%u",
                         token->filename, token->line);
        TM32ASM_DestroyTokenStream(bodyStream);
        return false;
    }
    
    TM32ASM_LogDebug("Collected %zu tokens for repeat body", bodyStream->tokenCount);
    
    // Expand the repeat body the specified number of times
    for (uint32_t i = 0; i < repeatCount; i++) {
        // Reset the body stream's read pointer
        bodyStream->tokenReadPointer = 0;
        
        // Process each token in the body and add to output
        TM32ASM_Token* repeatToken;
        while ((repeatToken = TM32ASM_ConsumeNextToken(bodyStream)) != NULL) {
            // Create a copy of the token for the output stream
            TM32ASM_Token* outputToken = TM32ASM_CreateToken(repeatToken->lexeme, repeatToken->type);
            if (outputToken == NULL) {
                TM32ASM_LogError("Failed to create token copy for repeat expansion");
                TM32ASM_DestroyTokenStream(bodyStream);
                return false;
            }
            outputToken->param = repeatToken->param;
            outputToken->filename = repeatToken->filename;
            outputToken->line = repeatToken->line;
            
            TM32ASM_InsertToken(outputStream, outputToken);
        }
    }
    
    TM32ASM_LogDebug("Expanded repeat body %u times", repeatCount);
    
    // Clean up the body stream
    TM32ASM_DestroyTokenStream(bodyStream);
    
    return true;
}

static bool TM32ASM_HandleEndrepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // This should not be reached if the repeat directive is handled correctly,
    // but we need it for error checking
    TM32ASM_LogError(".endrepeat/.endr directive without matching .repeat/.rept at %s:%u",
                     token->filename, token->line);
    return false;
}

/**
 * @brief   Evaluates a condition token stream for while loops.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   condition      The condition token stream to evaluate.
 * 
 * @return  `true` if the condition evaluates to true, `false` otherwise.
 */
static bool TM32ASM_EvaluateCondition (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    condition
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(condition, false);
    
    // Reset the condition stream's read pointer
    condition->tokenReadPointer = 0;
    
    // For simplicity, we'll implement basic condition evaluation
    // This handles single symbol/literal comparisons
    TM32ASM_Token* firstToken = TM32ASM_ConsumeNextToken(condition);
    if (firstToken == NULL) {
        return false; // Empty condition is false
    }
    
    // If it's a numeric literal, check if it's non-zero
    if (firstToken->type == TM32ASM_TT_DECIMAL ||
        firstToken->type == TM32ASM_TT_BINARY ||
        firstToken->type == TM32ASM_TT_OCTAL ||
        firstToken->type == TM32ASM_TT_HEXADECIMAL) {
        uint32_t value = (uint32_t)strtoul(firstToken->lexeme, NULL, 0);
        return value != 0;
    }
    
    // If it's an identifier, try to substitute with a symbol
    if (firstToken->type == TM32ASM_TT_IDENTIFIER) {
        TM32ASM_Symbol* symbol = TM32ASM_LookupSymbol(preprocessor, firstToken->lexeme);
        if (symbol != NULL) {
            // Recursively evaluate the symbol's value
            TM32ASM_TokenStream* symbolStream = TM32ASM_CreateTokenStream();
            if (symbolStream == NULL) {
                return false;
            }
            
            // Create a copy of the symbol's value token
            TM32ASM_Token* symbolToken = TM32ASM_CreateToken(symbol->value->lexeme, symbol->value->type);
            if (symbolToken == NULL) {
                TM32ASM_DestroyTokenStream(symbolStream);
                return false;
            }
            symbolToken->param = symbol->value->param;
            symbolToken->filename = symbol->value->filename;
            symbolToken->line = symbol->value->line;
            
            TM32ASM_InsertToken(symbolStream, symbolToken);
            
            bool result = TM32ASM_EvaluateCondition(preprocessor, symbolStream);
            TM32ASM_DestroyTokenStream(symbolStream);
            return result;
        }
    }
    
    // Default: unknown condition types are false
    return false;
}

static bool TM32ASM_HandleWhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .while directive at %s:%u", token->filename, token->line);
    
    // Create a token stream to collect the condition
    TM32ASM_TokenStream* conditionStream = TM32ASM_CreateTokenStream();
    if (conditionStream == NULL) {
        TM32ASM_LogError("Failed to create token stream for while condition");
        return false;
    }
    
    // Collect condition tokens until end of line or start of body
    TM32ASM_Token* conditionToken;
    while ((conditionToken = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        // For simplicity, we'll stop at the next line or when we encounter certain tokens
        // In a full implementation, this would be more sophisticated
        if (conditionToken->line != token->line) {
            // Put the token back and break
            inputStream->tokenReadPointer--;
            break;
        }
        
        // Add token to condition (create a copy)
        TM32ASM_Token* tokenCopy = TM32ASM_CreateToken(conditionToken->lexeme, conditionToken->type);
        if (tokenCopy == NULL) {
            TM32ASM_LogError("Failed to create token copy for while condition");
            TM32ASM_DestroyTokenStream(conditionStream);
            return false;
        }
        tokenCopy->param = conditionToken->param;
        tokenCopy->filename = conditionToken->filename;
        tokenCopy->line = conditionToken->line;
        
        TM32ASM_InsertToken(conditionStream, tokenCopy);
    }
    
    if (conditionStream->tokenCount == 0) {
        TM32ASM_LogError(".while directive missing condition at %s:%u",
                         token->filename, token->line);
        TM32ASM_DestroyTokenStream(conditionStream);
        return false;
    }
    
    TM32ASM_LogDebug("Collected %zu tokens for while condition", conditionStream->tokenCount);
    
    // Create a new token stream to collect the while body
    TM32ASM_TokenStream* bodyStream = TM32ASM_CreateTokenStream();
    if (bodyStream == NULL) {
        TM32ASM_LogError("Failed to create token stream for while body");
        TM32ASM_DestroyTokenStream(conditionStream);
        return false;
    }
    
    // Collect tokens until we find .endwhile/.endw
    uint32_t nestedWhiles = 0;
    TM32ASM_Token* bodyToken;
    
    while ((bodyToken = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        if (bodyToken->type == TM32ASM_TT_DIRECTIVE) {
            if (strcmp(bodyToken->lexeme, ".while") == 0) {
                nestedWhiles++;
            } else if (strcmp(bodyToken->lexeme, ".endwhile") == 0 || strcmp(bodyToken->lexeme, ".endw") == 0) {
                if (nestedWhiles == 0) {
                    // This is our matching .endwhile/.endw
                    break;
                }
                nestedWhiles--;
            }
        }
        
        // Add token to body (create a copy)
        TM32ASM_Token* tokenCopy = TM32ASM_CreateToken(bodyToken->lexeme, bodyToken->type);
        if (tokenCopy == NULL) {
            TM32ASM_LogError("Failed to create token copy for while body");
            TM32ASM_DestroyTokenStream(conditionStream);
            TM32ASM_DestroyTokenStream(bodyStream);
            return false;
        }
        tokenCopy->param = bodyToken->param;
        tokenCopy->filename = bodyToken->filename;
        tokenCopy->line = bodyToken->line;
        
        TM32ASM_InsertToken(bodyStream, tokenCopy);
    }
    
    if (bodyToken == NULL) {
        TM32ASM_LogError(".while directive without matching .endwhile/.endw at %s:%u",
                         token->filename, token->line);
        TM32ASM_DestroyTokenStream(conditionStream);
        TM32ASM_DestroyTokenStream(bodyStream);
        return false;
    }
    
    TM32ASM_LogDebug("Collected %zu tokens for while body", bodyStream->tokenCount);
    
    // Execute the while loop with safety limit
    uint32_t iteration = 0;
    while (iteration < MAX_WHILE_ITERATIONS) {
        // Evaluate the condition
        if (!TM32ASM_EvaluateCondition(preprocessor, conditionStream)) {
            // Condition is false, exit loop
            break;
        }
        
        TM32ASM_LogDebug("While condition true, executing iteration %u", iteration);
        
        // Reset the body stream's read pointer
        bodyStream->tokenReadPointer = 0;
        
        // Create a temporary output stream for this iteration
        TM32ASM_TokenStream* iterationOutput = TM32ASM_CreateTokenStream();
        if (iterationOutput == NULL) {
            TM32ASM_LogError("Failed to create iteration output stream");
            TM32ASM_DestroyTokenStream(conditionStream);
            TM32ASM_DestroyTokenStream(bodyStream);
            return false;
        }
        
        // Increment while count to maintain proper loop context during recursive processing
        preprocessor->whileCount++;
        
        // Process the body tokens through the preprocessor to handle directives
        bool processSuccess = TM32ASM_ProcessTokenStream(preprocessor, bodyStream, iterationOutput);
        
        // Decrement while count after processing
        preprocessor->whileCount--;
        
        if (!processSuccess) {
            TM32ASM_LogError("Failed to process while loop body tokens");
            TM32ASM_DestroyTokenStream(conditionStream);
            TM32ASM_DestroyTokenStream(bodyStream);
            TM32ASM_DestroyTokenStream(iterationOutput);
            return false;
        }
        
        // Check for loop control directives
        if (preprocessor->loopControl == TM32ASM_LOOP_CONTROL_BREAK) {
            TM32ASM_LogDebug("While loop terminated early by .break directive");
            preprocessor->loopControl = TM32ASM_LOOP_CONTROL_NONE; // Reset state
            TM32ASM_DestroyTokenStream(iterationOutput);
            break; // Exit the while loop
        } else if (preprocessor->loopControl == TM32ASM_LOOP_CONTROL_CONTINUE) {
            TM32ASM_LogDebug("While loop iteration skipped by .continue directive");
            preprocessor->loopControl = TM32ASM_LOOP_CONTROL_NONE; // Reset state
            TM32ASM_DestroyTokenStream(iterationOutput);
            iteration++;
            continue; // Skip to next iteration
        }
        
        // Copy processed tokens to the main output stream
        iterationOutput->tokenReadPointer = 0;
        TM32ASM_Token* processedToken;
        while ((processedToken = TM32ASM_ConsumeNextToken(iterationOutput)) != NULL) {
            // Create a copy of the processed token
            TM32ASM_Token* outputToken = TM32ASM_CreateToken(processedToken->lexeme, processedToken->type);
            if (outputToken == NULL) {
                TM32ASM_LogError("Failed to create token copy for while expansion");
                TM32ASM_DestroyTokenStream(conditionStream);
                TM32ASM_DestroyTokenStream(bodyStream);
                TM32ASM_DestroyTokenStream(iterationOutput);
                return false;
            }
            outputToken->param = processedToken->param;
            outputToken->filename = processedToken->filename;
            outputToken->line = processedToken->line;
            
            TM32ASM_InsertToken(outputStream, outputToken);
        }
        
        // Clean up iteration output
        TM32ASM_DestroyTokenStream(iterationOutput);
        
        iteration++;
    }
    
    if (iteration >= MAX_WHILE_ITERATIONS) {
        TM32ASM_LogError(".while loop exceeded maximum iterations (%u) at %s:%u - possible infinite loop",
                         MAX_WHILE_ITERATIONS, token->filename, token->line);
        TM32ASM_DestroyTokenStream(conditionStream);
        TM32ASM_DestroyTokenStream(bodyStream);
        return false;
    }
    
    TM32ASM_LogDebug("While loop completed after %u iterations", iteration);
    
    // Clean up the streams
    TM32ASM_DestroyTokenStream(conditionStream);
    TM32ASM_DestroyTokenStream(bodyStream);
    
    return true;
}

static bool TM32ASM_HandleEndwhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // This should not be reached if the while directive is handled correctly,
    // but we need it for error checking
    TM32ASM_LogError(".endwhile/.endw directive without matching .while at %s:%u",
                     token->filename, token->line);
    return false;
}

static bool TM32ASM_HandleForDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    TM32ASM_LogDebug("Processing .for directive at %s:%u", token->filename, token->line);
    
    // Expect syntax: .for variable start end [step]
    TM32ASM_Token* varToken = TM32ASM_ConsumeNextToken(inputStream);
    if (varToken == NULL || varToken->type != TM32ASM_TT_IDENTIFIER) {
        TM32ASM_LogError(".for directive missing variable name at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_Token* startToken = TM32ASM_ConsumeNextToken(inputStream);
    if (startToken == NULL || (startToken->type != TM32ASM_TT_DECIMAL && 
                               startToken->type != TM32ASM_TT_HEXADECIMAL &&
                               startToken->type != TM32ASM_TT_BINARY &&
                               startToken->type != TM32ASM_TT_OCTAL)) {
        TM32ASM_LogError(".for directive missing start value at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_Token* endToken = TM32ASM_ConsumeNextToken(inputStream);
    if (endToken == NULL || (endToken->type != TM32ASM_TT_DECIMAL && 
                             endToken->type != TM32ASM_TT_HEXADECIMAL &&
                             endToken->type != TM32ASM_TT_BINARY &&
                             endToken->type != TM32ASM_TT_OCTAL)) {
        TM32ASM_LogError(".for directive missing end value at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    // Optional step parameter (defaults to 1)
    int32_t step = 1;
    TM32ASM_Token* stepToken = TM32ASM_PeekNextToken(inputStream, 0);
    if (stepToken != NULL && (stepToken->type == TM32ASM_TT_DECIMAL ||
                              stepToken->type == TM32ASM_TT_HEXADECIMAL ||
                              stepToken->type == TM32ASM_TT_BINARY ||
                              stepToken->type == TM32ASM_TT_OCTAL)) {
        TM32ASM_ConsumeNextToken(inputStream); // consume the step token
        step = (int32_t)strtol(stepToken->lexeme, NULL, 0);
        if (step == 0) {
            TM32ASM_LogError(".for directive step cannot be zero at %s:%u",
                             token->filename, token->line);
            return false;
        }
    }
    
    // Create a token stream to collect the body
    TM32ASM_TokenStream* bodyStream = TM32ASM_CreateTokenStream();
    if (bodyStream == NULL) {
        TM32ASM_LogError("Failed to create token stream for for loop body");
        return false;
    }
    
    // Collect body tokens until matching .endfor/.endf
    TM32ASM_Token* bodyToken;
    uint32_t nestedCount = 0;
    
    while ((bodyToken = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        if (strcmp(bodyToken->lexeme, ".for") == 0) {
            nestedCount++;
        } else if (strcmp(bodyToken->lexeme, ".endfor") == 0 || strcmp(bodyToken->lexeme, ".endf") == 0) {
            if (nestedCount == 0) {
                // This is our matching .endfor/.endf
                break;
            } else {
                nestedCount--;
            }
        }
        
        // Create a copy of the token for the body stream
        TM32ASM_Token* bodyCopy = TM32ASM_CreateToken(bodyToken->lexeme, bodyToken->type);
        if (bodyCopy == NULL) {
            TM32ASM_LogError("Failed to create token copy for for loop body");
            TM32ASM_DestroyTokenStream(bodyStream);
            return false;
        }
        bodyCopy->param = bodyToken->param;
        bodyCopy->filename = bodyToken->filename;
        bodyCopy->line = bodyToken->line;
        
        if (!TM32ASM_InsertToken(bodyStream, bodyCopy)) {
            TM32ASM_LogError("Failed to append token to for loop body");
            TM32ASM_DestroyToken(bodyCopy);
            TM32ASM_DestroyTokenStream(bodyStream);
            return false;
        }
    }
    
    if (bodyToken == NULL) {
        TM32ASM_LogError(".for directive without matching .endfor/.endf at %s:%u",
                         token->filename, token->line);
        TM32ASM_DestroyTokenStream(bodyStream);
        return false;
    }
    
    TM32ASM_LogDebug("Collected %zu tokens for for loop body", bodyStream->tokenCount);
    
    // Create for loop state
    if (preprocessor->forCount >= preprocessor->forCapacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->fors,
                               &preprocessor->forCapacity,
                               sizeof(TM32ASM_ForState))) {
            TM32ASM_LogError("Failed to expand for loop stack");
            TM32ASM_DestroyTokenStream(bodyStream);
            return false;
        }
    }
    
    TM32ASM_ForState* forState = &preprocessor->fors[preprocessor->forCount++];
    forState->body = bodyStream;
    
    // Copy variable name
    forState->variable = TM32ASM_Create(strlen(varToken->lexeme) + 1, char);
    if (forState->variable == NULL) {
        TM32ASM_LogError("Failed to allocate memory for for loop variable name");
        TM32ASM_DestroyTokenStream(bodyStream);
        preprocessor->forCount--;
        return false;
    }
    strcpy(forState->variable, varToken->lexeme);
    
    forState->start = (int32_t)strtol(startToken->lexeme, NULL, 0);
    forState->end = (int32_t)strtol(endToken->lexeme, NULL, 0);
    forState->step = step;
    forState->current = forState->start;
    forState->line = token->line;
    forState->filename = token->filename;
    
    TM32ASM_LogDebug("For loop range: start=%d, end=%d, step=%d", 
                     forState->start, forState->end, forState->step);
    
    // Execute the for loop
    bool ascending = (step > 0);
    while ((ascending && forState->current <= forState->end) ||
           (!ascending && forState->current >= forState->end)) {
        
        TM32ASM_LogDebug("For loop iteration: %s = %d", forState->variable, forState->current);
        
        // Set the loop variable value
        TM32ASM_Symbol* symbol = TM32ASM_LookupSymbol(preprocessor, forState->variable);
        if (symbol != NULL) {
            // Update existing symbol's value
            TM32ASM_Destroy(symbol->value->lexeme);
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", forState->current);
            symbol->value->lexeme = TM32ASM_Create(strlen(buffer) + 1, char);
            if (symbol->value->lexeme != NULL) {
                strcpy(symbol->value->lexeme, buffer);
            }
            symbol->value->param = forState->current;
        } else {
            // Create new symbol
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", forState->current);
            TM32ASM_Token* valueToken = TM32ASM_CreateToken(buffer, TM32ASM_TT_DECIMAL);
            if (valueToken == NULL) {
                TM32ASM_LogError("Failed to create value token for for loop variable");
                break;
            }
            valueToken->param = forState->current;
            
            if (!TM32ASM_DefineSymbol(preprocessor, forState->variable, valueToken, false,
                                      forState->filename, forState->line)) {
                TM32ASM_LogError("Failed to define for loop variable");
                TM32ASM_DestroyToken(valueToken);
                break;
            }
        }
        
        // Process the body
        bodyStream->tokenReadPointer = 0; // Reset read pointer
        
        // Increment for count to maintain proper loop context during recursive processing
        preprocessor->forCount++;
        
        bool processSuccess = TM32ASM_ProcessTokenStream(preprocessor, bodyStream, outputStream);
        
        // Decrement for count after processing
        preprocessor->forCount--;
        
        if (!processSuccess) {
            TM32ASM_LogError("Failed to process for loop body");
            break;
        }
        
        // Check for loop control directives
        if (preprocessor->loopControl == TM32ASM_LOOP_CONTROL_BREAK) {
            TM32ASM_LogDebug("For loop terminated early by .break directive");
            preprocessor->loopControl = TM32ASM_LOOP_CONTROL_NONE; // Reset state
            break; // Exit the for loop
        } else if (preprocessor->loopControl == TM32ASM_LOOP_CONTROL_CONTINUE) {
            TM32ASM_LogDebug("For loop iteration skipped by .continue directive");
            preprocessor->loopControl = TM32ASM_LOOP_CONTROL_NONE; // Reset state
            // Continue to next iteration (increment happens below)
        }
        
        // Increment/decrement the current value
        forState->current += step;
    }
    
    // Clean up
    preprocessor->forCount--;
    TM32ASM_DestroyTokenStream(forState->body);
    TM32ASM_Destroy(forState->variable);
    
    TM32ASM_LogDebug("For loop completed");
    return true;
}

static bool TM32ASM_HandleEndforDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // This should not be reached if the for directive is handled correctly,
    // but we need it for error checking
    TM32ASM_LogError(".endfor/.endf directive without matching .for at %s:%u",
                     token->filename, token->line);
    return false;
}

static bool TM32ASM_HandleContinueDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Check if we're inside a loop (while or for)
    if (preprocessor->whileCount == 0 && preprocessor->forCount == 0) {
        TM32ASM_LogError(".continue directive outside of any loop at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_LogDebug(".continue directive encountered at %s:%u", token->filename, token->line);
    
    // Set the loop control state to continue
    preprocessor->loopControl = TM32ASM_LOOP_CONTROL_CONTINUE;
    
    return true;
}

static bool TM32ASM_HandleBreakDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Check if we're inside a loop (while or for)
    if (preprocessor->whileCount == 0 && preprocessor->forCount == 0) {
        TM32ASM_LogError(".break directive outside of any loop at %s:%u",
                         token->filename, token->line);
        return false;
    }
    
    TM32ASM_LogDebug(".break directive encountered at %s:%u", token->filename, token->line);
    
    // Set the loop control state to break
    preprocessor->loopControl = TM32ASM_LOOP_CONTROL_BREAK;
    
    return true;
}

static bool TM32ASM_HandleWarnDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Collect all remaining tokens on the line as the warning message
    TM32ASM_Token* messageToken;
    char messageBuffer[4096] = {0}; // Buffer for concatenated message
    size_t messageLen = 0;
    
    while ((messageToken = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        // Stop at end of line or if we encounter another directive
        if (messageToken->type == TM32ASM_TT_DIRECTIVE) {
            // Put the directive token back for later processing
            inputStream->tokenReadPointer--;
            break;
        }
        
        // Append token to message (with space separator)
        if (messageLen > 0 && messageLen < sizeof(messageBuffer) - 2) {
            messageBuffer[messageLen++] = ' ';
        }
        
        size_t tokenLen = strlen(messageToken->lexeme);
        if (messageLen + tokenLen < sizeof(messageBuffer) - 1) {
            strcpy(messageBuffer + messageLen, messageToken->lexeme);
            messageLen += tokenLen;
        }
    }
    
    // Emit the warning
    if (messageLen > 0) {
        TM32ASM_LogWarn("User warning at %s:%u: %s", token->filename, token->line, messageBuffer);
    } else {
        TM32ASM_LogWarn("User warning at %s:%u", token->filename, token->line);
    }
    
    return true;
}

static bool TM32ASM_HandleErrorDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Collect all remaining tokens on the line as the error message
    TM32ASM_Token* messageToken;
    char messageBuffer[4096] = {0}; // Buffer for concatenated message
    size_t messageLen = 0;
    
    while ((messageToken = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        // Stop at end of line or if we encounter another directive
        if (messageToken->type == TM32ASM_TT_DIRECTIVE) {
            // Put the directive token back for later processing
            inputStream->tokenReadPointer--;
            break;
        }
        
        // Append token to message (with space separator)
        if (messageLen > 0 && messageLen < sizeof(messageBuffer) - 2) {
            messageBuffer[messageLen++] = ' ';
        }
        
        size_t tokenLen = strlen(messageToken->lexeme);
        if (messageLen + tokenLen < sizeof(messageBuffer) - 1) {
            strcpy(messageBuffer + messageLen, messageToken->lexeme);
            messageLen += tokenLen;
        }
    }
    
    // Emit the error and halt assembly
    if (messageLen > 0) {
        TM32ASM_LogError("User error at %s:%u: %s", token->filename, token->line, messageBuffer);
    } else {
        TM32ASM_LogError("User error at %s:%u", token->filename, token->line);
    }
    
    return false; // Return false to halt assembly
}

static bool TM32ASM_HandleAssertDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Get the condition expression - for now, we'll do a simple check
    // A full implementation would evaluate the expression properly
    TM32ASM_Token* conditionToken = TM32ASM_ConsumeNextToken(inputStream);
    if (conditionToken == NULL) {
        TM32ASM_LogError("Expected condition after .assert directive at %s:%u", token->filename, token->line);
        return false;
    }
    
    // For now, implement a simple assertion that checks if a symbol is defined and non-zero
    bool assertionPassed = false;
    
    if (conditionToken->type == TM32ASM_TT_IDENTIFIER) {
        TM32ASM_Symbol* symbol = TM32ASM_LookupSymbol(preprocessor, conditionToken->lexeme);
        if (symbol != NULL && symbol->value != NULL) {
            if (symbol->value->type == TM32ASM_TT_DECIMAL ||
                symbol->value->type == TM32ASM_TT_HEXADECIMAL ||
                symbol->value->type == TM32ASM_TT_BINARY ||
                symbol->value->type == TM32ASM_TT_OCTAL) {
                uint32_t value = TM32ASM_GetIntegerLexeme(symbol->value);
                assertionPassed = (value != 0);
            }
        }
    } else if (conditionToken->type == TM32ASM_TT_DECIMAL ||
               conditionToken->type == TM32ASM_TT_HEXADECIMAL ||
               conditionToken->type == TM32ASM_TT_BINARY ||
               conditionToken->type == TM32ASM_TT_OCTAL) {
        // Parse the numeric value using proper function
        uint32_t value = TM32ASM_GetIntegerLexeme(conditionToken);
        assertionPassed = (value != 0);
    } else {
        // For other token types, assume true for now
        assertionPassed = true;
    }
    
    // Collect optional message
    TM32ASM_Token* messageToken;
    char messageBuffer[4096] = {0};
    size_t messageLen = 0;
    
    while ((messageToken = TM32ASM_ConsumeNextToken(inputStream)) != NULL) {
        if (messageToken->type == TM32ASM_TT_DIRECTIVE) {
            inputStream->tokenReadPointer--;
            break;
        }
        
        if (messageLen > 0 && messageLen < sizeof(messageBuffer) - 2) {
            messageBuffer[messageLen++] = ' ';
        }
        
        size_t tokenLen = strlen(messageToken->lexeme);
        if (messageLen + tokenLen < sizeof(messageBuffer) - 1) {
            strcpy(messageBuffer + messageLen, messageToken->lexeme);
            messageLen += tokenLen;
        }
    }
    
    if (!assertionPassed) {
        if (messageLen > 0) {
            TM32ASM_LogError("Assertion failed at %s:%u: %s", token->filename, token->line, messageBuffer);
        } else {
            TM32ASM_LogError("Assertion failed at %s:%u: %s", token->filename, token->line, conditionToken->lexeme);
        }
        return false; // Halt assembly on assertion failure
    }
    
    TM32ASM_LogDebug("Assertion passed at %s:%u", token->filename, token->line);
    return true;
}

static bool TM32ASM_HandleFileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Get the filename token
    TM32ASM_Token* filenameToken = TM32ASM_ConsumeNextToken(inputStream);
    if (filenameToken == NULL) {
        TM32ASM_LogError("Expected filename after .file directive at %s:%u", token->filename, token->line);
        return false;
    }
    
    // Clean up existing debug filename
    if (preprocessor->debugFilename != NULL) {
        TM32ASM_Destroy(preprocessor->debugFilename);
    }
    
    // Set new debug filename (remove quotes if present)
    const char* filename = filenameToken->lexeme;
    if (filename[0] == '"' && filename[strlen(filename) - 1] == '"') {
        // Remove quotes
        size_t len = strlen(filename) - 2;
        preprocessor->debugFilename = TM32ASM_Create(len + 1, char);
        if (preprocessor->debugFilename != NULL) {
            strncpy(preprocessor->debugFilename, filename + 1, len);
            preprocessor->debugFilename[len] = '\0';
        }
    } else {
        // Use filename as-is
        size_t len = strlen(filename);
        preprocessor->debugFilename = TM32ASM_Create(len + 1, char);
        if (preprocessor->debugFilename != NULL) {
            strcpy(preprocessor->debugFilename, filename);
        }
    }
    
    if (preprocessor->debugFilename == NULL) {
        TM32ASM_LogError("Failed to allocate memory for debug filename at %s:%u", token->filename, token->line);
        return false;
    }
    
    TM32ASM_LogDebug("Set debug filename to '%s' at %s:%u", preprocessor->debugFilename, token->filename, token->line);
    return true;
}

static bool TM32ASM_HandleLineDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream,
    TM32ASM_TokenStream*    outputStream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(inputStream, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Get the line number token
    TM32ASM_Token* lineToken = TM32ASM_ConsumeNextToken(inputStream);
    if (lineToken == NULL) {
        TM32ASM_LogError("Expected line number after .line directive at %s:%u", token->filename, token->line);
        return false;
    }
    
    // Parse line number
    if (lineToken->type != TM32ASM_TT_DECIMAL) {
        TM32ASM_LogError("Expected numeric line number after .line directive at %s:%u, got %s", 
                         token->filename, token->line, lineToken->lexeme);
        return false;
    }
    
    uint32_t lineNumber = (uint32_t)strtol(lineToken->lexeme, NULL, 0);
    preprocessor->debugLine = lineNumber;
    
    TM32ASM_LogDebug("Set debug line number to %u at %s:%u", lineNumber, token->filename, token->line);
    return true;
}
