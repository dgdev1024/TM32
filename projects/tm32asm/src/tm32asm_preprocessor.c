/**
 * @file    tm32asm_preprocessor.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/30
 *
 * @brief   Contains the implementation of the TM32 assembler tool's
 *          preprocessor component.
 */

/* Include Files **************************************************************/

#include <tm32asm_preprocessor.h>
#include <ctype.h>

/* Private Constants **********************************************************/

#define TM32ASM_INITIAL_SYMBOL_CAPACITY     64      /** @brief Initial capacity for symbol array */
#define TM32ASM_INITIAL_INCLUDE_CAPACITY    16      /** @brief Initial capacity for include paths */

/* Private Function Prototypes ************************************************/

/**
 * @brief   Initializes the preprocessor's internal arrays and state.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if initialization was successful; `false` otherwise.
 */
static bool TM32ASM_InitializePreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Cleans up the preprocessor's internal state and allocated memory.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 */
static void TM32ASM_CleanupPreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Finds a symbol by name in the symbol table.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   name            The symbol name to search for.
 *
 * @return  A pointer to the symbol if found; `NULL` otherwise.
 */
static TM32ASM_Symbol* TM32ASM_FindSymbol (
    const TM32ASM_Preprocessor* preprocessor,
    const char*                 name
);

/**
 * @brief   Adds a symbol to the symbol table.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   symbol          The symbol to add.
 *
 * @return  `true` if the symbol was added successfully; `false` otherwise.
 */
static bool TM32ASM_AddSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const TM32ASM_Symbol*   symbol
);

/**
 * @brief   Resizes the symbol array if needed.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if resize was successful; `false` otherwise.
 */
static bool TM32ASM_ResizeSymbolArray (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Destroys a symbol and frees its memory.
 *
 * @param   symbol  A pointer to the symbol to destroy.
 */
static void TM32ASM_DestroySymbol (
    TM32ASM_Symbol* symbol
);

/**
 * @brief   Checks if a file has already been included.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   filename        The filename to check.
 *
 * @return  `true` if the file has been included; `false` otherwise.
 */
static bool TM32ASM_IsFileIncluded (
    const TM32ASM_Preprocessor* preprocessor,
    const char*                 filename
);

/**
 * @brief   Marks a file as included.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   filename        The filename to mark as included.
 *
 * @return  `true` if successful; `false` otherwise.
 */
static bool TM32ASM_MarkFileIncluded (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filename
);

/**
 * @brief   Performs the file inclusion pass.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if the pass was successful; `false` otherwise.
 */
static bool TM32ASM_PassFileInclusion (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Performs the symbol declaration pass.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if the pass was successful; `false` otherwise.
 */
static bool TM32ASM_PassSymbolDeclaration (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Performs the macro expansion pass.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if the pass was successful; `false` otherwise.
 */
static bool TM32ASM_PassMacroExpansion (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Performs the variable and constant evaluation pass.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if the pass was successful; `false` otherwise.
 */
static bool TM32ASM_PassVariableEvaluation (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Performs the control flow and loop processing pass.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if the pass was successful; `false` otherwise.
 */
static bool TM32ASM_PassControlFlow (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Performs the final token stream generation pass.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if the pass was successful; `false` otherwise.
 */
static bool TM32ASM_PassFinalization (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Optimizes consecutive newline tokens by reducing them to single newlines.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if the pass was successful; `false` otherwise.
 */
static bool TM32ASM_PassNewlineOptimization (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Helper function to duplicate a string.
 *
 * @param   str     The string to duplicate.
 *
 * @return  A pointer to the duplicated string, or `NULL` on error.
 */
static char* TM32ASM_DuplicateString (
    const char* str
);

/* Control Flow Helper Functions *********************************************/

/**
 * @brief   Pushes a new control flow context onto the stack.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   type            The type of control flow construct.
 * @param   line            The line number where the construct starts.
 * @param   filename        The filename where the construct is defined.
 *
 * @return  `true` if the context was pushed successfully; `false` otherwise.
 */
static bool TM32ASM_PushControlFlowContext (
    TM32ASM_Preprocessor*       preprocessor,
    TM32ASM_ControlFlowType     type,
    uint32_t                    line,
    const char*                 filename
);

/**
 * @brief   Pops the top control flow context from the stack.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if a context was popped successfully; `false` otherwise.
 */
static bool TM32ASM_PopControlFlowContext (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Gets the current (top) control flow context.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  A pointer to the current context, or `NULL` if stack is empty.
 */
static TM32ASM_ControlFlowContext* TM32ASM_GetCurrentControlFlowContext (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Checks if the current context allows code emission.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if code should be emitted; `false` otherwise.
 */
static bool TM32ASM_ShouldEmitCode (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Processes an .if directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .if directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessIfDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes an .elif directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .elif directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessElifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes an .else directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .else directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessElseDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes an .endif directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .endif directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessEndifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes a .repeat directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .repeat directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessRepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex,
    TM32ASM_TokenStream*    outputStream
);

/**
 * @brief   Processes an .endrepeat directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .endrepeat directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessEndrepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes a .while directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .while directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessWhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex,
    TM32ASM_TokenStream*    outputStream
);

/**
 * @brief   Processes an .endwhile directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .endwhile directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessEndwhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes a .for directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .for directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessForDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex,
    TM32ASM_TokenStream*    outputStream
);

/**
 * @brief   Processes an .endfor directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .endfor directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessEndforDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes a loop body, handling .break and .continue directives.
 *
 * @param   preprocessor        A pointer to the TM32ASM_Preprocessor instance.
 * @param   startBodyIndex      Start index of the loop body in the input token stream.
 * @param   endBodyIndex        End index of the loop body in the input token stream.
 * @param   outputStream        The output token stream to emit processed tokens to.
 * @param   breakRequested      Pointer to bool that will be set to true if .break is encountered.
 * @param   continueRequested   Pointer to bool that will be set to true if .continue is encountered.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessLoopBody (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  startBodyIndex,
    size_t                  endBodyIndex,
    TM32ASM_TokenStream*    outputStream,
    bool*                   breakRequested,
    bool*                   continueRequested
);

/**
 * @brief   Processes a .break directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .break directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessBreakDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Processes a .continue directive.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   token           The .continue directive token.
 * @param   tokenIndex      Current position in the token stream.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessContinueDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
);

/**
 * @brief   Executes the body of a loop construct by re-applying passes 3-5.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   startIndex      Start of the loop body in the token stream.
 * @param   endIndex        End of the loop body in the token stream.
 * @param   outputStream    The output token stream to append results to.
 *
 * @return  `true` if execution was successful; `false` otherwise.
 */
static bool TM32ASM_ExecuteLoopBody (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_TokenStream*    outputStream
);

/* Private Functions - Initialization and Cleanup ****************************/

static bool TM32ASM_InitializePreprocessor (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    // Initialize symbol array
    preprocessor->symbols = TM32ASM_Create(TM32ASM_INITIAL_SYMBOL_CAPACITY, TM32ASM_Symbol);
    if (preprocessor->symbols == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for symbol array");
        return false;
    }
    preprocessor->symbolCount = 0;
    preprocessor->symbolCapacity = TM32ASM_INITIAL_SYMBOL_CAPACITY;
    
    // Initialize include paths array
    preprocessor->includePaths = TM32ASM_Create(TM32ASM_INITIAL_INCLUDE_CAPACITY, char*);
    if (preprocessor->includePaths == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for include paths array");
        TM32ASM_Destroy(preprocessor->symbols);
        return false;
    }
    preprocessor->includePathCount = 0;
    
    // Initialize file stack
    preprocessor->fileStack = TM32ASM_Create(TM32ASM_MAX_INCLUDE_DEPTH, TM32ASM_FileContext);
    if (preprocessor->fileStack == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for file stack");
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor->includePaths);
        return false;
    }
    preprocessor->fileStackDepth = 0;
    
    // Initialize included files tracking
    preprocessor->includedFiles = TM32ASM_Create(TM32ASM_INITIAL_INCLUDE_CAPACITY, char*);
    if (preprocessor->includedFiles == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for included files array");
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor->includePaths);
        TM32ASM_Destroy(preprocessor->fileStack);
        return false;
    }
    preprocessor->includedFileCount = 0;
    
    // Initialize control flow stack
    preprocessor->controlFlowStack = TM32ASM_Create(TM32ASM_MAX_LOOP_NESTING, TM32ASM_ControlFlowContext);
    if (preprocessor->controlFlowStack == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for control flow stack");
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor->includePaths);
        TM32ASM_Destroy(preprocessor->fileStack);
        TM32ASM_Destroy(preprocessor->includedFiles);
        return false;
    }
    preprocessor->controlFlowDepth = 0;
    
    // Initialize processing state
    preprocessor->currentPass = TM32ASM_PP_FILE_INCLUSION;
    preprocessor->currentTokenIndex = 0;
    preprocessor->macroStackDepth = 0;
    preprocessor->errorOccurred = false;
    
    // Initialize options
    preprocessor->warningsAsErrors = false;
    preprocessor->verbose = false;
    
    return true;
}

static void TM32ASM_CleanupPreprocessor (
    TM32ASM_Preprocessor* preprocessor
)
{
    if (preprocessor == NULL)
    {
        return;
    }
    
    // Clean up symbols
    if (preprocessor->symbols != NULL)
    {
        for (size_t i = 0; i < preprocessor->symbolCount; i++)
        {
            TM32ASM_DestroySymbol(&preprocessor->symbols[i]);
        }
        TM32ASM_Destroy(preprocessor->symbols);
    }
    
    // Clean up include paths
    if (preprocessor->includePaths != NULL)
    {
        for (size_t i = 0; i < preprocessor->includePathCount; i++)
        {
            TM32ASM_Destroy(preprocessor->includePaths[i]);
        }
        TM32ASM_Destroy(preprocessor->includePaths);
    }
    
    // Clean up included files tracking
    if (preprocessor->includedFiles != NULL)
    {
        for (size_t i = 0; i < preprocessor->includedFileCount; i++)
        {
            TM32ASM_Destroy(preprocessor->includedFiles[i]);
        }
        TM32ASM_Destroy(preprocessor->includedFiles);
    }
    
    // Clean up token streams - we now own both input and output streams
    if (preprocessor->inputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->inputTokenStream);
    }
    
    if (preprocessor->outputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputTokenStream);
    }
    
    // Clean up file stack contexts
    if (preprocessor->fileStack != NULL)
    {
        for (size_t i = 0; i < preprocessor->fileStackDepth; i++)
        {
            if (preprocessor->fileStack[i].filename != NULL)
            {
                // Cast away const for destruction since we own this string
                free((char*)preprocessor->fileStack[i].filename);
            }
        }
        TM32ASM_Destroy(preprocessor->fileStack);
    }
    
    // Clean up control flow stack
    TM32ASM_Destroy(preprocessor->controlFlowStack);
}

static TM32ASM_Symbol* TM32ASM_FindSymbol (
    const TM32ASM_Preprocessor* preprocessor,
    const char*                 name
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(name, NULL);
    
    for (size_t i = 0; i < preprocessor->symbolCount; i++)
    {
        if (strcmp(preprocessor->symbols[i].name, name) == 0)
        {
            return &preprocessor->symbols[i];
        }
    }
    
    return NULL;
}

static bool TM32ASM_AddSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const TM32ASM_Symbol*   symbol
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(symbol, false);
    
    // Check if we need to resize the array
    if (preprocessor->symbolCount >= preprocessor->symbolCapacity)
    {
        if (!TM32ASM_ResizeSymbolArray(preprocessor))
        {
            return false;
        }
    }
    
    // Copy the symbol
    TM32ASM_Symbol* newSymbol = &preprocessor->symbols[preprocessor->symbolCount];
    memset(newSymbol, 0, sizeof(TM32ASM_Symbol));
    
    // Copy name
    newSymbol->name = TM32ASM_Create(strlen(symbol->name) + 1, char);
    if (newSymbol->name == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for symbol name");
        return false;
    }
    strcpy(newSymbol->name, symbol->name);
    
    // Copy value if present
    if (symbol->value != NULL)
    {
        newSymbol->value = TM32ASM_Create(strlen(symbol->value) + 1, char);
        if (newSymbol->value == NULL)
        {
            TM32ASM_LogErrno("Could not allocate memory for symbol value");
            TM32ASM_Destroy(newSymbol->name);
            return false;
        }
        strcpy(newSymbol->value, symbol->value);
    }
    
    // Copy other fields
    newSymbol->type = symbol->type;
    newSymbol->parameterCount = symbol->parameterCount;
    newSymbol->isDefined = symbol->isDefined;
    newSymbol->line = symbol->line;
    newSymbol->filename = symbol->filename;
    
    // Copy macro body for parametric macros
    if (symbol->macroBody != NULL)
    {
        newSymbol->macroBody = symbol->macroBody; // Transfer ownership - don't copy the whole stream
    }
    else
    {
        newSymbol->macroBody = NULL;
    }
    
    // TODO: Handle parameter copying for parametric macros
    newSymbol->parameters = NULL;
    
    preprocessor->symbolCount++;
    return true;
}

static bool TM32ASM_ResizeSymbolArray (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    size_t newCapacity = preprocessor->symbolCapacity * 2;
    TM32ASM_Symbol* newSymbols = TM32ASM_Resize(
        preprocessor->symbols,
        newCapacity,
        TM32ASM_Symbol
    );
    
    if (newSymbols == NULL)
    {
        TM32ASM_LogErrno("Could not resize symbol array");
        return false;
    }
    
    preprocessor->symbols = newSymbols;
    preprocessor->symbolCapacity = newCapacity;
    return true;
}

static void TM32ASM_DestroySymbol (
    TM32ASM_Symbol* symbol
)
{
    if (symbol == NULL)
    {
        return;
    }
    
    TM32ASM_Destroy(symbol->name);
    TM32ASM_Destroy(symbol->value);
    
    if (symbol->parameters != NULL)
    {
        for (size_t i = 0; i < symbol->parameterCount; i++)
        {
            TM32ASM_Destroy(symbol->parameters[i]);
        }
        TM32ASM_Destroy(symbol->parameters);
    }
    
    // Clean up macro body if it exists
    if (symbol->macroBody != NULL)
    {
        TM32ASM_DestroyTokenStream(symbol->macroBody);
        symbol->macroBody = NULL;
    }
    
    memset(symbol, 0, sizeof(TM32ASM_Symbol));
}

/* Private Functions - File Inclusion ****************************************/

static bool TM32ASM_IsFileIncluded (
    const TM32ASM_Preprocessor* preprocessor,
    const char*                 filename
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(filename, false);
    
    for (size_t i = 0; i < preprocessor->includedFileCount; i++)
    {
        if (strcmp(preprocessor->includedFiles[i], filename) == 0)
        {
            return true;
        }
    }
    
    return false;
}

static bool TM32ASM_MarkFileIncluded (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             filename
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(filename, false);
    
    // Allocate memory for the filename copy
    char* filenameCopy = TM32ASM_Create(strlen(filename) + 1, char);
    if (filenameCopy == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for included filename");
        return false;
    }
    strcpy(filenameCopy, filename);
    
    // Add to the included files array
    preprocessor->includedFiles[preprocessor->includedFileCount] = filenameCopy;
    preprocessor->includedFileCount++;
    
    return true;
}

/* Private Functions - Pass Implementation ***********************************/

static bool TM32ASM_PassFileInclusion (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(preprocessor->inputTokenStream, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting file inclusion pass");
    }
    
    // Reset read pointer to start of token stream
    preprocessor->inputTokenStream->tokenReadPointer = 0;
    
    // Create a new token stream for the output with included files
    TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
    if (outputStream == NULL)
    {
        TM32ASM_LogError("Could not create output token stream for file inclusion");
        return false;
    }
    
    // Process all tokens in the input stream
    TM32ASM_Token* token = NULL;
    while ((token = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream)) != NULL)
    {
        // Skip overly verbose token processing debug output
        
        // Check for .include directive
        if (token->type == TM32ASM_TT_DIRECTIVE && token->param == TM32ASM_DT_INCLUDE)
        {
            TM32ASM_LogInfo("PROCESSING .include directive at line %u", token->line);
            
            // Get the next token, which should be a string containing the filename
            TM32ASM_Token* filenameToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
            if (filenameToken == NULL)
            {
                TM32ASM_LogError("Expected filename after .include directive at line %u", token->line);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            if (preprocessor->verbose)
            {
                TM32ASM_LogInfo("Filename token: type=%d, lexeme='%s' (length=%zu), line=%u", 
                              filenameToken->type, filenameToken->lexeme ? filenameToken->lexeme : "NULL",
                              filenameToken->lexeme ? strlen(filenameToken->lexeme) : 0, filenameToken->line);
            }
            
            if (filenameToken->type != TM32ASM_TT_STRING)
            {
                TM32ASM_LogError("Expected string filename after .include directive at line %u, got %s", 
                               token->line, filenameToken->lexeme);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            // Extract filename from string token (remove quotes)
            const char* filename = filenameToken->lexeme;
            if (strlen(filename) < 2 || filename[0] != '"' || filename[strlen(filename) - 1] != '"')
            {
                TM32ASM_LogError("Invalid string format for filename at line %u: %s", 
                               filenameToken->line, filename);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            // Create a copy of the filename without quotes
            size_t filenameLen = strlen(filename) - 2; // Remove quotes
            char* includedFilename = TM32ASM_Create(filenameLen + 1, char);
            if (includedFilename == NULL)
            {
                TM32ASM_LogErrno("Could not allocate memory for included filename");
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            strncpy(includedFilename, filename + 1, filenameLen);
            includedFilename[filenameLen] = '\0';
            
            if (preprocessor->verbose)
            {
                TM32ASM_LogInfo("Extracted filename: '%s' (length=%zu) from context: '%s'", 
                                includedFilename, strlen(includedFilename), 
                                token->filename ? token->filename : "unknown");
            }
            
            // Check if file has already been included
            if (TM32ASM_IsFileIncluded(preprocessor, includedFilename))
            {
                if (preprocessor->verbose)
                {
                    TM32ASM_LogInfo("File '%s' already included, skipping", includedFilename);
                }
                TM32ASM_Destroy(includedFilename);
                continue;
            }
            
            // Check include depth
            if (preprocessor->fileStackDepth >= TM32ASM_MAX_INCLUDE_DEPTH)
            {
                TM32ASM_LogError("Maximum include depth (%u) exceeded when including '%s'", 
                               TM32ASM_MAX_INCLUDE_DEPTH, includedFilename);
                TM32ASM_Destroy(includedFilename);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            // Try to resolve the file path
            char* resolvedPath = NULL;
            if (includedFilename[0] == '/' || includedFilename[0] == '\\')
            {
                // Absolute path - use as-is
                resolvedPath = TM32ASM_Create(strlen(includedFilename) + 1, char);
                if (resolvedPath != NULL)
                {
                    strcpy(resolvedPath, includedFilename);
                }
            }
            else
            {
                // Relative path - try include paths
                for (size_t i = 0; i < preprocessor->includePathCount; i++)
                {
                    size_t pathLen = strlen(preprocessor->includePaths[i]) + strlen(includedFilename) + 2; // +2 for '/' and '\0'
                    char* testPath = TM32ASM_Create(pathLen, char);
                    if (testPath == NULL)
                    {
                        continue;
                    }
                    
                    snprintf(testPath, pathLen, "%s/%s", preprocessor->includePaths[i], includedFilename);
                    
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Testing include path: '%s'", testPath);
                    }
                    
                    // Test if file exists
                    FILE* testFile = fopen(testPath, "r");
                    if (testFile != NULL)
                    {
                        fclose(testFile);
                        resolvedPath = testPath;
                        if (preprocessor->verbose)
                        {
                            TM32ASM_LogInfo("Found file at include path: '%s'", testPath);
                        }
                        break;
                    }
                    
                    TM32ASM_Destroy(testPath);
                }
                
                // If not found in include paths, try relative to current file
                if (resolvedPath == NULL && token->filename != NULL)
                {
                    // Extract directory from current filename
                    const char* lastSlash = strrchr(token->filename, '/');
                    const char* lastBackslash = strrchr(token->filename, '\\');
                    const char* dirEnd = (lastSlash > lastBackslash) ? lastSlash : lastBackslash;
                    
                    if (dirEnd != NULL)
                    {
                        size_t dirLen = dirEnd - token->filename + 1;
                        size_t pathLen = dirLen + strlen(includedFilename) + 1;
                        char* testPath = TM32ASM_Create(pathLen, char);
                        if (testPath != NULL)
                        {
                            strncpy(testPath, token->filename, dirLen);
                            strcpy(testPath + dirLen, includedFilename);
                            
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Testing relative path: '%s' (from context dir of '%s')", 
                                                testPath, token->filename);
                            }
                            
                            // Test if file exists
                            FILE* testFile = fopen(testPath, "r");
                            if (testFile != NULL)
                            {
                                fclose(testFile);
                                resolvedPath = testPath;
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("Found file at relative path: '%s'", testPath);
                                }
                            }
                            else
                            {
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("File not found at relative path: '%s'", testPath);
                                }
                                TM32ASM_Destroy(testPath);
                            }
                        }
                    }
                }
            }
            
            if (resolvedPath == NULL)
            {
                TM32ASM_LogError("Could not find include file '%s' at line %u", includedFilename, token->line);
                TM32ASM_Destroy(includedFilename);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            // Mark file as included
            if (!TM32ASM_MarkFileIncluded(preprocessor, includedFilename))
            {
                TM32ASM_LogError("Could not mark file '%s' as included", includedFilename);
                TM32ASM_Destroy(includedFilename);
                TM32ASM_Destroy(resolvedPath);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            // Create lexer for included file
            TM32ASM_Lexer* includeLexer = TM32ASM_CreateLexer();
            if (includeLexer == NULL)
            {
                TM32ASM_LogError("Could not create lexer for included file '%s'", resolvedPath);
                TM32ASM_Destroy(includedFilename);
                TM32ASM_Destroy(resolvedPath);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            // Load and tokenize the included file
            if (!TM32ASM_LoadSourceFile(includeLexer, resolvedPath))
            {
                TM32ASM_LogError("Could not load included file '%s'", resolvedPath);
                TM32ASM_DestroyLexer(includeLexer);
                TM32ASM_Destroy(includedFilename);
                TM32ASM_Destroy(resolvedPath);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            if (!TM32ASM_TokenizeSource(includeLexer))
            {
                TM32ASM_LogError("Could not tokenize included file '%s'", resolvedPath);
                TM32ASM_DestroyLexer(includeLexer);
                TM32ASM_Destroy(includedFilename);
                TM32ASM_Destroy(resolvedPath);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            // Add file context to stack
            if (preprocessor->fileStackDepth < TM32ASM_MAX_INCLUDE_DEPTH)
            {
                TM32ASM_FileContext* context = &preprocessor->fileStack[preprocessor->fileStackDepth];
                context->filename = resolvedPath; // Transfer ownership
                context->tokenStream = includeLexer->tokenStream;
                context->currentPosition = 0;
                context->line = 1;
                preprocessor->fileStackDepth++;
            }
            
            // Copy tokens from included file to output stream (excluding EOF)
            includeLexer->tokenStream->tokenReadPointer = 0;
            TM32ASM_Token* includeToken = NULL;
            while ((includeToken = TM32ASM_ConsumeNextToken(includeLexer->tokenStream)) != NULL)
            {
                if (includeToken->type == TM32ASM_TT_END_OF_FILE)
                {
                    break; // Skip EOF token
                }
                
                // Copy the token to output stream
                TM32ASM_Token* copiedToken = TM32ASM_CopyToken(includeToken);
                if (copiedToken == NULL)
                {
                    TM32ASM_LogError("Could not copy token from included file '%s'", resolvedPath);
                    TM32ASM_DestroyLexer(includeLexer);
                    TM32ASM_Destroy(includedFilename);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                
                if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
                {
                    TM32ASM_LogError("Could not add token from included file '%s' to output stream", resolvedPath);
                    TM32ASM_DestroyToken(copiedToken);
                    TM32ASM_DestroyLexer(includeLexer);
                    TM32ASM_Destroy(includedFilename);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
            }
            
            // Print success message before cleaning up (while resolvedPath is still valid)
            if (preprocessor->verbose)
            {
                TM32ASM_LogInfo("Successfully included file '%s'", resolvedPath);
            }
            
            // Pop file context from stack
            if (preprocessor->fileStackDepth > 0)
            {
                preprocessor->fileStackDepth--;
                // Free the filename since we're done processing this file
                // The tokens have already been copied with their own filename references
                TM32ASM_FileContext* poppedContext = &preprocessor->fileStack[preprocessor->fileStackDepth];
                if (poppedContext->filename != NULL)
                {
                    char* filenameToFree = (char*)poppedContext->filename;
                    TM32ASM_Destroy(filenameToFree);
                    poppedContext->filename = NULL;
                }
            }
            
            // Clean up
            TM32ASM_DestroyLexer(includeLexer);
            TM32ASM_Destroy(includedFilename);
            
            // Skip to next token (don't copy the .include directive to output)
            TM32ASM_LogInfo("SKIPPING .include directive at line %u (successfully processed)", token->line);
            continue;
        }
        else
        {
            // Regular token - copy to output stream
            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
            if (copiedToken == NULL)
            {
                TM32ASM_LogError("Could not copy token to output stream");
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
            {
                TM32ASM_LogError("Could not add token to output stream");
                TM32ASM_DestroyToken(copiedToken);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
        }
    }
    
    // Set the output token stream to the processed stream
    if (preprocessor->outputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputTokenStream);
    }
    preprocessor->outputTokenStream = outputStream;
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("File inclusion pass completed successfully");
    }
    
    return true;
}

static bool TM32ASM_PassSymbolDeclaration (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(preprocessor->inputTokenStream, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting symbol declaration pass");
    }
    
    // Create output token stream
    TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
    if (outputStream == NULL)
    {
        TM32ASM_LogError("Could not create output token stream for symbol declaration pass");
        return false;
    }
    
    // Reset read pointer to start of token stream
    preprocessor->inputTokenStream->tokenReadPointer = 0;
    
    // Scan through all tokens looking for symbol declarations
    TM32ASM_Token* token = NULL;
    while ((token = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream)) != NULL)
    {
        bool skipToken = false;
        
        // Check for directive tokens that declare symbols
        if (token->type == TM32ASM_TT_DIRECTIVE)
        {
            switch (token->param)
            {
                case TM32ASM_DT_MACRO:
                {
                    // .macro directive - parametric macro definition
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Found .macro directive at line %u", token->line);
                    }
                    
                    // Get the macro name (next token should be identifier)
                    TM32ASM_Token* nameToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                    if (nameToken == NULL || nameToken->type != TM32ASM_TT_IDENTIFIER)
                    {
                        TM32ASM_LogError("Expected identifier after .macro directive at line %u", token->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Check if macro already exists
                    TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, nameToken->lexeme);
                    if (existingSymbol != NULL)
                    {
                        TM32ASM_LogError("Symbol '%s' is already defined at line %u (redefinition at line %u)",
                                       nameToken->lexeme, existingSymbol->line, nameToken->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Skip to end of parameter line (if any)
                    TM32ASM_Token* nextToken = NULL;
                    while ((nextToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream)) != NULL)
                    {
                        if (nextToken->type == TM32ASM_TT_NEWLINE)
                        {
                            break;
                        }
                        // TODO: Parse and store parameter names for later use
                    }
                    
                    // Capture macro body until .endmacro
                    TM32ASM_TokenStream* macroBody = TM32ASM_CreateTokenStream();
                    if (macroBody == NULL)
                    {
                        TM32ASM_LogError("Could not create macro body token stream");
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    int macroDepth = 1;
                    while ((nextToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream)) != NULL && macroDepth > 0)
                    {
                        if (nextToken->type == TM32ASM_TT_DIRECTIVE)
                        {
                            if (nextToken->param == TM32ASM_DT_MACRO)
                            {
                                macroDepth++;
                            }
                            else if (nextToken->param == TM32ASM_DT_ENDMACRO)
                            {
                                macroDepth--;
                                if (macroDepth == 0)
                                {
                                    break; // Don't include .endmacro in the body
                                }
                            }
                        }
                        
                        // Copy token to macro body
                        TM32ASM_Token* bodyCopy = TM32ASM_CopyToken(nextToken);
                        if (bodyCopy == NULL)
                        {
                            TM32ASM_LogError("Could not copy token to macro body");
                            TM32ASM_DestroyTokenStream(macroBody);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                        
                        if (TM32ASM_PushTokenBack(macroBody, bodyCopy) == NULL)
                        {
                            TM32ASM_LogError("Could not add token to macro body");
                            TM32ASM_DestroyToken(bodyCopy);
                            TM32ASM_DestroyTokenStream(macroBody);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                    }
                    
                    // Create symbol entry for the macro
                    TM32ASM_Symbol symbol = {0};
                    symbol.name = nameToken->lexeme;  // Temporary assignment for AddSymbol
                    symbol.type = TM32ASM_ST_MACRO_PARAMETRIC;
                    symbol.value = NULL;
                    symbol.parameters = NULL;
                    symbol.parameterCount = 0;
                    symbol.isDefined = true;
                    symbol.line = nameToken->line;
                    symbol.filename = nameToken->filename;
                    symbol.macroBody = macroBody; // Store the captured macro body
                    
                    if (!TM32ASM_AddSymbol(preprocessor, &symbol))
                    {
                        TM32ASM_LogError("Failed to add macro symbol '%s'", nameToken->lexeme);
                        TM32ASM_DestroyTokenStream(macroBody);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Recorded parametric macro: %s", nameToken->lexeme);
                    }
                    skipToken = true; // Don't copy .macro directive to output
                    break;
                }
                
                case TM32ASM_DT_DEFINE:
                {
                    // .define/.def directive - simple text substitution macro
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Found .define directive at line %u", token->line);
                    }
                    
                    // Get the macro name (next token should be identifier)
                    TM32ASM_Token* nameToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                    if (nameToken == NULL || nameToken->type != TM32ASM_TT_IDENTIFIER)
                    {
                        TM32ASM_LogError("Expected identifier after .define directive at line %u", token->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Check if symbol already exists
                    TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, nameToken->lexeme);
                    if (existingSymbol != NULL)
                    {
                        TM32ASM_LogError("Symbol '%s' is already defined at line %u (redefinition at line %u)",
                                       nameToken->lexeme, existingSymbol->line, nameToken->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Get the replacement text (next token - can be any type)
                    TM32ASM_Token* valueToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                    if (valueToken == NULL)
                    {
                        TM32ASM_LogError("Expected value after .define identifier at line %u", token->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Create symbol entry for the simple macro
                    TM32ASM_Symbol symbol = {0};
                    symbol.name = nameToken->lexeme;  // Temporary assignment for AddSymbol
                    symbol.type = TM32ASM_ST_MACRO_SIMPLE;
                    symbol.value = valueToken->lexeme;  // Temporary assignment for AddSymbol
                    symbol.parameters = NULL;
                    symbol.parameterCount = 0;
                    symbol.isDefined = true;
                    symbol.line = nameToken->line;
                    symbol.filename = nameToken->filename;
                    
                    if (!TM32ASM_AddSymbol(preprocessor, &symbol))
                    {
                        TM32ASM_LogError("Failed to add simple macro symbol '%s'", nameToken->lexeme);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Recorded simple macro: %s = %s", nameToken->lexeme, valueToken->lexeme);
                    }
                    skipToken = true; // Don't copy .define directive to output
                    break;
                }
                
                case TM32ASM_DT_LET:
                {
                    // .let directive - mutable variable declaration
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Found .let directive at line %u", token->line);
                    }
                    
                    // Get the variable name (next token should be identifier)
                    TM32ASM_Token* nameToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                    if (nameToken == NULL || nameToken->type != TM32ASM_TT_IDENTIFIER)
                    {
                        TM32ASM_LogError("Expected identifier after .let directive at line %u", token->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Check if symbol already exists
                    TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, nameToken->lexeme);
                    if (existingSymbol != NULL)
                    {
                        // Variables can be reassigned, but constants and macros cannot
                        if (existingSymbol->type == TM32ASM_ST_CONSTANT)
                        {
                            TM32ASM_LogError("Cannot reassign constant '%s' (defined at line %u) at line %u",
                                           nameToken->lexeme, existingSymbol->line, nameToken->line);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                        else if (existingSymbol->type == TM32ASM_ST_MACRO_SIMPLE || existingSymbol->type == TM32ASM_ST_MACRO_PARAMETRIC)
                        {
                            TM32ASM_LogError("Cannot reassign macro '%s' (defined at line %u) at line %u",
                                           nameToken->lexeme, existingSymbol->line, nameToken->line);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                        // Variables (TM32ASM_ST_VARIABLE) can be reassigned - continue with processing
                    }
                    
                    // Check for assignment operator (regular or compound)
                    TM32ASM_Token* nextToken = TM32ASM_PeekNextToken(preprocessor->inputTokenStream, 0);
                    char* initialValue = NULL;
                    TM32ASM_TokenType assignmentOp = TM32ASM_TT_UNKNOWN;
                    
                    if (nextToken != NULL && 
                        (nextToken->type == TM32ASM_TT_ASSIGN_EQUAL ||
                         nextToken->type == TM32ASM_TT_ASSIGN_PLUS ||
                         nextToken->type == TM32ASM_TT_ASSIGN_MINUS ||
                         nextToken->type == TM32ASM_TT_ASSIGN_TIMES ||
                         nextToken->type == TM32ASM_TT_ASSIGN_EXPONENT ||
                         nextToken->type == TM32ASM_TT_ASSIGN_DIVIDE ||
                         nextToken->type == TM32ASM_TT_ASSIGN_MODULO ||
                         nextToken->type == TM32ASM_TT_ASSIGN_BITWISE_AND ||
                         nextToken->type == TM32ASM_TT_ASSIGN_BITWISE_OR ||
                         nextToken->type == TM32ASM_TT_ASSIGN_BITWISE_XOR ||
                         nextToken->type == TM32ASM_TT_ASSIGN_LEFT_SHIFT ||
                         nextToken->type == TM32ASM_TT_ASSIGN_RIGHT_SHIFT))
                    {
                        // Consume the assignment operator
                        assignmentOp = nextToken->type;
                        TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                        
                        // For compound assignment operators, we need the current variable value
                        int64_t currentValue = 0;
                        bool hasCurrentValue = false;
                        
                        if (assignmentOp != TM32ASM_TT_ASSIGN_EQUAL && existingSymbol != NULL && existingSymbol->value != NULL)
                        {
                            currentValue = strtoll(existingSymbol->value, NULL, 0);
                            hasCurrentValue = true;
                        }
                        
                        // Collect expression tokens until end of line or statement
                        TM32ASM_TokenStream* expressionTokens = TM32ASM_CreateTokenStream();
                        if (expressionTokens == NULL)
                        {
                            TM32ASM_LogError("Could not create expression token stream for variable");
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                        
                        // Read tokens until end of line
                        TM32ASM_Token* exprToken = NULL;
                        while ((exprToken = TM32ASM_PeekNextToken(preprocessor->inputTokenStream, 0)) != NULL &&
                               exprToken->type != TM32ASM_TT_NEWLINE &&
                               exprToken->type != TM32ASM_TT_END_OF_FILE)
                        {
                            TM32ASM_Token* consumedToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(consumedToken);
                            if (copiedToken == NULL || TM32ASM_PushTokenBack(expressionTokens, copiedToken) == NULL)
                            {
                                TM32ASM_LogError("Could not store expression token for variable");
                                TM32ASM_DestroyTokenStream(expressionTokens);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                        }
                        
                        // Try to evaluate the expression immediately
                        if (expressionTokens->tokenCount > 0)
                        {
                            int64_t expressionResult;
                            if (TM32ASM_EvaluateExpression(preprocessor, expressionTokens, 0, expressionTokens->tokenCount, &expressionResult))
                            {
                                // Successfully evaluated - now apply the operation
                                int64_t finalResult;
                                
                                switch (assignmentOp)
                                {
                                    case TM32ASM_TT_ASSIGN_EQUAL:
                                        finalResult = expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_PLUS:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use += on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue + expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_MINUS:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use -= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue - expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_TIMES:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use *= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue * expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_DIVIDE:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use /= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        if (expressionResult == 0)
                                        {
                                            TM32ASM_LogError("Division by zero in /= assignment for variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue / expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_MODULO:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use modulo assignment on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        if (expressionResult == 0)
                                        {
                                            TM32ASM_LogError("Modulo by zero in modulo assignment for variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue % expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_EXPONENT:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use **= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        if (expressionResult < 0)
                                        {
                                            TM32ASM_LogError("Negative exponents not supported in **= assignment");
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = 1;
                                        for (int64_t i = 0; i < expressionResult; i++)
                                        {
                                            finalResult *= currentValue;
                                        }
                                        break;
                                    case TM32ASM_TT_ASSIGN_BITWISE_AND:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use &= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue & expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_BITWISE_OR:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use |= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue | expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_BITWISE_XOR:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use ^= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue ^ expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_LEFT_SHIFT:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use <<= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue << expressionResult;
                                        break;
                                    case TM32ASM_TT_ASSIGN_RIGHT_SHIFT:
                                        if (!hasCurrentValue)
                                        {
                                            TM32ASM_LogError("Cannot use >>= on undefined variable '%s'", nameToken->lexeme);
                                            TM32ASM_DestroyTokenStream(expressionTokens);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        finalResult = currentValue >> expressionResult;
                                        break;
                                    default:
                                        TM32ASM_LogError("Unknown assignment operator for variable '%s'", nameToken->lexeme);
                                        TM32ASM_DestroyTokenStream(expressionTokens);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                }
                                
                                // Store the final result
                                char resultBuffer[32];
                                snprintf(resultBuffer, sizeof(resultBuffer), "%lld", (long long)finalResult);
                                initialValue = TM32ASM_DuplicateString(resultBuffer);
                                
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("Evaluated variable expression: %s = %lld", nameToken->lexeme, (long long)finalResult);
                                }
                            }
                            else
                            {
                                // Could not evaluate now - will need forward reference resolution
                                initialValue = "UNEVALUATED";
                                
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("Variable expression requires forward reference resolution: %s", nameToken->lexeme);
                                }
                            }
                        }
                        else
                        {
                            initialValue = "0";  // Empty expression defaults to 0
                        }
                        
                        TM32ASM_DestroyTokenStream(expressionTokens);
                    }
                    
                    // Create or update symbol entry for the variable
                    if (existingSymbol != NULL && existingSymbol->type == TM32ASM_ST_VARIABLE)
                    {
                        // Update existing variable
                        if (existingSymbol->value != NULL)
                        {
                            TM32ASM_Destroy(existingSymbol->value);
                        }
                        existingSymbol->value = initialValue;
                        existingSymbol->line = nameToken->line;
                        existingSymbol->filename = nameToken->filename;
                        
                        if (preprocessor->verbose)
                        {
                            TM32ASM_LogInfo("Updated variable: %s", nameToken->lexeme);
                        }
                    }
                    else
                    {
                        // Create new symbol entry for the variable
                        TM32ASM_Symbol symbol = {0};
                        symbol.name = nameToken->lexeme;  // Temporary assignment for AddSymbol
                        symbol.type = TM32ASM_ST_VARIABLE;
                        symbol.value = initialValue;
                        symbol.parameters = NULL;
                        symbol.parameterCount = 0;
                        symbol.isDefined = true;
                        symbol.line = nameToken->line;
                        symbol.filename = nameToken->filename;
                        
                        if (!TM32ASM_AddSymbol(preprocessor, &symbol))
                        {
                            TM32ASM_LogError("Failed to add variable symbol '%s'", nameToken->lexeme);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                        
                        if (preprocessor->verbose)
                        {
                            TM32ASM_LogInfo("Recorded variable: %s", nameToken->lexeme);
                        }
                    }
                    skipToken = true; // Don't copy .let directive to output
                    break;
                }
                
                case TM32ASM_DT_CONST:
                {
                    // .const directive - immutable constant declaration
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Found .const directive at line %u", token->line);
                    }
                    
                    // Get the constant name (next token should be identifier)
                    TM32ASM_Token* nameToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                    if (nameToken == NULL)
                    {
                        TM32ASM_LogError("No token found after .const directive at line %u", token->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    if (nameToken->type != TM32ASM_TT_IDENTIFIER)
                    {
                        TM32ASM_LogError("Expected identifier after .const directive at line %u", token->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Check if symbol already exists
                    TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, nameToken->lexeme);
                    if (existingSymbol != NULL)
                    {
                        TM32ASM_LogError("Symbol '%s' is already defined at line %u (redefinition at line %u)",
                                       nameToken->lexeme, existingSymbol->line, nameToken->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Constants must have an assignment operator
                    TM32ASM_Token* assignToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                    if (assignToken == NULL || assignToken->type != TM32ASM_TT_ASSIGN_EQUAL)
                    {
                        TM32ASM_LogError("Expected '=' after .const identifier at line %u", token->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Collect expression tokens until end of line or statement
                    TM32ASM_TokenStream* expressionTokens = TM32ASM_CreateTokenStream();
                    if (expressionTokens == NULL)
                    {
                        TM32ASM_LogError("Could not create expression token stream for constant");
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Read tokens until end of line
                    TM32ASM_Token* exprToken = NULL;
                    while ((exprToken = TM32ASM_PeekNextToken(preprocessor->inputTokenStream, 0)) != NULL &&
                           exprToken->type != TM32ASM_TT_NEWLINE &&
                           exprToken->type != TM32ASM_TT_END_OF_FILE)
                    {
                        TM32ASM_Token* consumedToken = TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(consumedToken);
                        if (copiedToken == NULL || TM32ASM_PushTokenBack(expressionTokens, copiedToken) == NULL)
                        {
                            TM32ASM_LogError("Could not store expression token for constant");
                            TM32ASM_DestroyTokenStream(expressionTokens);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                    }
                    
                    // Try to evaluate the expression immediately
                    char* constantValue = NULL;
                    if (expressionTokens->tokenCount > 0)
                    {
                        int64_t result;
                        if (TM32ASM_EvaluateExpression(preprocessor, expressionTokens, 0, expressionTokens->tokenCount, &result))
                        {
                            // Successfully evaluated - store the result
                            char resultBuffer[32];
                            snprintf(resultBuffer, sizeof(resultBuffer), "%lld", (long long)result);
                            constantValue = TM32ASM_DuplicateString(resultBuffer);
                            
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Evaluated constant expression: %s = %lld", nameToken->lexeme, (long long)result);
                            }
                        }
                        else
                        {
                            // Could not evaluate now - will need forward reference resolution
                            constantValue = "UNEVALUATED";
                            
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Constant expression requires forward reference resolution: %s", nameToken->lexeme);
                            }
                        }
                    }
                    else
                    {
                        TM32ASM_LogError("Expected expression after '=' in .const directive");
                        TM32ASM_DestroyTokenStream(expressionTokens);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    TM32ASM_DestroyTokenStream(expressionTokens);
                    
                    // Create symbol entry for the constant
                    TM32ASM_Symbol symbol = {0};
                    symbol.name = nameToken->lexeme;  // Temporary assignment for AddSymbol
                    symbol.type = TM32ASM_ST_CONSTANT;
                    symbol.value = constantValue;  // Will be evaluated in variable evaluation pass
                    symbol.parameters = NULL;
                    symbol.parameterCount = 0;
                    symbol.isDefined = true;
                    symbol.line = nameToken->line;
                    symbol.filename = nameToken->filename;
                    
                    if (!TM32ASM_AddSymbol(preprocessor, &symbol))
                    {
                        TM32ASM_LogError("Failed to add constant symbol '%s'", nameToken->lexeme);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Recorded constant: %s", nameToken->lexeme);
                    }
                    skipToken = true; // Don't copy .const directive to output
                    break;
                }
                
                default:
                    // Not a symbol declaration directive, copy to output
                    break;
            }
        }
        
        // Copy the current token to output stream (unless we're skipping it)
        if (!skipToken)
        {
            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
            if (copiedToken == NULL)
            {
                TM32ASM_LogError("Could not copy token during symbol declaration pass");
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
            {
                TM32ASM_LogError("Could not add token to output stream during symbol declaration pass");
                TM32ASM_DestroyToken(copiedToken);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
        }
    }
    
    // Set the output token stream
    if (preprocessor->outputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputTokenStream);
    }
    preprocessor->outputTokenStream = outputStream;
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Symbol declaration pass completed successfully with %zu symbols recorded",
                        preprocessor->symbolCount);
    }
    
    return true;
}

static bool TM32ASM_PassMacroExpansion (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting macro expansion pass");
    }
    
    // Iterative macro expansion to handle nested macro calls
    // Keep expanding until no more expansions occur
    bool expansionOccurred = true;
    int iterationCount = 0;
    const int maxIterations = 100; // Prevent infinite recursion
    
    TM32ASM_TokenStream* workingInput = preprocessor->inputTokenStream;
    TM32ASM_TokenStream* finalOutput = NULL;
    
    while (expansionOccurred && iterationCount < maxIterations)
    {
        expansionOccurred = false;
        iterationCount++;
        
        if (preprocessor->verbose && iterationCount > 1)
        {
            TM32ASM_LogInfo("Macro expansion iteration %d", iterationCount);
        }
        
        // Create output token stream for this iteration
        TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
        if (outputStream == NULL)
        {
            TM32ASM_LogError("Could not create output token stream for macro expansion pass");
            if (finalOutput != NULL && finalOutput != preprocessor->inputTokenStream)
            {
                TM32ASM_DestroyTokenStream(finalOutput);
            }
            return false;
        }

        // Process tokens by index for better control
        for (size_t i = 0; i < workingInput->tokenCount; i++)
        {
            TM32ASM_Token* token = workingInput->tokens[i];
        bool skipToken = false;
        
        // Skip macro definition directives - they should not appear in the output
        if (token->type == TM32ASM_TT_DIRECTIVE && 
            (token->param == TM32ASM_DT_MACRO || token->param == TM32ASM_DT_DEFINE))
        {
            if (token->param == TM32ASM_DT_MACRO)
            {
                // Skip entire macro definition until .endmacro
                skipToken = true;
                i++; // Skip .macro
                
                // Skip macro name and parameters line
                while (i < preprocessor->inputTokenStream->tokenCount)
                {
                    TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[i];
                    i++;
                    if (currentToken->type == TM32ASM_TT_NEWLINE)
                    {
                        break;
                    }
                }
                
                // Skip all tokens until .endmacro
                int macroDepth = 1;
                while (i < preprocessor->inputTokenStream->tokenCount && macroDepth > 0)
                {
                    TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[i];
                    if (currentToken->type == TM32ASM_TT_DIRECTIVE)
                    {
                        if (currentToken->param == TM32ASM_DT_MACRO)
                        {
                            macroDepth++;
                        }
                        else if (currentToken->param == TM32ASM_DT_ENDMACRO)
                        {
                            macroDepth--;
                        }
                    }
                    i++;
                }
                i--; // Compensate for the for loop increment
            }
            else if (token->param == TM32ASM_DT_DEFINE)
            {
                // Skip .define directive and its arguments (name and value)
                skipToken = true;
                
                // Skip the identifier name (next token)
                if (i + 1 < preprocessor->inputTokenStream->tokenCount)
                {
                    i++;
                }
                
                // Skip the string value (next token)
                if (i + 1 < preprocessor->inputTokenStream->tokenCount)
                {
                    i++;
                }
            }
            continue;
        }
        
        // Check for macro invocation (parametric macros)
        if (token->type == TM32ASM_TT_IDENTIFIER)
        {
            if (preprocessor->verbose)
            {
                TM32ASM_LogInfo("Checking identifier '%s' for macro expansion", token->lexeme);
            }
            
            TM32ASM_Symbol* macroSymbol = TM32ASM_FindSymbol(preprocessor, token->lexeme);
            if (macroSymbol != NULL && macroSymbol->type == TM32ASM_ST_MACRO_PARAMETRIC)
            {
                if (preprocessor->verbose)
                {
                    TM32ASM_LogInfo("Found parametric macro '%s', expanding...", token->lexeme);
                }
                
                expansionOccurred = true; // Mark that an expansion occurred
                skipToken = true; // We're replacing this token with expanded content
                
                // Parse parameters until newline
                TM32ASM_TokenStream** parameters = NULL;
                size_t parameterCount = 0;
                i++; // Move past macro name
                
                // Collect parameters
                TM32ASM_TokenStream* currentParam = TM32ASM_CreateTokenStream();
                if (currentParam == NULL)
                {
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                
                while (i < preprocessor->inputTokenStream->tokenCount)
                {
                    TM32ASM_Token* paramToken = preprocessor->inputTokenStream->tokens[i];
                    
                    if (paramToken->type == TM32ASM_TT_NEWLINE)
                    {
                        // End of parameter list
                        if (currentParam->tokenCount > 0)
                        {
                            parameters = realloc(parameters, sizeof(TM32ASM_TokenStream*) * (parameterCount + 1));
                            if (parameters == NULL)
                            {
                                TM32ASM_DestroyTokenStream(currentParam);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            parameters[parameterCount] = currentParam;
                            parameterCount++;
                        }
                        else
                        {
                            TM32ASM_DestroyTokenStream(currentParam);
                        }
                        break;
                    }
                    else if (paramToken->type == TM32ASM_TT_COMMA)
                    {
                        // End of current parameter
                        if (currentParam->tokenCount > 0)
                        {
                            parameters = realloc(parameters, sizeof(TM32ASM_TokenStream*) * (parameterCount + 1));
                            if (parameters == NULL)
                            {
                                TM32ASM_DestroyTokenStream(currentParam);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            parameters[parameterCount] = currentParam;
                            parameterCount++;
                        }
                        else
                        {
                            TM32ASM_DestroyTokenStream(currentParam);
                        }
                        
                        // Start new parameter
                        currentParam = TM32ASM_CreateTokenStream();
                        if (currentParam == NULL)
                        {
                            for (size_t j = 0; j < parameterCount; j++)
                            {
                                TM32ASM_DestroyTokenStream(parameters[j]);
                            }
                            free(parameters);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                    }
                    else
                    {
                        // Add token to current parameter
                        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(paramToken);
                        if (copiedToken == NULL)
                        {
                            TM32ASM_DestroyTokenStream(currentParam);
                            for (size_t j = 0; j < parameterCount; j++)
                            {
                                TM32ASM_DestroyTokenStream(parameters[j]);
                            }
                            free(parameters);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                        
                        if (TM32ASM_PushTokenBack(currentParam, copiedToken) == NULL)
                        {
                            TM32ASM_DestroyToken(copiedToken);
                            TM32ASM_DestroyTokenStream(currentParam);
                            for (size_t j = 0; j < parameterCount; j++)
                            {
                                TM32ASM_DestroyTokenStream(parameters[j]);
                            }
                            free(parameters);
                            TM32ASM_DestroyTokenStream(outputStream);
                            return false;
                        }
                    }
                    
                    i++;
                }
                
                // Expand macro body with parameter substitution
                if (macroSymbol->macroBody != NULL)
                {
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Expanding macro body with %zu tokens", macroSymbol->macroBody->tokenCount);
                    }
                    
                    for (size_t bodyIndex = 0; bodyIndex < macroSymbol->macroBody->tokenCount; bodyIndex++)
                    {
                        TM32ASM_Token* bodyToken = macroSymbol->macroBody->tokens[bodyIndex];
                        bool tokenHandled = false;
                        
                        // Handle .shift directive
                        if (bodyToken->type == TM32ASM_TT_DIRECTIVE && bodyToken->param == TM32ASM_DT_SHIFT)
                        {
                            // Get the shift count from the next token
                            int shiftCount = 1; // Default shift count
                            if (bodyIndex + 1 < macroSymbol->macroBody->tokenCount)
                            {
                                TM32ASM_Token* nextToken = macroSymbol->macroBody->tokens[bodyIndex + 1];
                                if (nextToken != NULL && nextToken->type == TM32ASM_TT_DECIMAL)
                                {
                                    shiftCount = atoi(nextToken->lexeme);
                                    bodyIndex++; // Skip the shift count token
                                }
                            }
                            
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Processing .shift %d directive", shiftCount);
                            }
                            
                            // Shift parameters by removing the specified number from the front
                            if (shiftCount == 0)
                            {
                                // Shift 0 does nothing, but log it for completeness
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("After shift: %zu parameters remaining (no change)", parameterCount);
                                }
                            }
                            else if (shiftCount > 0 && (size_t)shiftCount <= parameterCount)
                            {
                                // Destroy the parameters we're shifting out
                                for (int i = 0; i < shiftCount; i++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[i]);
                                }
                                
                                // Move remaining parameters forward
                                for (size_t i = 0; i < parameterCount - shiftCount; i++)
                                {
                                    parameters[i] = parameters[i + shiftCount];
                                }
                                
                                // Update parameter count
                                parameterCount -= shiftCount;
                                
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("After shift: %zu parameters remaining", parameterCount);
                                }
                            }
                            else if (shiftCount > 0)
                            {
                                // Shift count exceeds available parameters - clear all parameters
                                for (size_t i = 0; i < parameterCount; i++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[i]);
                                }
                                parameterCount = 0;
                                
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("After shift: all parameters removed");
                                }
                            }
                            
                            tokenHandled = true; // Don't copy .shift directive to output
                        }
                        // Check for parameter substitution (@1, @2, etc.)
                        else if (bodyToken->type == TM32ASM_TT_IDENTIFIER && bodyToken->lexeme != NULL &&
                            bodyToken->lexeme[0] == '@' && isdigit(bodyToken->lexeme[1]))
                        {
                            int paramIndex = atoi(bodyToken->lexeme + 1) - 1; // Convert to 0-based
                            if (paramIndex >= 0 && (size_t)paramIndex < parameterCount)
                            {
                                if (preprocessor->verbose)
                                {
                                    TM32ASM_LogInfo("Substituting parameter %d", paramIndex + 1);
                                }
                                
                                // Substitute with parameter tokens
                                TM32ASM_TokenStream* paramStream = parameters[paramIndex];
                                for (size_t j = 0; j < paramStream->tokenCount; j++)
                                {
                                    TM32ASM_Token* copiedToken = TM32ASM_CopyToken(paramStream->tokens[j]);
                                    if (copiedToken == NULL)
                                    {
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                    if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
                                    {
                                        TM32ASM_DestroyToken(copiedToken);
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                }
                                tokenHandled = true;
                            }
                        }
                        // Handle special parameter tokens
                        else if (bodyToken->type == TM32ASM_TT_PARAM_POSITIONAL)
                        {
                            // @1, @2, etc. - extract parameter index from lexeme
                            if (bodyToken->lexeme != NULL && bodyToken->lexeme[0] == '@' && isdigit(bodyToken->lexeme[1]))
                            {
                                int paramIndex = atoi(bodyToken->lexeme + 1) - 1; // Convert to 0-based
                                if (paramIndex >= 0 && (size_t)paramIndex < parameterCount)
                                {
                                    if (preprocessor->verbose)
                                    {
                                        TM32ASM_LogInfo("Substituting positional parameter %d", paramIndex + 1);
                                    }
                                    
                                    // Get the parameter token stream
                                    TM32ASM_TokenStream* paramStream = parameters[paramIndex];
                                    
                                    // Check if merging is needed with previous or next tokens
                                    bool shouldMergePrev = false;
                                    bool shouldMergeNext = false;
                                    
                                    // Check for prefix merging (previous token)
                                    if (bodyIndex > 0)
                                    {
                                        TM32ASM_Token* prevToken = macroSymbol->macroBody->tokens[bodyIndex - 1];
                                        if (prevToken != NULL && prevToken->type == TM32ASM_TT_IDENTIFIER)
                                        {
                                            // Check if this token was already processed (to avoid double-processing)
                                            if (outputStream->tokenCount > 0)
                                            {
                                                TM32ASM_Token* lastToken = outputStream->tokens[outputStream->tokenCount - 1];
                                                if (lastToken != NULL && lastToken->type == TM32ASM_TT_IDENTIFIER &&
                                                    strcmp(lastToken->lexeme, prevToken->lexeme) == 0)
                                                {
                                                    shouldMergePrev = true;
                                                }
                                            }
                                        }
                                    }
                                    
                                    // Check for suffix merging (next token)
                                    if (bodyIndex + 1 < macroSymbol->macroBody->tokenCount)
                                    {
                                        TM32ASM_Token* nextToken = macroSymbol->macroBody->tokens[bodyIndex + 1];
                                        if (nextToken != NULL && nextToken->type == TM32ASM_TT_IDENTIFIER)
                                        {
                                            shouldMergeNext = true;
                                        }
                                    }
                                    
                                    // If no merging is needed and parameter is a single token, preserve its type
                                    if (!shouldMergePrev && !shouldMergeNext && paramStream->tokenCount == 1)
                                    {
                                        TM32ASM_Token* paramToken = paramStream->tokens[0];
                                        TM32ASM_Token* resultToken = TM32ASM_CopyToken(paramToken);
                                        if (resultToken == NULL)
                                        {
                                            for (size_t k = 0; k < parameterCount; k++)
                                            {
                                                TM32ASM_DestroyTokenStream(parameters[k]);
                                            }
                                            free(parameters);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        resultToken->filename = bodyToken->filename;
                                        resultToken->line = bodyToken->line;
                                        
                                        if (TM32ASM_PushTokenBack(outputStream, resultToken) == NULL)
                                        {
                                            TM32ASM_DestroyToken(resultToken);
                                            for (size_t k = 0; k < parameterCount; k++)
                                            {
                                                TM32ASM_DestroyTokenStream(parameters[k]);
                                            }
                                            free(parameters);
                                            TM32ASM_DestroyTokenStream(outputStream);
                                            return false;
                                        }
                                        tokenHandled = true;
                                    }
                                    else
                                    {
                                        // Merging is needed or multiple tokens, use string-based approach
                                        char paramValue[256] = {0};
                                        
                                        // Concatenate all tokens in the parameter to form the substitution string
                                        for (size_t j = 0; j < paramStream->tokenCount; j++)
                                        {
                                            if (j > 0 && strlen(paramValue) + 1 < sizeof(paramValue))
                                            {
                                                strcat(paramValue, " "); // Add space between tokens if needed
                                            }
                                            if (strlen(paramValue) + strlen(paramStream->tokens[j]->lexeme) < sizeof(paramValue))
                                            {
                                                strcat(paramValue, paramStream->tokens[j]->lexeme);
                                            }
                                        }
                                        
                                        // Perform the merging logic
                                        char mergedId[512] = {0};
                                        
                                        if (shouldMergePrev)
                                        {
                                            // Remove the last token since we'll replace it with the merged one
                                            TM32ASM_Token* lastToken = outputStream->tokens[outputStream->tokenCount - 1];
                                            strcpy(mergedId, lastToken->lexeme);
                                            strcat(mergedId, paramValue);
                                            TM32ASM_DestroyToken(lastToken);
                                            outputStream->tokenCount--;
                                            if (preprocessor->verbose)
                                            {
                                                TM32ASM_LogInfo("Merging preceding identifier with @%d: %s + %s -> %s", 
                                                              paramIndex + 1, lastToken->lexeme, paramValue, mergedId);
                                            }
                                        }
                                        else
                                        {
                                            strcpy(mergedId, paramValue);
                                        }
                                        
                                        // Check for suffix merging (next token)
                                        if (shouldMergeNext)
                                        {
                                            TM32ASM_Token* nextToken = macroSymbol->macroBody->tokens[bodyIndex + 1];
                                            // Merge the parameter value with the following identifier
                                            strncat(mergedId, nextToken->lexeme, sizeof(mergedId) - strlen(mergedId) - 1);
                                            if (preprocessor->verbose)
                                            {
                                                TM32ASM_LogInfo("Merging @%d with following identifier: %s -> %s", 
                                                              paramIndex + 1, nextToken->lexeme, mergedId);
                                            }
                                        }
                                    
                                    TM32ASM_Token* resultToken = TM32ASM_CreateToken(mergedId, TM32ASM_TT_IDENTIFIER);
                                    if (resultToken == NULL)
                                    {
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                    resultToken->filename = bodyToken->filename;
                                    resultToken->line = bodyToken->line;
                                    
                                    if (TM32ASM_PushTokenBack(outputStream, resultToken) == NULL)
                                    {
                                        TM32ASM_DestroyToken(resultToken);
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                    
                                    // If we merged with the next token, skip it in the next iteration
                                    if (shouldMergeNext)
                                    {
                                        bodyIndex++; // Skip the next token since we merged it
                                    }
                                    
                                    tokenHandled = true;
                                    }
                                }
                            }
                        }
                        else if (bodyToken->type == TM32ASM_TT_PARAM_MACRO_NAME)
                        {
                            // @0 - expands to macro name, with identifier merging support
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Substituting macro name: %s", macroSymbol->name);
                            }
                            
                            // Check if the previous token is an identifier that should be merged (prefix)
                            char mergedId[512] = {0};
                            bool shouldMergePrev = false;
                            
                            if (bodyIndex > 0)
                            {
                                TM32ASM_Token* prevToken = macroSymbol->macroBody->tokens[bodyIndex - 1];
                                if (prevToken != NULL && prevToken->type == TM32ASM_TT_IDENTIFIER)
                                {
                                    // Check if this token was already processed (to avoid double-processing)
                                    if (outputStream->tokenCount > 0)
                                    {
                                        TM32ASM_Token* lastToken = outputStream->tokens[outputStream->tokenCount - 1];
                                        if (lastToken != NULL && lastToken->type == TM32ASM_TT_IDENTIFIER &&
                                            strcmp(lastToken->lexeme, prevToken->lexeme) == 0)
                                        {
                                            // The previous token was just added, we need to merge with it
                                            strcpy(mergedId, prevToken->lexeme);
                                            strcat(mergedId, macroSymbol->name);
                                            shouldMergePrev = true;
                                            // Remove the last token since we'll replace it with the merged one
                                            TM32ASM_DestroyToken(lastToken);
                                            outputStream->tokenCount--;
                                            if (preprocessor->verbose)
                                            {
                                                TM32ASM_LogInfo("Merging preceding identifier with @0: %s + %s -> %s", 
                                                              prevToken->lexeme, macroSymbol->name, mergedId);
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if (!shouldMergePrev)
                            {
                                strcpy(mergedId, macroSymbol->name);
                            }
                            
                            // Check if the next token is an identifier that should be merged (suffix)
                            bool shouldMergeNext = false;
                            
                            if (bodyIndex + 1 < macroSymbol->macroBody->tokenCount)
                            {
                                TM32ASM_Token* nextToken = macroSymbol->macroBody->tokens[bodyIndex + 1];
                                if (nextToken != NULL && nextToken->type == TM32ASM_TT_IDENTIFIER)
                                {
                                    // Merge the macro name with the following identifier
                                    strncat(mergedId, nextToken->lexeme, sizeof(mergedId) - strlen(mergedId) - 1);
                                    shouldMergeNext = true;
                                    if (preprocessor->verbose)
                                    {
                                        TM32ASM_LogInfo("Merging @0 with following identifier: %s -> %s", 
                                                      nextToken->lexeme, mergedId);
                                    }
                                }
                            }
                            
                            TM32ASM_Token* resultToken = TM32ASM_CreateToken(mergedId, TM32ASM_TT_IDENTIFIER);
                            if (resultToken == NULL)
                            {
                                for (size_t k = 0; k < parameterCount; k++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[k]);
                                }
                                free(parameters);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            resultToken->filename = bodyToken->filename;
                            resultToken->line = bodyToken->line;
                            
                            if (TM32ASM_PushTokenBack(outputStream, resultToken) == NULL)
                            {
                                TM32ASM_DestroyToken(resultToken);
                                for (size_t k = 0; k < parameterCount; k++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[k]);
                                }
                                free(parameters);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            
                            // If we merged with the next token, skip it in the next iteration
                            if (shouldMergeNext)
                            {
                                bodyIndex++; // Skip the next token since we merged it
                            }
                            
                            tokenHandled = true;
                        }
                        else if (bodyToken->type == TM32ASM_TT_PARAM_COUNT)
                        {
                            // @# or @NARG - expands to parameter count, with identifier merging support
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Substituting parameter count: %zu", parameterCount);
                            }
                            
                            char countStr[32];
                            snprintf(countStr, sizeof(countStr), "%zu", parameterCount);
                            
                            // Check if the previous token is an identifier that should be merged (prefix)
                            char mergedId[512] = {0};
                            bool shouldMergePrev = false;
                            
                            if (bodyIndex > 0)
                            {
                                TM32ASM_Token* prevToken = macroSymbol->macroBody->tokens[bodyIndex - 1];
                                if (prevToken != NULL && prevToken->type == TM32ASM_TT_IDENTIFIER)
                                {
                                    // Check if this token was already processed (to avoid double-processing)
                                    if (outputStream->tokenCount > 0)
                                    {
                                        TM32ASM_Token* lastToken = outputStream->tokens[outputStream->tokenCount - 1];
                                        if (lastToken != NULL && lastToken->type == TM32ASM_TT_IDENTIFIER &&
                                            strcmp(lastToken->lexeme, prevToken->lexeme) == 0)
                                        {
                                            // The previous token was just added, we need to merge with it
                                            strcpy(mergedId, prevToken->lexeme);
                                            strcat(mergedId, countStr);
                                            shouldMergePrev = true;
                                            // Remove the last token since we'll replace it with the merged one
                                            TM32ASM_DestroyToken(lastToken);
                                            outputStream->tokenCount--;
                                            if (preprocessor->verbose)
                                            {
                                                TM32ASM_LogInfo("Merging preceding identifier with @#: %s + %s -> %s", 
                                                              prevToken->lexeme, countStr, mergedId);
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if (!shouldMergePrev)
                            {
                                strcpy(mergedId, countStr);
                            }
                            
                            // Check if the next token is an identifier that should be merged (suffix)
                            bool shouldMergeNext = false;
                            TM32ASM_TokenType resultTokenType = TM32ASM_TT_DECIMAL; // Default to decimal if no merging
                            
                            if (bodyIndex + 1 < macroSymbol->macroBody->tokenCount)
                            {
                                TM32ASM_Token* nextToken = macroSymbol->macroBody->tokens[bodyIndex + 1];
                                if (nextToken != NULL && nextToken->type == TM32ASM_TT_IDENTIFIER)
                                {
                                    // Merge the parameter count with the following identifier
                                    strncat(mergedId, nextToken->lexeme, sizeof(mergedId) - strlen(mergedId) - 1);
                                    shouldMergeNext = true;
                                    resultTokenType = TM32ASM_TT_IDENTIFIER; // Result is an identifier
                                    if (preprocessor->verbose)
                                    {
                                        TM32ASM_LogInfo("Merging @# with following identifier: %s -> %s", 
                                                      nextToken->lexeme, mergedId);
                                    }
                                }
                            }
                            
                            // If we merged with any identifier, the result should be an identifier
                            if (shouldMergePrev)
                            {
                                resultTokenType = TM32ASM_TT_IDENTIFIER;
                            }
                            
                            TM32ASM_Token* resultToken = TM32ASM_CreateToken(mergedId, resultTokenType);
                            if (resultToken == NULL)
                            {
                                for (size_t k = 0; k < parameterCount; k++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[k]);
                                }
                                free(parameters);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            resultToken->filename = bodyToken->filename;
                            resultToken->line = bodyToken->line;
                            
                            if (TM32ASM_PushTokenBack(outputStream, resultToken) == NULL)
                            {
                                TM32ASM_DestroyToken(resultToken);
                                for (size_t k = 0; k < parameterCount; k++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[k]);
                                }
                                free(parameters);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            
                            // If we merged with the next token, skip it in the next iteration
                            if (shouldMergeNext)
                            {
                                bodyIndex++; // Skip the next token since we merged it
                            }
                            
                            tokenHandled = true;
                        }
                        else if (bodyToken->type == TM32ASM_TT_PARAM_LIST)
                        {
                            // @* - expands to all parameters separated by commas
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Substituting all parameters");
                            }
                            
                            for (size_t paramIdx = 0; paramIdx < parameterCount; paramIdx++)
                            {
                                if (paramIdx > 0)
                                {
                                    // Add comma separator
                                    TM32ASM_Token* commaToken = TM32ASM_CreateToken(",", TM32ASM_TT_COMMA);
                                    if (commaToken == NULL)
                                    {
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                    commaToken->filename = bodyToken->filename;
                                    commaToken->line = bodyToken->line;
                                    
                                    if (TM32ASM_PushTokenBack(outputStream, commaToken) == NULL)
                                    {
                                        TM32ASM_DestroyToken(commaToken);
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                }
                                
                                // Add parameter tokens
                                TM32ASM_TokenStream* paramStream = parameters[paramIdx];
                                for (size_t j = 0; j < paramStream->tokenCount; j++)
                                {
                                    TM32ASM_Token* copiedToken = TM32ASM_CopyToken(paramStream->tokens[j]);
                                    if (copiedToken == NULL)
                                    {
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                    if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
                                    {
                                        TM32ASM_DestroyToken(copiedToken);
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                }
                            }
                            tokenHandled = true;
                        }
                        else if (bodyToken->type == TM32ASM_TT_PARAM_LIST_NOT_FIRST)
                        {
                            // @- - expands to all parameters except the first, separated by commas
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Substituting all parameters except first");
                            }
                            
                            for (size_t paramIdx = 1; paramIdx < parameterCount; paramIdx++)
                            {
                                if (paramIdx > 1)
                                {
                                    // Add comma separator
                                    TM32ASM_Token* commaToken = TM32ASM_CreateToken(",", TM32ASM_TT_COMMA);
                                    if (commaToken == NULL)
                                    {
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                    commaToken->filename = bodyToken->filename;
                                    commaToken->line = bodyToken->line;
                                    
                                    if (TM32ASM_PushTokenBack(outputStream, commaToken) == NULL)
                                    {
                                        TM32ASM_DestroyToken(commaToken);
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                }
                                
                                // Add parameter tokens
                                TM32ASM_TokenStream* paramStream = parameters[paramIdx];
                                for (size_t j = 0; j < paramStream->tokenCount; j++)
                                {
                                    TM32ASM_Token* copiedToken = TM32ASM_CopyToken(paramStream->tokens[j]);
                                    if (copiedToken == NULL)
                                    {
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                    if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
                                    {
                                        TM32ASM_DestroyToken(copiedToken);
                                        for (size_t k = 0; k < parameterCount; k++)
                                        {
                                            TM32ASM_DestroyTokenStream(parameters[k]);
                                        }
                                        free(parameters);
                                        TM32ASM_DestroyTokenStream(outputStream);
                                        return false;
                                    }
                                }
                            }
                            tokenHandled = true;
                        }
                        else if (bodyToken->type == TM32ASM_TT_PARAM_MANGLE_UNIQUE)
                        {
                            // @? - expands to unique identifier based on macro name and line number
                            if (preprocessor->verbose)
                            {
                                TM32ASM_LogInfo("Substituting unique identifier for macro %s at line %u", 
                                              macroSymbol->name, token->line);
                            }
                            
                            char uniqueId[256];
                            snprintf(uniqueId, sizeof(uniqueId), "_%s_%u", macroSymbol->name, token->line);
                            
                            // Check if the previous token is an identifier that should be merged (prefix)
                            char mergedId[512] = {0};
                            bool shouldMergePrev = false;
                            
                            if (bodyIndex > 0)
                            {
                                TM32ASM_Token* prevToken = macroSymbol->macroBody->tokens[bodyIndex - 1];
                                if (prevToken != NULL && prevToken->type == TM32ASM_TT_IDENTIFIER)
                                {
                                    // Check if this token was already processed (to avoid double-processing)
                                    // We'll mark it as handled by checking the output stream's last token
                                    if (outputStream->tokenCount > 0)
                                    {
                                        TM32ASM_Token* lastToken = outputStream->tokens[outputStream->tokenCount - 1];
                                        if (lastToken != NULL && lastToken->type == TM32ASM_TT_IDENTIFIER &&
                                            strcmp(lastToken->lexeme, prevToken->lexeme) == 0)
                                        {
                                            // The previous token was just added, we need to merge with it
                                            strcpy(mergedId, prevToken->lexeme);
                                            strcat(mergedId, uniqueId);
                                            shouldMergePrev = true;
                                            // Remove the last token since we'll replace it with the merged one
                                            TM32ASM_DestroyToken(lastToken);
                                            outputStream->tokenCount--;
                                            if (preprocessor->verbose)
                                            {
                                                TM32ASM_LogInfo("Merging preceding identifier with @?: %s + %s -> %s", 
                                                              prevToken->lexeme, uniqueId, mergedId);
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if (!shouldMergePrev)
                            {
                                strcpy(mergedId, uniqueId);
                            }
                            
                            // Check if the next token is an identifier that should be merged (suffix)
                            bool shouldMergeNext = false;
                            
                            if (bodyIndex + 1 < macroSymbol->macroBody->tokenCount)
                            {
                                TM32ASM_Token* nextToken = macroSymbol->macroBody->tokens[bodyIndex + 1];
                                if (nextToken != NULL && nextToken->type == TM32ASM_TT_IDENTIFIER)
                                {
                                    // Merge the unique identifier with the following identifier
                                    strncat(mergedId, nextToken->lexeme, sizeof(mergedId) - strlen(mergedId) - 1);
                                    shouldMergeNext = true;
                                    if (preprocessor->verbose)
                                    {
                                        TM32ASM_LogInfo("Merging @? with following identifier: %s -> %s", 
                                                      nextToken->lexeme, mergedId);
                                    }
                                }
                            }
                            
                            TM32ASM_Token* uniqueToken = TM32ASM_CreateToken(mergedId, TM32ASM_TT_IDENTIFIER);
                            if (uniqueToken == NULL)
                            {
                                for (size_t k = 0; k < parameterCount; k++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[k]);
                                }
                                free(parameters);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            uniqueToken->filename = bodyToken->filename;
                            uniqueToken->line = bodyToken->line;
                            
                            if (TM32ASM_PushTokenBack(outputStream, uniqueToken) == NULL)
                            {
                                TM32ASM_DestroyToken(uniqueToken);
                                for (size_t k = 0; k < parameterCount; k++)
                                {
                                    TM32ASM_DestroyTokenStream(parameters[k]);
                                }
                                free(parameters);
                                TM32ASM_DestroyTokenStream(outputStream);
                                return false;
                            }
                            
                            // If we merged with the next token, skip it in the next iteration
                            if (shouldMergeNext)
                            {
                                bodyIndex++; // Skip the next token since we merged it
                            }
                            
                            tokenHandled = true;
                        }
                        
                        if (!tokenHandled)
                        {
                            // Check if this is a string literal that contains parameter placeholders
                            bool hasParameterPlaceholders = false;
                            if (bodyToken->type == TM32ASM_TT_STRING && bodyToken->lexeme != NULL)
                            {
                                // Check for @? or special parameter tokens
                                if (strstr(bodyToken->lexeme, "@?") != NULL ||
                                    strstr(bodyToken->lexeme, "@0") != NULL ||
                                    strstr(bodyToken->lexeme, "@#") != NULL ||
                                    strstr(bodyToken->lexeme, "@*") != NULL ||
                                    strstr(bodyToken->lexeme, "@-") != NULL)
                                {
                                    hasParameterPlaceholders = true;
                                }
                                else
                                {
                                    // Check for positional parameters
                                    for (size_t i = 1; i <= parameterCount; i++)
                                    {
                                        char paramPattern[8];
                                        snprintf(paramPattern, sizeof(paramPattern), "@%zu", i);
                                        if (strstr(bodyToken->lexeme, paramPattern) != NULL)
                                        {
                                            hasParameterPlaceholders = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            
                            if (hasParameterPlaceholders)
                            {
                                // String contains parameter placeholders - need to expand them
                                char expandedString[1024];
                                strcpy(expandedString, bodyToken->lexeme);
                                
                                // First, handle @? placeholders
                                if (strstr(expandedString, "@?") != NULL)
                                {
                                    // Generate unique identifier for @?
                                    char uniqueId[256];
                                    snprintf(uniqueId, sizeof(uniqueId), "_%s_%u", macroSymbol->name, token->line);
                                    
                                    // Replace @? with unique identifier in the string
                                    char* pos = strstr(expandedString, "@?");
                                    while (pos != NULL)
                                    {
                                        // Calculate lengths
                                        size_t beforeLen = pos - expandedString;
                                        size_t afterLen = strlen(pos + 2); // +2 to skip "@?"
                                        
                                        // Create new string with substitution
                                        char newString[1024];
                                        strncpy(newString, expandedString, beforeLen);
                                        newString[beforeLen] = '\0';
                                        strcat(newString, uniqueId);
                                        strcat(newString, pos + 2);
                                        
                                        strcpy(expandedString, newString);
                                        
                                        if (preprocessor->verbose)
                                        {
                                            TM32ASM_LogInfo("Substituted @? in string literal: %s", expandedString);
                                        }
                                        
                                        // Look for next occurrence
                                        pos = strstr(expandedString + beforeLen + strlen(uniqueId), "@?");
                                    }
                                }
                                
                                // Then, handle positional parameters (@1, @2, etc.)
                                for (size_t paramIdx = 1; paramIdx <= parameterCount; paramIdx++)
                                {
                                    char paramPattern[8];
                                    snprintf(paramPattern, sizeof(paramPattern), "@%zu", paramIdx);
                                    
                                    if (strstr(expandedString, paramPattern) != NULL)
                                    {
                                        // Get the parameter value as a single string
                                        TM32ASM_TokenStream* paramStream = parameters[paramIdx - 1];
                                        char paramValue[256] = {0};
                                        
                                        // Concatenate all tokens in the parameter to form the substitution string
                                        for (size_t j = 0; j < paramStream->tokenCount; j++)
                                        {
                                            if (j > 0 && strlen(paramValue) + 1 < sizeof(paramValue))
                                            {
                                                strcat(paramValue, " "); // Add space between tokens
                                            }
                                            if (strlen(paramValue) + strlen(paramStream->tokens[j]->lexeme) < sizeof(paramValue))
                                            {
                                                strcat(paramValue, paramStream->tokens[j]->lexeme);
                                            }
                                        }
                                        
                                        // Replace all occurrences of this parameter pattern
                                        char* pos = strstr(expandedString, paramPattern);
                                        while (pos != NULL)
                                        {
                                            // Calculate lengths
                                            size_t beforeLen = pos - expandedString;
                                            size_t afterLen = strlen(pos + strlen(paramPattern));
                                            
                                            // Create new string with substitution
                                            char newString[1024];
                                            strncpy(newString, expandedString, beforeLen);
                                            newString[beforeLen] = '\0';
                                            strcat(newString, paramValue);
                                            strcat(newString, pos + strlen(paramPattern));
                                            
                                            strcpy(expandedString, newString);
                                            
                                            if (preprocessor->verbose)
                                            {
                                                TM32ASM_LogInfo("Substituted @%zu with '%s' in string literal", paramIdx, paramValue);
                                            }
                                            
                                            // Look for next occurrence of this parameter
                                            pos = strstr(expandedString + beforeLen + strlen(paramValue), paramPattern);
                                        }
                                    }
                                }
                                
                                // Handle @0 (macro name) in string literals
                                if (strstr(expandedString, "@0") != NULL)
                                {
                                    char* pos = strstr(expandedString, "@0");
                                    while (pos != NULL)
                                    {
                                        // Calculate lengths
                                        size_t beforeLen = pos - expandedString;
                                        size_t afterLen = strlen(pos + 2); // +2 to skip "@0"
                                        
                                        // Create new string with substitution
                                        char newString[1024];
                                        strncpy(newString, expandedString, beforeLen);
                                        newString[beforeLen] = '\0';
                                        strcat(newString, macroSymbol->name);
                                        strcat(newString, pos + 2);
                                        
                                        strcpy(expandedString, newString);
                                        
                                        if (preprocessor->verbose)
                                        {
                                            TM32ASM_LogInfo("Substituted @0 with '%s' in string literal", macroSymbol->name);
                                        }
                                        
                                        // Look for next occurrence
                                        pos = strstr(expandedString + beforeLen + strlen(macroSymbol->name), "@0");
                                    }
                                }
                                
                                // Handle @# (parameter count) in string literals
                                if (strstr(expandedString, "@#") != NULL)
                                {
                                    char countStr[32];
                                    snprintf(countStr, sizeof(countStr), "%zu", parameterCount);
                                    
                                    char* pos = strstr(expandedString, "@#");
                                    while (pos != NULL)
                                    {
                                        // Calculate lengths
                                        size_t beforeLen = pos - expandedString;
                                        size_t afterLen = strlen(pos + 2); // +2 to skip "@#"
                                        
                                        // Create new string with substitution
                                        char newString[1024];
                                        strncpy(newString, expandedString, beforeLen);
                                        newString[beforeLen] = '\0';
                                        strcat(newString, countStr);
                                        strcat(newString, pos + 2);
                                        
                                        strcpy(expandedString, newString);
                                        
                                        if (preprocessor->verbose)
                                        {
                                            TM32ASM_LogInfo("Substituted @# with '%s' in string literal", countStr);
                                        }
                                        
                                        // Look for next occurrence
                                        pos = strstr(expandedString + beforeLen + strlen(countStr), "@#");
                                    }
                                }
                                
                                // Handle @* (all parameters) in string literals
                                if (strstr(expandedString, "@*") != NULL)
                                {
                                    // Build string with all parameters separated by commas
                                    char allParams[512] = {0};
                                    for (size_t paramIdx = 0; paramIdx < parameterCount; paramIdx++)
                                    {
                                        if (paramIdx > 0)
                                        {
                                            strcat(allParams, ", ");
                                        }
                                        
                                        TM32ASM_TokenStream* paramStream = parameters[paramIdx];
                                        for (size_t j = 0; j < paramStream->tokenCount; j++)
                                        {
                                            if (j > 0 && strlen(allParams) + 1 < sizeof(allParams))
                                            {
                                                strcat(allParams, " ");
                                            }
                                            if (strlen(allParams) + strlen(paramStream->tokens[j]->lexeme) < sizeof(allParams))
                                            {
                                                strcat(allParams, paramStream->tokens[j]->lexeme);
                                            }
                                        }
                                    }
                                    
                                    char* pos = strstr(expandedString, "@*");
                                    while (pos != NULL)
                                    {
                                        // Calculate lengths
                                        size_t beforeLen = pos - expandedString;
                                        size_t afterLen = strlen(pos + 2); // +2 to skip "@*"
                                        
                                        // Create new string with substitution
                                        char newString[1024];
                                        strncpy(newString, expandedString, beforeLen);
                                        newString[beforeLen] = '\0';
                                        strcat(newString, allParams);
                                        strcat(newString, pos + 2);
                                        
                                        strcpy(expandedString, newString);
                                        
                                        if (preprocessor->verbose)
                                        {
                                            TM32ASM_LogInfo("Substituted @* with '%s' in string literal", allParams);
                                        }
                                        
                                        // Look for next occurrence
                                        pos = strstr(expandedString + beforeLen + strlen(allParams), "@*");
                                    }
                                }
                                
                                // Handle @- (all parameters but first) in string literals
                                if (strstr(expandedString, "@-") != NULL)
                                {
                                    // Build string with all parameters except first, separated by commas
                                    char allButFirst[512] = {0};
                                    for (size_t paramIdx = 1; paramIdx < parameterCount; paramIdx++) // Start from 1 to skip first
                                    {
                                        if (paramIdx > 1)
                                        {
                                            strcat(allButFirst, ", ");
                                        }
                                        
                                        TM32ASM_TokenStream* paramStream = parameters[paramIdx];
                                        for (size_t j = 0; j < paramStream->tokenCount; j++)
                                        {
                                            if (j > 0 && strlen(allButFirst) + 1 < sizeof(allButFirst))
                                            {
                                                strcat(allButFirst, " ");
                                            }
                                            if (strlen(allButFirst) + strlen(paramStream->tokens[j]->lexeme) < sizeof(allButFirst))
                                            {
                                                strcat(allButFirst, paramStream->tokens[j]->lexeme);
                                            }
                                        }
                                    }
                                    
                                    char* pos = strstr(expandedString, "@-");
                                    while (pos != NULL)
                                    {
                                        // Calculate lengths
                                        size_t beforeLen = pos - expandedString;
                                        size_t afterLen = strlen(pos + 2); // +2 to skip "@-"
                                        
                                        // Create new string with substitution
                                        char newString[1024];
                                        strncpy(newString, expandedString, beforeLen);
                                        newString[beforeLen] = '\0';
                                        strcat(newString, allButFirst);
                                        strcat(newString, pos + 2);
                                        
                                        strcpy(expandedString, newString);
                                        
                                        if (preprocessor->verbose)
                                        {
                                            TM32ASM_LogInfo("Substituted @- with '%s' in string literal", allButFirst);
                                        }
                                        
                                        // Look for next occurrence
                                        pos = strstr(expandedString + beforeLen + strlen(allButFirst), "@-");
                                    }
                                }
                                
                                // Create new string token with expanded content
                                TM32ASM_Token* expandedToken = TM32ASM_CreateToken(expandedString, TM32ASM_TT_STRING);
                                if (expandedToken == NULL)
                                {
                                    for (size_t k = 0; k < parameterCount; k++)
                                    {
                                        TM32ASM_DestroyTokenStream(parameters[k]);
                                    }
                                    free(parameters);
                                    TM32ASM_DestroyTokenStream(outputStream);
                                    return false;
                                }
                                expandedToken->filename = bodyToken->filename;
                                expandedToken->line = bodyToken->line;
                                
                                if (TM32ASM_PushTokenBack(outputStream, expandedToken) == NULL)
                                {
                                    TM32ASM_DestroyToken(expandedToken);
                                    for (size_t k = 0; k < parameterCount; k++)
                                    {
                                        TM32ASM_DestroyTokenStream(parameters[k]);
                                    }
                                    free(parameters);
                                    TM32ASM_DestroyTokenStream(outputStream);
                                    return false;
                                }
                                tokenHandled = true;
                            }
                            
                            if (!tokenHandled)
                            {
                                // Copy body token as-is
                                TM32ASM_Token* copiedToken = TM32ASM_CopyToken(bodyToken);
                                if (copiedToken == NULL)
                                {
                                    for (size_t k = 0; k < parameterCount; k++)
                                    {
                                        TM32ASM_DestroyTokenStream(parameters[k]);
                                    }
                                    free(parameters);
                                    TM32ASM_DestroyTokenStream(outputStream);
                                    return false;
                                }
                                if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
                                {
                                    TM32ASM_DestroyToken(copiedToken);
                                    for (size_t k = 0; k < parameterCount; k++)
                                    {
                                        TM32ASM_DestroyTokenStream(parameters[k]);
                                    }
                                    free(parameters);
                                    TM32ASM_DestroyTokenStream(outputStream);
                                    return false;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Macro body is NULL!");
                    }
                }
                
                // Clean up parameters
                for (size_t j = 0; j < parameterCount; j++)
                {
                    TM32ASM_DestroyTokenStream(parameters[j]);
                }
                free(parameters);
                
                continue;
            }
            else if (macroSymbol != NULL && macroSymbol->type == TM32ASM_ST_MACRO_SIMPLE)
            {
                if (preprocessor->verbose)
                {
                    TM32ASM_LogInfo("Found simple macro '%s', expanding...", token->lexeme);
                }
                
                expansionOccurred = true; // Mark that an expansion occurred
                // Simple text substitution macro
                skipToken = true;
                
                // Determine the appropriate token type based on the stored value
                TM32ASM_TokenType tokenType = TM32ASM_TT_IDENTIFIER;
                const char* valueToUse = macroSymbol->value;
                
                if (valueToUse != NULL && strlen(valueToUse) >= 2 && 
                    valueToUse[0] == '"' && valueToUse[strlen(valueToUse) - 1] == '"')
                {
                    // String literal - keep as string token
                    tokenType = TM32ASM_TT_STRING;
                }
                else if (valueToUse != NULL && strlen(valueToUse) >= 3 &&
                         valueToUse[0] == '\'' && valueToUse[strlen(valueToUse) - 1] == '\'')
                {
                    // Character literal
                    tokenType = TM32ASM_TT_CHARACTER;
                }
                else if (valueToUse != NULL && 
                         ((isdigit(valueToUse[0])) ||
                          (valueToUse[0] == '-' && isdigit(valueToUse[1])) ||
                          (valueToUse[0] == '0' && valueToUse[1] == 'x')))
                {
                    // Numeric literal (decimal or hexadecimal)
                    if (valueToUse[0] == '0' && valueToUse[1] == 'x')
                    {
                        tokenType = TM32ASM_TT_HEXADECIMAL;
                    }
                    else
                    {
                        tokenType = TM32ASM_TT_DECIMAL;
                    }
                }
                // else it's an identifier (default)
                
                TM32ASM_Token* valueToken = TM32ASM_CreateToken(valueToUse, tokenType);
                if (valueToken == NULL)
                {
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                valueToken->filename = token->filename;
                valueToken->line = token->line;
                
                if (TM32ASM_PushTokenBack(outputStream, valueToken) == NULL)
                {
                    TM32ASM_DestroyToken(valueToken);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                continue;
            }
        }
        
        // Copy non-macro tokens
        if (!skipToken)
        {
            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
            if (copiedToken == NULL)
            {
                TM32ASM_LogError("Could not copy token during macro expansion pass");
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
            {
                TM32ASM_LogError("Could not add token to output stream during macro expansion pass");
                TM32ASM_DestroyToken(copiedToken);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
        }
    }
    
        // Prepare for next iteration if needed
        if (expansionOccurred && iterationCount < maxIterations)
        {
            // Clean up previous iteration's input (except the original)
            if (finalOutput != NULL && finalOutput != preprocessor->inputTokenStream)
            {
                TM32ASM_DestroyTokenStream(finalOutput);
            }
            finalOutput = outputStream;
            workingInput = outputStream;
        }
        else
        {
            // This is the final iteration
            finalOutput = outputStream;
        }
    }
    
    // Check for too many iterations (possible infinite recursion)
    if (iterationCount >= maxIterations)
    {
        TM32ASM_LogWarn("Macro expansion stopped after %d iterations - possible infinite recursion", maxIterations);
    }
    
    // Set the final output token stream
    if (preprocessor->outputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputTokenStream);
    }
    preprocessor->outputTokenStream = finalOutput;
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Macro expansion pass completed");
    }
    
    return true;
}

static bool TM32ASM_PassVariableEvaluation (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting variable and constant evaluation pass");
    }
    
    // Create output token stream
    TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
    if (outputStream == NULL)
    {
        TM32ASM_LogError("Could not create output token stream for variable evaluation pass");
        return false;
    }
    
    // Process all tokens from input to output, substituting constants and variables
    for (size_t i = 0; i < preprocessor->inputTokenStream->tokenCount; i++)
    {
        TM32ASM_Token* token = preprocessor->inputTokenStream->tokens[i];
        TM32ASM_Token* outputToken = NULL;
        
        // Check if this is an identifier that should be substituted
        if (token->type == TM32ASM_TT_IDENTIFIER)
        {
            // Look up the symbol
            TM32ASM_Symbol* symbol = TM32ASM_FindSymbol(preprocessor, token->lexeme);
            if (symbol != NULL && (symbol->type == TM32ASM_ST_CONSTANT || symbol->type == TM32ASM_ST_VARIABLE))
            {
                // Substitute with the symbol's value
                if (symbol->value != NULL && strcmp(symbol->value, "UNEVALUATED") != 0)
                {
                    // Parse the value as an integer and create a decimal token
                    int64_t value = strtoll(symbol->value, NULL, 0);
                    
                    // Create a decimal token with the value
                    char valueStr[32];
                    snprintf(valueStr, sizeof(valueStr), "%lld", (long long)value);
                    
                    outputToken = TM32ASM_CreateToken(valueStr, TM32ASM_TT_DECIMAL);
                    if (outputToken != NULL)
                    {
                        outputToken->filename = token->filename;
                        outputToken->line = token->line;
                        
                        if (preprocessor->verbose)
                        {
                            TM32ASM_LogInfo("Substituted %s with value %lld at line %u", 
                                          token->lexeme, (long long)value, token->line);
                        }
                    }
                }
                else
                {
                    // Symbol value not available or unevaluated - copy as is
                    outputToken = TM32ASM_CopyToken(token);
                    
                    if (preprocessor->verbose)
                    {
                        TM32ASM_LogInfo("Symbol %s has no value or is unevaluated - copying as identifier", token->lexeme);
                    }
                }
            }
            else
            {
                // Not a constant or variable - copy as is
                outputToken = TM32ASM_CopyToken(token);
            }
        }
        else
        {
            // Not an identifier - copy as is
            outputToken = TM32ASM_CopyToken(token);
        }
        
        // Add the token to the output stream
        if (outputToken == NULL)
        {
            TM32ASM_LogError("Could not create output token during variable evaluation pass");
            TM32ASM_DestroyTokenStream(outputStream);
            return false;
        }
        
        if (TM32ASM_PushTokenBack(outputStream, outputToken) == NULL)
        {
            TM32ASM_LogError("Could not add token to output stream during variable evaluation pass");
            TM32ASM_DestroyToken(outputToken);
            TM32ASM_DestroyTokenStream(outputStream);
            return false;
        }
    }
    
    // Set the output token stream
    if (preprocessor->outputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputTokenStream);
    }
    preprocessor->outputTokenStream = outputStream;
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Variable evaluation pass completed with %zu tokens", outputStream->tokenCount);
    }
    
    return true;
}

static bool TM32ASM_PassControlFlow (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting control flow and loop processing pass");
    }
    
    // Reset control flow stack for this pass
    preprocessor->controlFlowDepth = 0;
    
    // Create output token stream
    TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
    if (outputStream == NULL)
    {
        TM32ASM_LogError("Could not create output token stream for control flow pass");
        return false;
    }
    
    // Process all tokens in the input stream
    for (size_t i = 0; i < preprocessor->inputTokenStream->tokenCount; i++)
    {
        TM32ASM_Token* token = preprocessor->inputTokenStream->tokens[i];
        bool shouldEmitToken = true;
        bool skipToken = false;
        
        // Check if this is a control flow directive
        if (token->type == TM32ASM_TT_DIRECTIVE)
        {
            if (token->param == TM32ASM_DT_IF)
            {
                if (!TM32ASM_ProcessIfDirective(preprocessor, token, &i))
                {
                    TM32ASM_LogError("Failed to process .if directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .if token itself
            }
            else if (token->param == TM32ASM_DT_ELSEIF)
            {
                if (!TM32ASM_ProcessElifDirective(preprocessor, token, &i))
                {
                    TM32ASM_LogError("Failed to process .elif directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .elif token itself
            }
            else if (token->param == TM32ASM_DT_ELSE)
            {
                if (!TM32ASM_ProcessElseDirective(preprocessor, token, &i))
                {
                    TM32ASM_LogError("Failed to process .else directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .else token itself
            }
            else if (token->param == TM32ASM_DT_ENDIF)
            {
                if (!TM32ASM_ProcessEndifDirective(preprocessor, token, &i))
                {
                    TM32ASM_LogError("Failed to process .endif directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .endif token itself
            }
            else if (token->param == TM32ASM_DT_REPEAT)
            {
                if (!TM32ASM_ProcessRepeatDirective(preprocessor, token, &i, outputStream))
                {
                    TM32ASM_LogError("Failed to process .repeat directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .repeat token itself
            }
            else if (token->param == TM32ASM_DT_ENDREPEAT)
            {
                if (!TM32ASM_ProcessEndrepeatDirective(preprocessor, token, &i))
                {
                    TM32ASM_LogError("Failed to process .endrepeat directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .endrepeat token itself
            }
            else if (token->param == TM32ASM_DT_WHILE)
            {
                if (!TM32ASM_ProcessWhileDirective(preprocessor, token, &i, outputStream))
                {
                    TM32ASM_LogError("Failed to process .while directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .while token itself
            }
            else if (token->param == TM32ASM_DT_ENDWHILE)
            {
                if (!TM32ASM_ProcessEndwhileDirective(preprocessor, token, &i))
                {
                    TM32ASM_LogError("Failed to process .endwhile directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .endwhile token itself
            }
            else if (token->param == TM32ASM_DT_FOR)
            {
                if (!TM32ASM_ProcessForDirective(preprocessor, token, &i, outputStream))
                {
                    TM32ASM_LogError("Failed to process .for directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .for token itself
            }
            else if (token->param == TM32ASM_DT_ENDFOR)
            {
                if (!TM32ASM_ProcessEndforDirective(preprocessor, token, &i))
                {
                    TM32ASM_LogError("Failed to process .endfor directive at line %u", token->line);
                    TM32ASM_DestroyTokenStream(outputStream);
                    return false;
                }
                skipToken = true; // Don't emit the .endfor token itself
            }
            // Note: .break and .continue directives are handled within loop processing functions
            // and should not be encountered during normal control flow processing
        }
        
        // Check if we should emit this token based on current control flow state
        if (!skipToken)
        {
            shouldEmitToken = TM32ASM_ShouldEmitCode(preprocessor);
        }
        
        // Emit the token if it should be included
        if (shouldEmitToken && !skipToken)
        {
            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
            if (copiedToken == NULL)
            {
                TM32ASM_LogError("Could not copy token during control flow pass");
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
            
            if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
            {
                TM32ASM_LogError("Could not add token to output stream during control flow pass");
                TM32ASM_DestroyToken(copiedToken);
                TM32ASM_DestroyTokenStream(outputStream);
                return false;
            }
        }
    }
    
    // Check for unclosed control flow blocks
    if (preprocessor->controlFlowDepth > 0)
    {
        TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
        if (context != NULL)
        {
            const char* typeName = "unknown";
            switch (context->type)
            {
            case TM32ASM_CFT_CONDITIONAL:
                typeName = ".if";
                break;
            case TM32ASM_CFT_REPEAT:
                typeName = ".repeat";
                break;
            case TM32ASM_CFT_WHILE:
                typeName = ".while";
                break;
            case TM32ASM_CFT_FOR:
                typeName = ".for";
                break;
            }
            
            TM32ASM_LogError("Unclosed %s block starting at line %u in %s", 
                            typeName, context->startLine, context->filename);
        }
        
        TM32ASM_DestroyTokenStream(outputStream);
        return false;
    }
    
    // Set the output token stream
    if (preprocessor->outputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputTokenStream);
    }
    preprocessor->outputTokenStream = outputStream;
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Control flow pass completed with %zu tokens", outputStream->tokenCount);
    }
    
    return true;
}

static bool TM32ASM_PassFinalization (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting final token stream generation pass");
    }
    
    // The finalization pass should process the tokens that have been processed by all previous passes
    // At this point, inputTokenStream contains the output from all previous passes
    if (preprocessor->inputTokenStream != NULL)
    {
        for (size_t i = 0; i < preprocessor->inputTokenStream->tokenCount; i++)
        {
            TM32ASM_Token* token = preprocessor->inputTokenStream->tokens[i];
            if (token != NULL)
            {
                // Copy the token to avoid shared ownership
                TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
                if (copiedToken == NULL)
                {
                    TM32ASM_LogError("Failed to copy token");
                    return false;
                }
                
                if (TM32ASM_InsertToken(preprocessor->outputTokenStream, copiedToken) == NULL)
                {
                    TM32ASM_DestroyToken(copiedToken);
                    TM32ASM_LogError("Failed to insert copied token to output stream");
                    return false;
                }
            }
        }
    }
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Finalization pass completed with %zu tokens", 
                        preprocessor->outputTokenStream->tokenCount);
    }
    
    return true;
}

static bool TM32ASM_PassNewlineOptimization (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting newline optimization pass");
    }
    
    // Count input tokens for logging
    size_t inputTokenCount = preprocessor->inputTokenStream ? preprocessor->inputTokenStream->tokenCount : 0;
    
    // Process tokens from input stream, reducing consecutive newlines
    if (preprocessor->inputTokenStream != NULL)
    {
        bool previousWasNewline = false;
        size_t optimizedCount = 0;
        
        for (size_t i = 0; i < preprocessor->inputTokenStream->tokenCount; i++)
        {
            TM32ASM_Token* token = preprocessor->inputTokenStream->tokens[i];
            if (token == NULL) continue;
            
            // Check if this is a newline token
            bool isNewline = (token->type == TM32ASM_TT_NEWLINE);
            
            // Skip consecutive newlines (keep only the first one)
            if (isNewline && previousWasNewline)
            {
                optimizedCount++;
                continue; // Skip this newline token
            }
            
            // Copy the token to output stream
            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
            if (copiedToken == NULL)
            {
                TM32ASM_LogError("Failed to copy token during newline optimization");
                return false;
            }
            
            if (TM32ASM_InsertToken(preprocessor->outputTokenStream, copiedToken) == NULL)
            {
                TM32ASM_DestroyToken(copiedToken);
                TM32ASM_LogError("Failed to insert token during newline optimization");
                return false;
            }
            
            // Update state for next iteration
            previousWasNewline = isNewline;
        }
        
        if (preprocessor->verbose)
        {
            TM32ASM_LogInfo("Newline optimization completed: %zu tokens processed, %zu consecutive newlines removed, %zu tokens output", 
                           inputTokenCount, optimizedCount, preprocessor->outputTokenStream->tokenCount);
        }
    }
    
    return true;
}

/* Public Functions ***********************************************************/

TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ()
{
    TM32ASM_Preprocessor* preprocessor =
        TM32ASM_CreateZero(1, TM32ASM_Preprocessor);
    if (preprocessor == NULL)
    {
        TM32ASM_LogErrno("Could not allocate TM32ASM preprocessor");
        return NULL;
    }

    if (!TM32ASM_InitializePreprocessor(preprocessor))
    {
        TM32ASM_LogError("Failed to initialize preprocessor");
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }

    return preprocessor;
}

void TM32ASM_DestroyPreprocessor (
    TM32ASM_Preprocessor* preprocessor
)
{
    if (preprocessor != NULL)
    {
        TM32ASM_CleanupPreprocessor(preprocessor);
        TM32ASM_Destroy(preprocessor);
    }
}

bool TM32ASM_AddIncludePath (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             path
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(path, false);
    
    // Allocate memory for path copy
    char* pathCopy = TM32ASM_Create(strlen(path) + 1, char);
    if (pathCopy == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for include path");
        return false;
    }
    strcpy(pathCopy, path);
    
    // Add to include paths array
    preprocessor->includePaths[preprocessor->includePathCount] = pathCopy;
    preprocessor->includePathCount++;
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Added include path: %s", path);
    }
    
    return true;
}

bool TM32ASM_DefineSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value,
    TM32ASM_SymbolType      type
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(name, false);
    
    // Check if symbol already exists
    TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, name);
    if (existingSymbol != NULL)
    {
        TM32ASM_LogError("Symbol '%s' is already defined", name);
        return false;
    }
    
    // Create new symbol
    TM32ASM_Symbol symbol = {0};
    symbol.name = (char*)name;  // Temporary assignment for AddSymbol
    symbol.value = (char*)value;
    symbol.type = type;
    symbol.parameterCount = 0;
    symbol.parameters = NULL;
    symbol.isDefined = true;
    symbol.line = 0;  // Command-line defined
    symbol.filename = "<command-line>";
    
    if (!TM32ASM_AddSymbol(preprocessor, &symbol))
    {
        TM32ASM_LogError("Failed to add symbol '%s'", name);
        return false;
    }
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Defined symbol: %s = %s", name, value ? value : "<empty>");
    }
    
    return true;
}

bool TM32ASM_SetInputTokenStream (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(tokenStream, false);
    
    preprocessor->inputTokenStream = tokenStream;
    
    // Create output token stream
    preprocessor->outputTokenStream = TM32ASM_CreateTokenStream();
    if (preprocessor->outputTokenStream == NULL)
    {
        TM32ASM_LogError("Failed to create output token stream");
        return false;
    }
    
    return true;
}

bool TM32ASM_ProcessTokenStream (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(preprocessor->inputTokenStream, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting preprocessor with %zu input tokens", 
                        preprocessor->inputTokenStream->tokenCount);
    }
    
    // First, run the file inclusion pass iteratively until no more includes are found
    size_t inclusionIteration = 1;
    const size_t maxInclusionIterations = 100; // Prevent infinite loops
    bool includesFound = true;
    
    preprocessor->currentPass = TM32ASM_PP_FILE_INCLUSION;
    
    while (includesFound && inclusionIteration <= maxInclusionIterations)
    {
        if (preprocessor->verbose)
        {
            TM32ASM_LogInfo("Executing File Inclusion pass (iteration %zu)", inclusionIteration);
        }
        
        if (!TM32ASM_PassFileInclusion(preprocessor))
        {
            TM32ASM_LogError("File Inclusion pass failed on iteration %zu", inclusionIteration);
            preprocessor->errorOccurred = true;
            return false;
        }
        
        if (preprocessor->errorOccurred)
        {
            TM32ASM_LogError("Error occurred during File Inclusion pass iteration %zu", inclusionIteration);
            return false;
        }
        
        // Check if any .include directives remain for the next iteration
        // This is normal - included files may contain their own .include directives
        includesFound = false;
        if (preprocessor->outputTokenStream != NULL)
        {
            // Look for .include directives in the output stream that need processing
            for (size_t j = 0; j < preprocessor->outputTokenStream->tokenCount; j++)
            {
                TM32ASM_Token* token = preprocessor->outputTokenStream->tokens[j];
                if (token != NULL && 
                    token->type == TM32ASM_TT_DIRECTIVE && 
                    token->param == TM32ASM_DT_INCLUDE)
                {
                    includesFound = true;
                    TM32ASM_LogInfo("Found .include directive at line %u for next iteration", token->line);
                    break;
                }
            }
        }
        
        // If no more directives found, we're done with file inclusion
        if (!includesFound)
        {
            TM32ASM_LogInfo("No more .include directives found - file inclusion complete after %zu iterations", inclusionIteration);
            break;
        }
        
        TM32ASM_LogInfo("Found remaining .include directives - continuing to iteration %zu", inclusionIteration + 1);
        
        // CRITICAL FIX: Chain output to input for next iteration
        // The output of this iteration becomes the input for the next iteration
        if (preprocessor->outputTokenStream != NULL)
        {
            // Destroy the old input stream (unless it's the original input on first iteration)
            if (preprocessor->inputTokenStream != NULL)
            {
                TM32ASM_DestroyTokenStream(preprocessor->inputTokenStream);
            }
            
            // Make the output of this iteration the input to the next iteration
            preprocessor->inputTokenStream = preprocessor->outputTokenStream;
            preprocessor->outputTokenStream = TM32ASM_CreateTokenStream();
            
            if (preprocessor->outputTokenStream == NULL)
            {
                TM32ASM_LogError("Failed to create output token stream for next iteration");
                preprocessor->errorOccurred = true;
                return false;
            }
        }
    }
    
    if (inclusionIteration > maxInclusionIterations)
    {
        TM32ASM_LogError("Maximum file inclusion iterations reached (%zu). Possible circular includes.", 
                         maxInclusionIterations);
        preprocessor->errorOccurred = true;
        return false;
    }
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("File inclusion completed after %zu iterations", inclusionIteration);
    }
    
    // CRITICAL FIX: After file inclusion, make output stream the input for remaining passes
    if (preprocessor->outputTokenStream != NULL)
    {
        // Destroy the old input stream
        if (preprocessor->inputTokenStream != NULL)
        {
            TM32ASM_DestroyTokenStream(preprocessor->inputTokenStream);
        }
        
        // Make the file inclusion output the input for remaining passes
        preprocessor->inputTokenStream = preprocessor->outputTokenStream;
        preprocessor->outputTokenStream = TM32ASM_CreateTokenStream();
        
        if (preprocessor->outputTokenStream == NULL)
        {
            TM32ASM_LogError("Failed to create output token stream for remaining passes");
            preprocessor->errorOccurred = true;
            return false;
        }
    }
    
    // Execute remaining preprocessor passes in sequence
    static const struct {
        TM32ASM_PreprocessorPass pass;
        bool (*function)(TM32ASM_Preprocessor*);
        const char* name;
    } remainingPasses[] = {
        { TM32ASM_PP_SYMBOL_DECLARATION,     TM32ASM_PassSymbolDeclaration,     "Symbol Declaration" },
        { TM32ASM_PP_MACRO_EXPANSION,        TM32ASM_PassMacroExpansion,        "Macro Expansion" },
        { TM32ASM_PP_VARIABLE_EVALUATION,    TM32ASM_PassVariableEvaluation,    "Variable Evaluation" },
        { TM32ASM_PP_CONTROL_FLOW,           TM32ASM_PassControlFlow,           "Control Flow Processing" },
        { TM32ASM_PP_FINALIZATION,           TM32ASM_PassFinalization,          "Finalization" },
        { TM32ASM_PP_NEWLINE_OPTIMIZATION,   TM32ASM_PassNewlineOptimization,   "Newline Optimization" }
    };
    
    const size_t numRemainingPasses = sizeof(remainingPasses) / sizeof(remainingPasses[0]);
    
    for (size_t i = 0; i < numRemainingPasses; i++)
    {
        preprocessor->currentPass = remainingPasses[i].pass;
        
        if (preprocessor->verbose)
        {
            TM32ASM_LogInfo("Executing pass %zu: %s", i + 2, remainingPasses[i].name);
        }
        
        if (!remainingPasses[i].function(preprocessor))
        {
            TM32ASM_LogError("Preprocessor pass '%s' failed", remainingPasses[i].name);
            preprocessor->errorOccurred = true;
            return false;
        }
        
        if (preprocessor->errorOccurred)
        {
            TM32ASM_LogError("Error occurred during pass '%s'", remainingPasses[i].name);
            return false;
        }
        
        // Chain passes together: output of this pass becomes input to next pass
        if (i < numRemainingPasses - 1 && preprocessor->outputTokenStream != NULL)
        {
            // Destroy the old input stream
            if (preprocessor->inputTokenStream != NULL)
            {
                TM32ASM_DestroyTokenStream(preprocessor->inputTokenStream);
            }
            
            // Make the output of this pass the input to the next pass
            preprocessor->inputTokenStream = preprocessor->outputTokenStream;
            preprocessor->outputTokenStream = TM32ASM_CreateTokenStream();
            
            if (preprocessor->outputTokenStream == NULL)
            {
                TM32ASM_LogError("Failed to create output token stream for next pass");
                preprocessor->errorOccurred = true;
                return false;
            }
        }
    }
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Preprocessing completed successfully, output has %zu tokens",
                        preprocessor->outputTokenStream->tokenCount);
    }
    
    return true;
}

TM32ASM_TokenStream* TM32ASM_GetOutputTokenStream (
    const TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    return preprocessor->outputTokenStream;
}

void TM32ASM_SetPreprocessorOptions (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    warningsAsErrors,
    bool                    verbose
)
{
    if (preprocessor != NULL)
    {
        preprocessor->warningsAsErrors = warningsAsErrors;
        preprocessor->verbose = verbose;
        
        if (verbose)
        {
            TM32ASM_LogInfo("Preprocessor options set: warnings as errors = %s, verbose = %s",
                            warningsAsErrors ? "true" : "false",
                            verbose ? "true" : "false");
        }
    }
}

/* Control Flow Helper Functions *********************************************/

static bool TM32ASM_PushControlFlowContext (
    TM32ASM_Preprocessor*       preprocessor,
    TM32ASM_ControlFlowType     type,
    uint32_t                    line,
    const char*                 filename
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->controlFlowDepth >= TM32ASM_MAX_LOOP_NESTING)
    {
        TM32ASM_LogError("Control flow nesting too deep (maximum %d levels)", TM32ASM_MAX_LOOP_NESTING);
        return false;
    }
    
    TM32ASM_ControlFlowContext* context = &preprocessor->controlFlowStack[preprocessor->controlFlowDepth];
    
    // Initialize the context
    context->type = type;
    context->isActive = true;
    context->startLine = line;
    context->filename = filename;
    
    // Initialize type-specific data
    switch (type)
    {
    case TM32ASM_CFT_CONDITIONAL:
        context->data.conditional.conditionMet = false;
        context->data.conditional.inElseBlock = false;
        break;
    case TM32ASM_CFT_REPEAT:
        context->data.repeat.count = 0;
        context->data.repeat.current = 0;
        context->data.repeat.iterationVar = NULL;
        break;
    case TM32ASM_CFT_WHILE:
        context->data.whileLoop.condition = NULL;
        break;
    case TM32ASM_CFT_FOR:
        context->data.forLoop.variable = NULL;
        context->data.forLoop.start = NULL;
        context->data.forLoop.end = NULL;
        context->data.forLoop.step = NULL;
        context->data.forLoop.current = 0;
        break;
    }
    
    preprocessor->controlFlowDepth++;
    return true;
}

static bool TM32ASM_PopControlFlowContext (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->controlFlowDepth == 0)
    {
        TM32ASM_LogError("Attempted to pop control flow context from empty stack");
        return false;
    }
    
    preprocessor->controlFlowDepth--;
    
    // Clean up any dynamically allocated data in the context
    TM32ASM_ControlFlowContext* context = &preprocessor->controlFlowStack[preprocessor->controlFlowDepth];
    
    switch (context->type)
    {
    case TM32ASM_CFT_REPEAT:
        if (context->data.repeat.iterationVar != NULL)
        {
            free(context->data.repeat.iterationVar);
            context->data.repeat.iterationVar = NULL;
        }
        break;
    case TM32ASM_CFT_WHILE:
        if (context->data.whileLoop.condition != NULL)
        {
            free(context->data.whileLoop.condition);
            context->data.whileLoop.condition = NULL;
        }
        break;
    case TM32ASM_CFT_FOR:
        if (context->data.forLoop.variable != NULL)
        {
            free(context->data.forLoop.variable);
            context->data.forLoop.variable = NULL;
        }
        if (context->data.forLoop.start != NULL)
        {
            free(context->data.forLoop.start);
            context->data.forLoop.start = NULL;
        }
        if (context->data.forLoop.end != NULL)
        {
            free(context->data.forLoop.end);
            context->data.forLoop.end = NULL;
        }
        if (context->data.forLoop.step != NULL)
        {
            free(context->data.forLoop.step);
            context->data.forLoop.step = NULL;
        }
        break;
    default:
        // No cleanup needed for conditional contexts
        break;
    }
    
    return true;
}

static TM32ASM_ControlFlowContext* TM32ASM_GetCurrentControlFlowContext (
    TM32ASM_Preprocessor* preprocessor
)
{
    if (preprocessor == NULL || preprocessor->controlFlowDepth == 0)
    {
        return NULL;
    }
    
    return &preprocessor->controlFlowStack[preprocessor->controlFlowDepth - 1];
}

static bool TM32ASM_ShouldEmitCode (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, true);
    
    // Check all nested contexts - code should only be emitted if all are active
    for (size_t i = 0; i < preprocessor->controlFlowDepth; i++)
    {
        TM32ASM_ControlFlowContext* context = &preprocessor->controlFlowStack[i];
        
        if (!context->isActive)
        {
            return false;
        }
        
        // For conditional contexts, check if we're in an active branch
        if (context->type == TM32ASM_CFT_CONDITIONAL)
        {
            // If we're in an else block, we should emit if no condition was met yet
            // If we're not in an else block, we should emit if the current condition was met
            if (context->data.conditional.inElseBlock)
            {
                if (context->data.conditional.conditionMet)
                {
                    return false; // A previous if/elif was true, so skip else
                }
            }
            else
            {
                // We're in an if/elif block - only emit if this condition is true
                // This will be handled by the individual directive processors
            }
        }
    }
    
    return true;
}

static bool TM32ASM_ProcessIfDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    TM32ASM_ReturnValueIfNull(tokenIndex, false);
    
    // Push a new conditional context
    if (!TM32ASM_PushControlFlowContext(preprocessor, TM32ASM_CFT_CONDITIONAL, 
                                        token->line, token->filename))
    {
        return false;
    }
    
    // Extract condition tokens from the current position to end of line
    TM32ASM_TokenStream* conditionTokens = TM32ASM_CreateTokenStream();
    if (conditionTokens == NULL)
    {
        TM32ASM_LogError("Could not create token stream for .if condition");
        TM32ASM_PopControlFlowContext(preprocessor);
        return false;
    }
    
    // Collect tokens until newline or end of stream
    size_t startIndex = *tokenIndex + 1; // Skip the .if token
    size_t endIndex = startIndex;
    
    while (endIndex < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[endIndex];
        if (currentToken->type == TM32ASM_TT_NEWLINE)
        {
            break;
        }
        endIndex++;
    }
    
    // Copy condition tokens
    for (size_t i = startIndex; i < endIndex; i++)
    {
        TM32ASM_Token* sourceToken = preprocessor->inputTokenStream->tokens[i];
        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(sourceToken);
        if (copiedToken == NULL)
        {
            TM32ASM_LogError("Could not copy condition token for .if directive");
            TM32ASM_DestroyTokenStream(conditionTokens);
            TM32ASM_PopControlFlowContext(preprocessor);
            return false;
        }
        
        if (TM32ASM_PushTokenBack(conditionTokens, copiedToken) == NULL)
        {
            TM32ASM_LogError("Could not add condition token for .if directive");
            TM32ASM_DestroyToken(copiedToken);
            TM32ASM_DestroyTokenStream(conditionTokens);
            TM32ASM_PopControlFlowContext(preprocessor);
            return false;
        }
    }
    
    // Evaluate the condition
    int64_t result = 0;
    bool conditionTrue = false;
    
    if (conditionTokens->tokenCount > 0)
    {
        if (TM32ASM_EvaluateExpression(preprocessor, conditionTokens, 0, 
                                       conditionTokens->tokenCount, &result))
        {
            conditionTrue = (result != 0);
        }
        else
        {
            TM32ASM_LogError("Could not evaluate .if condition");
            TM32ASM_DestroyTokenStream(conditionTokens);
            TM32ASM_PopControlFlowContext(preprocessor);
            return false;
        }
    }
    else
    {
        TM32ASM_LogError(".if directive requires a condition expression");
        TM32ASM_DestroyTokenStream(conditionTokens);
        TM32ASM_PopControlFlowContext(preprocessor);
        return false;
    }
    
    TM32ASM_DestroyTokenStream(conditionTokens);
    
    // Update the context based on condition result
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    if (context != NULL)
    {
        context->isActive = conditionTrue;
        context->data.conditional.conditionMet = conditionTrue;
    }
    
    // Update token index to skip past the condition tokens
    *tokenIndex = endIndex - 1; // -1 because the main loop will increment
    
    return true;
}

static bool TM32ASM_ProcessElifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    TM32ASM_ReturnValueIfNull(tokenIndex, false);
    
    // Check if we're in a conditional context
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    if (context == NULL || context->type != TM32ASM_CFT_CONDITIONAL)
    {
        TM32ASM_LogError(".elif directive without matching .if at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    if (context->data.conditional.inElseBlock)
    {
        TM32ASM_LogError(".elif directive after .else at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // If a previous condition was already met, skip this elif
    if (context->data.conditional.conditionMet)
    {
        context->isActive = false;
        
        // Skip to end of line
        size_t endIndex = *tokenIndex + 1;
        while (endIndex < preprocessor->inputTokenStream->tokenCount)
        {
            TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[endIndex];
            if (currentToken->type == TM32ASM_TT_NEWLINE)
            {
                break;
            }
            endIndex++;
        }
        *tokenIndex = endIndex - 1;
        return true;
    }
    
    // Extract and evaluate condition (same logic as .if)
    TM32ASM_TokenStream* conditionTokens = TM32ASM_CreateTokenStream();
    if (conditionTokens == NULL)
    {
        TM32ASM_LogError("Could not create token stream for .elif condition");
        return false;
    }
    
    // Collect tokens until newline or end of stream
    size_t startIndex = *tokenIndex + 1; // Skip the .elif token
    size_t endIndex = startIndex;
    
    while (endIndex < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[endIndex];
        if (currentToken->type == TM32ASM_TT_NEWLINE)
        {
            break;
        }
        endIndex++;
    }
    
    // Copy condition tokens
    for (size_t i = startIndex; i < endIndex; i++)
    {
        TM32ASM_Token* sourceToken = preprocessor->inputTokenStream->tokens[i];
        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(sourceToken);
        if (copiedToken == NULL)
        {
            TM32ASM_LogError("Could not copy condition token for .elif directive");
            TM32ASM_DestroyTokenStream(conditionTokens);
            return false;
        }
        
        if (TM32ASM_PushTokenBack(conditionTokens, copiedToken) == NULL)
        {
            TM32ASM_LogError("Could not add condition token for .elif directive");
            TM32ASM_DestroyToken(copiedToken);
            TM32ASM_DestroyTokenStream(conditionTokens);
            return false;
        }
    }
    
    // Evaluate the condition
    int64_t result = 0;
    bool conditionTrue = false;
    
    if (conditionTokens->tokenCount > 0)
    {
        if (TM32ASM_EvaluateExpression(preprocessor, conditionTokens, 0, 
                                       conditionTokens->tokenCount, &result))
        {
            conditionTrue = (result != 0);
        }
        else
        {
            TM32ASM_LogError("Could not evaluate .elif condition");
            TM32ASM_DestroyTokenStream(conditionTokens);
            return false;
        }
    }
    else
    {
        TM32ASM_LogError(".elif directive requires a condition expression");
        TM32ASM_DestroyTokenStream(conditionTokens);
        return false;
    }
    
    TM32ASM_DestroyTokenStream(conditionTokens);
    
    // Update the context based on condition result
    context->isActive = conditionTrue;
    if (conditionTrue)
    {
        context->data.conditional.conditionMet = true;
    }
    
    // Update token index to skip past the condition tokens
    *tokenIndex = endIndex - 1; // -1 because the main loop will increment
    
    return true;
}

static bool TM32ASM_ProcessElseDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Check if we're in a conditional context
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    if (context == NULL || context->type != TM32ASM_CFT_CONDITIONAL)
    {
        TM32ASM_LogError(".else directive without matching .if at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    if (context->data.conditional.inElseBlock)
    {
        TM32ASM_LogError("Multiple .else directives in same .if block at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // Mark that we're now in the else block
    context->data.conditional.inElseBlock = true;
    
    // The else block is active only if no previous condition was met
    context->isActive = !context->data.conditional.conditionMet;
    
    return true;
}

static bool TM32ASM_ProcessEndifDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Check if we're in a conditional context
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    if (context == NULL || context->type != TM32ASM_CFT_CONDITIONAL)
    {
        TM32ASM_LogError(".endif directive without matching .if at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // Pop the conditional context
    return TM32ASM_PopControlFlowContext(preprocessor);
}

/* Loop Directive Processing Functions ***************************************/

static bool TM32ASM_ProcessRepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex,
    TM32ASM_TokenStream*    outputStream
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    TM32ASM_ReturnValueIfNull(tokenIndex, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    
    // Parse repeat count and optional iteration variable first
    // Expected format: .repeat <count>[, <iteration_var>]
    size_t currentPos = *tokenIndex + 1;
    int64_t repeatCount = 0;
    char* iterationVar = NULL;
    
    // Parse repeat count expression
    size_t exprStart = currentPos;
    size_t exprEnd = exprStart;
    
    // Find end of count expression (comma or newline)
    while (exprEnd < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* exprToken = preprocessor->inputTokenStream->tokens[exprEnd];
        if (exprToken->type == TM32ASM_TT_COMMA ||
            exprToken->type == TM32ASM_TT_NEWLINE ||
            exprToken->type == TM32ASM_TT_DIRECTIVE)
        {
            break;
        }
        exprEnd++;
    }
    
    if (exprEnd > exprStart)
    {
        if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputTokenStream, 
                                        exprStart, exprEnd, &repeatCount))
        {
            TM32ASM_LogError("Failed to evaluate repeat count expression at line %u", token->line);
            return false;
        }
    }
    else
    {
        TM32ASM_LogError("Missing repeat count expression in .repeat directive at line %u", token->line);
        return false;
    }
    
    if (repeatCount < 0)
    {
        TM32ASM_LogError("Repeat count cannot be negative at line %u", token->line);
        return false;
    }
    
    // Check for optional iteration variable
    size_t bodyStartPos = exprEnd;
    if (exprEnd < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* nextToken = preprocessor->inputTokenStream->tokens[exprEnd];
        if (nextToken->type == TM32ASM_TT_COMMA && exprEnd + 1 < preprocessor->inputTokenStream->tokenCount)
        {
            TM32ASM_Token* varToken = preprocessor->inputTokenStream->tokens[exprEnd + 1];
            if (varToken->type == TM32ASM_TT_IDENTIFIER)
            {
                iterationVar = TM32ASM_DuplicateString(varToken->lexeme);
                bodyStartPos = exprEnd + 2;  // Skip comma and variable name
            }
        }
    }
    
    // Skip to first token after newline to find body start
    while (bodyStartPos < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* bodyToken = preprocessor->inputTokenStream->tokens[bodyStartPos];
        if (bodyToken->type == TM32ASM_TT_NEWLINE)
        {
            bodyStartPos++;  // Move past newline to body content
            break;
        }
        bodyStartPos++;
    }
    
    // Find the .endrepeat directive to determine the loop body
    size_t startBodyIndex = bodyStartPos;
    size_t endBodyIndex = 0;
    size_t i = startBodyIndex;
    size_t nestingLevel = 1;
    
    // Find matching .endrepeat
    while (i < preprocessor->inputTokenStream->tokenCount && nestingLevel > 0)
    {
        TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[i];
        
        if (currentToken->type == TM32ASM_TT_DIRECTIVE)
        {
            if (currentToken->param == TM32ASM_DT_REPEAT)
            {
                nestingLevel++;
            }
            else if (currentToken->param == TM32ASM_DT_ENDREPEAT)
            {
                nestingLevel--;
                if (nestingLevel == 0)
                {
                    endBodyIndex = i;
                    break;
                }
            }
        }
        i++;
    }
    
    if (nestingLevel > 0)
    {
        TM32ASM_LogError("Unclosed .repeat directive at line %u in %s", 
                         token->line, token->filename);
        if (iterationVar) TM32ASM_Destroy(iterationVar);
        return false;
    }
    
    // Push control flow context for the repeat loop
    if (!TM32ASM_PushControlFlowContext(preprocessor, TM32ASM_CFT_REPEAT, 
                                        token->line, token->filename))
    {
        TM32ASM_LogError("Failed to create control flow context for .repeat directive");
        if (iterationVar) TM32ASM_Destroy(iterationVar);
        return false;
    }
    
    // Set up repeat context data
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    context->data.repeat.count = repeatCount;
    context->data.repeat.current = 0;
    if (iterationVar != NULL)
    {
        context->data.repeat.iterationVar = TM32ASM_DuplicateString(iterationVar);
    }
    
    // Process the loop iterations and emit tokens directly to output
    for (int64_t iteration = 0; iteration < repeatCount; iteration++)
    {
        // Update context current iteration
        context->data.repeat.current = iteration;
        
        // Set iteration variable if provided
        if (iterationVar != NULL)
        {
            char iterValue[32];
            snprintf(iterValue, sizeof(iterValue), "%lld", (long long)iteration);
            
            // Find existing symbol to update or create new one
            TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, iterationVar);
            if (existingSymbol != NULL)
            {
                // Update existing symbol value
                if (existingSymbol->value != NULL)
                {
                    TM32ASM_Destroy(existingSymbol->value);
                }
                existingSymbol->value = TM32ASM_DuplicateString(iterValue);
            }
            else
            {
                // Create new symbol
                TM32ASM_DefineSymbol(preprocessor, iterationVar, iterValue, TM32ASM_ST_VARIABLE);
            }
        }
        
        // Copy loop body tokens to output, handling .break and .continue
        bool breakRequested = false;
        bool continueRequested = false;
        
        if (!TM32ASM_ProcessLoopBody(preprocessor, startBodyIndex, endBodyIndex, outputStream, 
                                     &breakRequested, &continueRequested))
        {
            TM32ASM_LogError("Failed to process loop body in .repeat directive at line %u", token->line);
            TM32ASM_PopControlFlowContext(preprocessor);
            if (iterationVar) TM32ASM_Destroy(iterationVar);
            return false;
        }
        
        // Handle break and continue
        if (breakRequested)
        {
            break; // Exit the repeat loop
        }
        
        // For continue, we just proceed to the next iteration (no special handling needed)
    }
    
    // Pop the control flow context
    TM32ASM_PopControlFlowContext(preprocessor);
    
    if (iterationVar)
    {
        TM32ASM_Destroy(iterationVar);
    }
    
    // Skip to end of loop
    *tokenIndex = endBodyIndex;
    
    return true;
}

static bool TM32ASM_ProcessEndrepeatDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Check if we're in a repeat context
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    if (context == NULL || context->type != TM32ASM_CFT_REPEAT)
    {
        TM32ASM_LogError(".endrepeat directive without matching .repeat at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // This should not be reached in normal execution since .repeat processes its own body
    TM32ASM_LogError("Unexpected .endrepeat directive at line %u - .repeat should handle its own termination", 
                     token->line);
    return false;
}

static bool TM32ASM_ProcessWhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex,
    TM32ASM_TokenStream*    outputStream
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    TM32ASM_ReturnValueIfNull(tokenIndex, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    
    // Parse condition expression first: .while <condition>
    size_t currentPos = *tokenIndex + 1;
    
    // Find the end of the condition expression (newline or directive)
    size_t conditionStart = currentPos;
    size_t conditionEnd = conditionStart;
    
    while (conditionEnd < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* condToken = preprocessor->inputTokenStream->tokens[conditionEnd];
        if (condToken->type == TM32ASM_TT_NEWLINE ||
            condToken->type == TM32ASM_TT_DIRECTIVE)
        {
            break;
        }
        conditionEnd++;
    }
    
    if (conditionEnd <= conditionStart)
    {
        TM32ASM_LogError("Missing condition expression in .while directive at line %u", token->line);
        return false;
    }
    
    // Skip to first token after newline to find body start
    size_t bodyStartPos = conditionEnd;
    while (bodyStartPos < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* bodyToken = preprocessor->inputTokenStream->tokens[bodyStartPos];
        if (bodyToken->type == TM32ASM_TT_NEWLINE)
        {
            bodyStartPos++;  // Move past newline to body content
            break;
        }
        bodyStartPos++;
    }
    
    // Find the .endwhile directive to determine the loop body
    size_t startBodyIndex = bodyStartPos;
    size_t endBodyIndex = 0;
    size_t i = startBodyIndex;
    size_t nestingLevel = 1;
    
    // Find matching .endwhile
    while (i < preprocessor->inputTokenStream->tokenCount && nestingLevel > 0)
    {
        TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[i];
        
        if (currentToken->type == TM32ASM_TT_DIRECTIVE)
        {
            if (currentToken->param == TM32ASM_DT_WHILE)
            {
                nestingLevel++;
            }
            else if (currentToken->param == TM32ASM_DT_ENDWHILE)
            {
                nestingLevel--;
                if (nestingLevel == 0)
                {
                    endBodyIndex = i;
                    break;
                }
            }
        }
        i++;
    }
    
    if (nestingLevel > 0)
    {
        TM32ASM_LogError("Unclosed .while directive at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // Push control flow context for the while loop
    if (!TM32ASM_PushControlFlowContext(preprocessor, TM32ASM_CFT_WHILE, 
                                        token->line, token->filename))
    {
        TM32ASM_LogError("Failed to create control flow context for .while directive");
        return false;
    }
    
    // Execute the while loop - evaluate condition and emit body tokens
    int64_t iterationCount = 0;
    const int64_t maxIterations = 1000; // Reasonable safety limit to prevent infinite loops
    
    while (iterationCount < maxIterations)
    {
        // Evaluate the condition expression
        int64_t conditionResult = 0;
        if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputTokenStream, 
                                        conditionStart, conditionEnd, &conditionResult))
        {
            TM32ASM_LogError("Failed to evaluate condition expression in .while directive at line %u", token->line);
            TM32ASM_PopControlFlowContext(preprocessor);
            return false;
        }
        
        // If condition is false (0), exit the loop
        if (conditionResult == 0)
        {
            break;
        }
        
        // Copy loop body tokens to output, handling .break and .continue
        bool breakRequested = false;
        bool continueRequested = false;
        
        if (!TM32ASM_ProcessLoopBody(preprocessor, startBodyIndex, endBodyIndex, outputStream, 
                                     &breakRequested, &continueRequested))
        {
            TM32ASM_LogError("Failed to process loop body in .while directive at line %u", token->line);
            TM32ASM_PopControlFlowContext(preprocessor);
            return false;
        }
        
        // Handle break and continue
        if (breakRequested)
        {
            break; // Exit the while loop
        }
        
        // For continue, we just proceed to the next iteration (no special handling needed)
        
        iterationCount++;
    }
    
    if (iterationCount >= maxIterations)
    {
        TM32ASM_LogError("While loop exceeded maximum iteration limit (%lld) at line %u - possible infinite loop", 
                         (long long)maxIterations, token->line);
        TM32ASM_PopControlFlowContext(preprocessor);
        return false;
    }
    
    if (preprocessor->verbose && iterationCount > 0)
    {
        TM32ASM_LogInfo("While loop at line %u executed %lld iterations", 
                        token->line, (long long)iterationCount);
    }
    
    // Pop the control flow context
    TM32ASM_PopControlFlowContext(preprocessor);
    
    // Skip to end of loop
    *tokenIndex = endBodyIndex;
    
    return true;
}

static bool TM32ASM_ProcessEndwhileDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Check if we're in a while context
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    if (context == NULL || context->type != TM32ASM_CFT_WHILE)
    {
        TM32ASM_LogError(".endwhile directive without matching .while at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // This should not be reached in normal execution since .while processes its own body
    TM32ASM_LogError("Unexpected .endwhile directive at line %u - .while should handle its own termination", 
                     token->line);
    return false;
}

static bool TM32ASM_ProcessForDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex,
    TM32ASM_TokenStream*    outputStream
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    TM32ASM_ReturnValueIfNull(tokenIndex, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    
    // Find the .endfor directive to determine the loop body
    size_t startBodyIndex = *tokenIndex + 1;
    size_t endBodyIndex = 0;
    size_t i = startBodyIndex;
    size_t nestingLevel = 1;
    
    // Find matching .endfor
    while (i < preprocessor->inputTokenStream->tokenCount && nestingLevel > 0)
    {
        TM32ASM_Token* currentToken = preprocessor->inputTokenStream->tokens[i];
        
        if (currentToken->type == TM32ASM_TT_DIRECTIVE)
        {
            if (currentToken->param == TM32ASM_DT_FOR)
            {
                nestingLevel++;
            }
            else if (currentToken->param == TM32ASM_DT_ENDFOR)
            {
                nestingLevel--;
                if (nestingLevel == 0)
                {
                    endBodyIndex = i;
                    break;
                }
            }
        }
        i++;
    }
    
    if (nestingLevel > 0)
    {
        TM32ASM_LogError("Unclosed .for directive at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // Parse for directive: .for <var> = <start> to <end> [step <step>]
    size_t currentPos = *tokenIndex + 1;
    
    // Get variable name
    if (currentPos >= preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_LogError("Missing variable name in .for directive at line %u", token->line);
        return false;
    }
    
    TM32ASM_Token* varToken = preprocessor->inputTokenStream->tokens[currentPos];
    if (varToken->type != TM32ASM_TT_IDENTIFIER)
    {
        TM32ASM_LogError("Expected variable name in .for directive at line %u", token->line);
        return false;
    }
    
    char* loopVar = TM32ASM_DuplicateString(varToken->lexeme);
    currentPos++;
    
    // Expect '=' token
    if (currentPos >= preprocessor->inputTokenStream->tokenCount ||
        preprocessor->inputTokenStream->tokens[currentPos]->type != TM32ASM_TT_ASSIGN_EQUAL)
    {
        TM32ASM_LogError("Expected '=' after variable name in .for directive at line %u", token->line);
        TM32ASM_Destroy(loopVar);
        return false;
    }
    currentPos++;
    
    // Parse start expression
    size_t startExprStart = currentPos;
    size_t startExprEnd = currentPos;
    
    // Find 'to' keyword
    while (startExprEnd < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* exprToken = preprocessor->inputTokenStream->tokens[startExprEnd];
        if (exprToken->type == TM32ASM_TT_KEYWORD && exprToken->param == TM32ASM_MKT_TO)
        {
            break;
        }
        startExprEnd++;
    }
    
    if (startExprEnd >= preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_LogError("Missing 'to' keyword in .for directive at line %u", token->line);
        TM32ASM_Destroy(loopVar);
        return false;
    }
    
    int64_t startValue = 0;
    if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputTokenStream, 
                                    startExprStart, startExprEnd, &startValue))
    {
        TM32ASM_LogError("Failed to evaluate start expression in .for directive at line %u", token->line);
        TM32ASM_Destroy(loopVar);
        return false;
    }
    
    currentPos = startExprEnd + 1; // Skip 'to' keyword
    
    // Parse end expression
    size_t endExprStart = currentPos;
    size_t endExprEnd = currentPos;
    
    // Find end of end expression (step keyword or newline)
    while (endExprEnd < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* exprToken = preprocessor->inputTokenStream->tokens[endExprEnd];
        if ((exprToken->type == TM32ASM_TT_KEYWORD && exprToken->param == TM32ASM_MKT_STEP) ||
            exprToken->type == TM32ASM_TT_NEWLINE ||
            exprToken->type == TM32ASM_TT_DIRECTIVE)
        {
            break;
        }
        endExprEnd++;
    }
    
    int64_t endValue = 0;
    if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputTokenStream, 
                                    endExprStart, endExprEnd, &endValue))
    {
        TM32ASM_LogError("Failed to evaluate end expression in .for directive at line %u", token->line);
        TM32ASM_Destroy(loopVar);
        return false;
    }
    
    // Parse optional step value
    int64_t stepValue = 1;
    if (endExprEnd < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* stepToken = preprocessor->inputTokenStream->tokens[endExprEnd];
        if (stepToken->type == TM32ASM_TT_KEYWORD && stepToken->param == TM32ASM_MKT_STEP)
        {
            size_t stepExprStart = endExprEnd + 1;
            size_t stepExprEnd = stepExprStart;
            
            // Find end of step expression
            while (stepExprEnd < preprocessor->inputTokenStream->tokenCount)
            {
                TM32ASM_Token* exprToken = preprocessor->inputTokenStream->tokens[stepExprEnd];
                if (exprToken->type == TM32ASM_TT_NEWLINE ||
                    exprToken->type == TM32ASM_TT_DIRECTIVE)
                {
                    break;
                }
                stepExprEnd++;
            }
            
            if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputTokenStream, 
                                            stepExprStart, stepExprEnd, &stepValue))
            {
                TM32ASM_LogError("Failed to evaluate step expression in .for directive at line %u", token->line);
                TM32ASM_Destroy(loopVar);
                return false;
            }
        }
    }
    
    if (stepValue == 0)
    {
        TM32ASM_LogError("Step value cannot be zero in .for directive at line %u", token->line);
        TM32ASM_Destroy(loopVar);
        return false;
    }
    
    // Now that we've parsed all parameters, find the actual body start
    // We need to recalculate startBodyIndex to skip past all the parameters
    size_t bodyStartPos = currentPos;
    
    // Find the first newline after all parameters to locate body start
    while (bodyStartPos < preprocessor->inputTokenStream->tokenCount)
    {
        TM32ASM_Token* bodyToken = preprocessor->inputTokenStream->tokens[bodyStartPos];
        if (bodyToken->type == TM32ASM_TT_NEWLINE)
        {
            bodyStartPos++;  // Move past newline to body content
            break;
        }
        bodyStartPos++;
    }
    
    // Update startBodyIndex to the correct position
    startBodyIndex = bodyStartPos;
    
    // Push control flow context for the for loop
    if (!TM32ASM_PushControlFlowContext(preprocessor, TM32ASM_CFT_FOR, 
                                        token->line, token->filename))
    {
        TM32ASM_LogError("Failed to create control flow context for .for directive");
        TM32ASM_Destroy(loopVar);
        return false;
    }
    
    // Set up for loop context data
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    context->data.forLoop.variable = TM32ASM_DuplicateString(loopVar);
    context->data.forLoop.current = startValue;
    
    // Execute the loop and emit tokens directly
    int64_t currentValue = startValue;
    bool shouldContinue = (stepValue > 0) ? (currentValue <= endValue) : (currentValue >= endValue);
    
    while (shouldContinue)
    {
        // Update context current value
        context->data.forLoop.current = currentValue;
        
        // Set loop variable value
        char iterValue[32];
        snprintf(iterValue, sizeof(iterValue), "%lld", (long long)currentValue);
        
        // Find existing symbol to update or create new one
        TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, loopVar);
        if (existingSymbol != NULL)
        {
            // Update existing symbol value
            if (existingSymbol->value != NULL)
            {
                TM32ASM_Destroy(existingSymbol->value);
            }
            existingSymbol->value = TM32ASM_DuplicateString(iterValue);
        }
        else
        {
            // Create new symbol
            TM32ASM_DefineSymbol(preprocessor, loopVar, iterValue, TM32ASM_ST_VARIABLE);
        }
        
        // Copy loop body tokens to output, handling .break and .continue
        bool breakRequested = false;
        bool continueRequested = false;
        
        if (!TM32ASM_ProcessLoopBody(preprocessor, startBodyIndex, endBodyIndex, outputStream, 
                                     &breakRequested, &continueRequested))
        {
            TM32ASM_LogError("Failed to process loop body in .for directive at line %u", token->line);
            TM32ASM_PopControlFlowContext(preprocessor);
            TM32ASM_Destroy(loopVar);
            return false;
        }
        
        // Handle break and continue
        if (breakRequested)
        {
            break; // Exit the for loop
        }
        
        // For continue, we just proceed to the next iteration (no special handling needed)
        
        // Update current value
        currentValue += stepValue;
        shouldContinue = (stepValue > 0) ? (currentValue <= endValue) : (currentValue >= endValue);
    }
    
    // Pop the control flow context
    TM32ASM_PopControlFlowContext(preprocessor);
    
    TM32ASM_Destroy(loopVar);
    
    // Skip to end of loop
    *tokenIndex = endBodyIndex;
    
    return true;
}

static bool TM32ASM_ProcessEndforDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Check if we're in a for context
    TM32ASM_ControlFlowContext* context = TM32ASM_GetCurrentControlFlowContext(preprocessor);
    if (context == NULL || context->type != TM32ASM_CFT_FOR)
    {
        TM32ASM_LogError(".endfor directive without matching .for at line %u in %s", 
                         token->line, token->filename);
        return false;
    }
    
    // This should not be reached in normal execution since .for processes its own body
    TM32ASM_LogError("Unexpected .endfor directive at line %u - .for should handle its own termination", 
                     token->line);
    return false;
}

/**
 * @brief   Processes a loop body, handling .break and .continue directives.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   startBodyIndex  Start index of the loop body in the input token stream.
 * @param   endBodyIndex    End index of the loop body in the input token stream.
 * @param   outputStream    The output token stream to emit processed tokens to.
 * @param   breakRequested  Pointer to bool that will be set to true if .break is encountered.
 * @param   continueRequested Pointer to bool that will be set to true if .continue is encountered.
 *
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessLoopBody (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  startBodyIndex,
    size_t                  endBodyIndex,
    TM32ASM_TokenStream*    outputStream,
    bool*                   breakRequested,
    bool*                   continueRequested
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(outputStream, false);
    TM32ASM_ReturnValueIfNull(breakRequested, false);
    TM32ASM_ReturnValueIfNull(continueRequested, false);
    
    *breakRequested = false;
    *continueRequested = false;
    
    // Create a temporary token stream for the loop body
    TM32ASM_TokenStream* bodyTokenStream = TM32ASM_CreateTokenStream();
    if (bodyTokenStream == NULL)
    {
        TM32ASM_LogError("Failed to create temporary token stream for loop body processing");
        return false;
    }
    
    // Copy the loop body tokens to the temporary stream
    for (size_t j = startBodyIndex; j < endBodyIndex; j++)
    {
        TM32ASM_Token* originalToken = preprocessor->inputTokenStream->tokens[j];
        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(originalToken);
        if (copiedToken == NULL)
        {
            TM32ASM_LogError("Could not copy token during loop body processing");
            TM32ASM_DestroyTokenStream(bodyTokenStream);
            return false;
        }
        
        if (TM32ASM_PushTokenBack(bodyTokenStream, copiedToken) == NULL)
        {
            TM32ASM_LogError("Could not add token to body token stream during loop body processing");
            TM32ASM_DestroyToken(copiedToken);
            TM32ASM_DestroyTokenStream(bodyTokenStream);
            return false;
        }
    }
    
    // Temporarily swap the input token stream to process the loop body
    TM32ASM_TokenStream* originalInput = preprocessor->inputTokenStream;
    preprocessor->inputTokenStream = bodyTokenStream;
    
    // Process the loop body with the control flow pass
    size_t i = 0;
    while (i < bodyTokenStream->tokenCount)
    {
        TM32ASM_Token* token = bodyTokenStream->tokens[i];
        bool skipToken = false;
        bool shouldEmitToken = true;
        
        // Handle control flow directives
        if (token->type == TM32ASM_TT_DIRECTIVE)
        {
            if (token->param == TM32ASM_DT_BREAK)
            {
                // Find the current loop context
                TM32ASM_ControlFlowContext* loopContext = NULL;
                for (int32_t k = preprocessor->controlFlowDepth - 1; k >= 0; k--)
                {
                    TM32ASM_ControlFlowContext* context = &preprocessor->controlFlowStack[k];
                    if (context->type == TM32ASM_CFT_REPEAT || 
                        context->type == TM32ASM_CFT_WHILE || 
                        context->type == TM32ASM_CFT_FOR)
                    {
                        loopContext = context;
                        break;
                    }
                }
                
                if (loopContext == NULL)
                {
                    TM32ASM_LogError(".break directive at line %u is not within a loop construct", token->line);
                    preprocessor->inputTokenStream = originalInput;
                    TM32ASM_DestroyTokenStream(bodyTokenStream);
                    return false;
                }
                
                *breakRequested = true;
                preprocessor->inputTokenStream = originalInput;
                TM32ASM_DestroyTokenStream(bodyTokenStream);
                return true; // Exit loop body processing
            }
            else if (token->param == TM32ASM_DT_CONTINUE)
            {
                // Find the current loop context
                TM32ASM_ControlFlowContext* loopContext = NULL;
                for (int32_t k = preprocessor->controlFlowDepth - 1; k >= 0; k--)
                {
                    TM32ASM_ControlFlowContext* context = &preprocessor->controlFlowStack[k];
                    if (context->type == TM32ASM_CFT_REPEAT || 
                        context->type == TM32ASM_CFT_WHILE || 
                        context->type == TM32ASM_CFT_FOR)
                    {
                        loopContext = context;
                        break;
                    }
                }
                
                if (loopContext == NULL)
                {
                    TM32ASM_LogError(".continue directive at line %u is not within a loop construct", token->line);
                    preprocessor->inputTokenStream = originalInput;
                    TM32ASM_DestroyTokenStream(bodyTokenStream);
                    return false;
                }
                
                *continueRequested = true;
                preprocessor->inputTokenStream = originalInput;
                TM32ASM_DestroyTokenStream(bodyTokenStream);
                return true; // Skip rest of current iteration
            }
            else if (token->param == TM32ASM_DT_REPEAT)
            {
                if (!TM32ASM_ProcessRepeatDirective(preprocessor, token, &i, outputStream))
                {
                    TM32ASM_LogError("Failed to process nested .repeat directive at line %u", token->line);
                    preprocessor->inputTokenStream = originalInput;
                    TM32ASM_DestroyTokenStream(bodyTokenStream);
                    return false;
                }
                skipToken = true;
            }
            else if (token->param == TM32ASM_DT_WHILE)
            {
                if (!TM32ASM_ProcessWhileDirective(preprocessor, token, &i, outputStream))
                {
                    TM32ASM_LogError("Failed to process nested .while directive at line %u", token->line);
                    preprocessor->inputTokenStream = originalInput;
                    TM32ASM_DestroyTokenStream(bodyTokenStream);
                    return false;
                }
                skipToken = true;
            }
            else if (token->param == TM32ASM_DT_FOR)
            {
                if (!TM32ASM_ProcessForDirective(preprocessor, token, &i, outputStream))
                {
                    TM32ASM_LogError("Failed to process nested .for directive at line %u", token->line);
                    preprocessor->inputTokenStream = originalInput;
                    TM32ASM_DestroyTokenStream(bodyTokenStream);
                    return false;
                }
                skipToken = true;
            }
        }
        
        // Emit the token if it should be included
        if (shouldEmitToken && !skipToken)
        {
            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
            if (copiedToken == NULL)
            {
                TM32ASM_LogError("Could not copy token during loop body processing");
                preprocessor->inputTokenStream = originalInput;
                TM32ASM_DestroyTokenStream(bodyTokenStream);
                return false;
            }
            
            if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
            {
                TM32ASM_LogError("Could not add token to output stream during loop body processing");
                TM32ASM_DestroyToken(copiedToken);
                preprocessor->inputTokenStream = originalInput;
                TM32ASM_DestroyTokenStream(bodyTokenStream);
                return false;
            }
        }
        
        if (!skipToken)
        {
            i++;
        }
    }
    
    // Restore the original input token stream
    preprocessor->inputTokenStream = originalInput;
    TM32ASM_DestroyTokenStream(bodyTokenStream);
    
    return true;
}

static bool TM32ASM_ProcessBreakDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Find the current loop context
    TM32ASM_ControlFlowContext* loopContext = NULL;
    for (int32_t i = preprocessor->controlFlowDepth - 1; i >= 0; i--)
    {
        TM32ASM_ControlFlowContext* context = &preprocessor->controlFlowStack[i];
        if (context->type == TM32ASM_CFT_REPEAT || 
            context->type == TM32ASM_CFT_WHILE || 
            context->type == TM32ASM_CFT_FOR)
        {
            loopContext = context;
            break;
        }
    }
    
    if (loopContext == NULL)
    {
        TM32ASM_LogError(".break directive at line %u is not within a loop construct", token->line);
        return false;
    }
    
    // Break directives are handled by the loop processing functions themselves
    // This function should not be called during normal control flow processing
    TM32ASM_LogError("Internal error: .break directive at line %u should be handled by loop processing", token->line);
    return false;
}

static bool TM32ASM_ProcessContinueDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          token,
    size_t*                 tokenIndex
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(token, false);
    
    // Find the current loop context
    TM32ASM_ControlFlowContext* loopContext = NULL;
    for (int32_t i = preprocessor->controlFlowDepth - 1; i >= 0; i--)
    {
        TM32ASM_ControlFlowContext* context = &preprocessor->controlFlowStack[i];
        if (context->type == TM32ASM_CFT_REPEAT || 
            context->type == TM32ASM_CFT_WHILE || 
            context->type == TM32ASM_CFT_FOR)
        {
            loopContext = context;
            break;
        }
    }
    
    if (loopContext == NULL)
    {
        TM32ASM_LogError(".continue directive at line %u is not within a loop construct", token->line);
        return false;
    }
    
    // Continue directives are handled by the loop processing functions themselves
    // This function should not be called during normal control flow processing
    TM32ASM_LogError("Internal error: .continue directive at line %u should be handled by loop processing", token->line);
    return false;
}

/* Expression Evaluation Functions *******************************************/

/**
 * @brief   Forward declaration for recursive expression parsing.
 */
static bool TM32ASM_ParseExpression (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
);

/**
 * @brief   Parses a primary expression (numbers, identifiers, parentheses).
 */
static bool TM32ASM_ParsePrimary (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (*currentIndex >= endIndex || *currentIndex >= tokenStream->tokenCount)
    {
        TM32ASM_LogError("Unexpected end of expression");
        return false;
    }
    
    TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
    (*currentIndex)++;
    
    switch (token->type)
    {
        case TM32ASM_TT_DECIMAL:
        case TM32ASM_TT_HEXADECIMAL:
        case TM32ASM_TT_BINARY:
        case TM32ASM_TT_OCTAL:
            *result = (int64_t)TM32ASM_GetIntegerLexeme(token);
            return true;
            
        case TM32ASM_TT_CHARACTER:
        {
            const char* lexeme = token->lexeme;
            if (lexeme != NULL && strlen(lexeme) >= 3 && lexeme[0] == '\'' && lexeme[strlen(lexeme) - 1] == '\'')
            {
                if (lexeme[1] == '\\' && strlen(lexeme) == 4)
                {
                    // Escape sequences
                    switch (lexeme[2])
                    {
                        case 'n': *result = '\n'; break;
                        case 't': *result = '\t'; break;
                        case 'r': *result = '\r'; break;
                        case '0': *result = '\0'; break;
                        case '\\': *result = '\\'; break;
                        case '\'': *result = '\''; break;
                        default: *result = lexeme[2]; break;
                    }
                }
                else
                {
                    *result = lexeme[1];
                }
                return true;
            }
            TM32ASM_LogError("Invalid character literal");
            return false;
        }
        
        case TM32ASM_TT_IDENTIFIER:
        {
            // Look up symbol value
            const char* lexeme = token->lexeme;
            TM32ASM_Symbol* symbol = TM32ASM_FindSymbol(preprocessor, lexeme);
            if (symbol == NULL)
            {
                TM32ASM_LogError("Undefined symbol '%s' in expression", lexeme);
                return false;
            }
            
            if (symbol->type == TM32ASM_ST_CONSTANT || symbol->type == TM32ASM_ST_VARIABLE)
            {
                if (symbol->value != NULL)
                {
                    *result = strtoll(symbol->value, NULL, 0);
                    return true;
                }
                else
                {
                    TM32ASM_LogError("Symbol '%s' has no value", lexeme);
                    return false;
                }
            }
            else
            {
                TM32ASM_LogError("Symbol '%s' is not a constant or variable", lexeme);
                return false;
            }
        }
        
        case TM32ASM_TT_OPEN_PARENTHESIS:
        {
            // Parse parenthesized expression
            int64_t subResult;
            if (!TM32ASM_ParseExpression(preprocessor, tokenStream, currentIndex, endIndex, &subResult))
            {
                return false;
            }
            
            // Expect closing parenthesis
            if (*currentIndex >= endIndex || *currentIndex >= tokenStream->tokenCount)
            {
                TM32ASM_LogError("Expected ')' in expression");
                return false;
            }
            
            TM32ASM_Token* closeParen = tokenStream->tokens[*currentIndex];
            if (closeParen->type != TM32ASM_TT_CLOSE_PARENTHESIS)
            {
                TM32ASM_LogError("Expected ')' in expression");
                return false;
            }
            (*currentIndex)++;
            
            *result = subResult;
            return true;
        }
        
        case TM32ASM_TT_MINUS:
        {
            // Unary minus
            int64_t operand;
            if (!TM32ASM_ParsePrimary(preprocessor, tokenStream, currentIndex, endIndex, &operand))
            {
                return false;
            }
            *result = -operand;
            return true;
        }
        
        case TM32ASM_TT_PLUS:
        {
            // Unary plus
            return TM32ASM_ParsePrimary(preprocessor, tokenStream, currentIndex, endIndex, result);
        }
        
        case TM32ASM_TT_BITWISE_NOT:
        {
            // Bitwise NOT
            int64_t operand;
            if (!TM32ASM_ParsePrimary(preprocessor, tokenStream, currentIndex, endIndex, &operand))
            {
                return false;
            }
            *result = ~operand;
            return true;
        }
        
        default:
            TM32ASM_LogError("Unexpected token in expression");
            return false;
    }
}

/**
 * @brief   Parses exponentiation (highest precedence binary operator).
 */
static bool TM32ASM_ParseExponentiation (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (!TM32ASM_ParsePrimary(preprocessor, tokenStream, currentIndex, endIndex, result))
    {
        return false;
    }
    
    while (*currentIndex < endIndex && *currentIndex < tokenStream->tokenCount)
    {
        TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
        if (token->type == TM32ASM_TT_EXPONENT)
        {
            (*currentIndex)++;
            int64_t right;
            if (!TM32ASM_ParsePrimary(preprocessor, tokenStream, currentIndex, endIndex, &right))
            {
                return false;
            }
            
            // Calculate power
            if (right < 0)
            {
                TM32ASM_LogError("Negative exponents not supported");
                return false;
            }
            
            int64_t temp = 1;
            for (int64_t i = 0; i < right; i++)
            {
                temp *= *result;
            }
            *result = temp;
        }
        else
        {
            break;
        }
    }
    
    return true;
}

/**
 * @brief   Parses multiplication, division, and modulo.
 */
static bool TM32ASM_ParseMultiplicative (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (!TM32ASM_ParseExponentiation(preprocessor, tokenStream, currentIndex, endIndex, result))
    {
        return false;
    }
    
    while (*currentIndex < endIndex && *currentIndex < tokenStream->tokenCount)
    {
        TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
        if (token->type == TM32ASM_TT_TIMES || token->type == TM32ASM_TT_DIVIDE || token->type == TM32ASM_TT_MODULO)
        {
            TM32ASM_TokenType op = token->type;
            (*currentIndex)++;
            int64_t right;
            if (!TM32ASM_ParseExponentiation(preprocessor, tokenStream, currentIndex, endIndex, &right))
            {
                return false;
            }
            
            switch (op)
            {
                case TM32ASM_TT_TIMES:
                    *result *= right;
                    break;
                case TM32ASM_TT_DIVIDE:
                    if (right == 0)
                    {
                        TM32ASM_LogError("Division by zero in expression");
                        return false;
                    }
                    *result /= right;
                    break;
                case TM32ASM_TT_MODULO:
                    if (right == 0)
                    {
                        TM32ASM_LogError("Modulo by zero in expression");
                        return false;
                    }
                    *result %= right;
                    break;
                default:
                    break;
            }
        }
        else
        {
            break;
        }
    }
    
    return true;
}

/**
 * @brief   Parses addition and subtraction.
 */
static bool TM32ASM_ParseAdditive (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (!TM32ASM_ParseMultiplicative(preprocessor, tokenStream, currentIndex, endIndex, result))
    {
        return false;
    }
    
    while (*currentIndex < endIndex && *currentIndex < tokenStream->tokenCount)
    {
        TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
        if (token->type == TM32ASM_TT_PLUS || token->type == TM32ASM_TT_MINUS)
        {
            TM32ASM_TokenType op = token->type;
            (*currentIndex)++;
            int64_t right;
            if (!TM32ASM_ParseMultiplicative(preprocessor, tokenStream, currentIndex, endIndex, &right))
            {
                return false;
            }
            
            if (op == TM32ASM_TT_PLUS)
            {
                *result += right;
            }
            else
            {
                *result -= right;
            }
        }
        else
        {
            break;
        }
    }
    
    return true;
}

/**
 * @brief   Parses shift operations.
 */
static bool TM32ASM_ParseShift (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (!TM32ASM_ParseAdditive(preprocessor, tokenStream, currentIndex, endIndex, result))
    {
        return false;
    }
    
    while (*currentIndex < endIndex && *currentIndex < tokenStream->tokenCount)
    {
        TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
        if (token->type == TM32ASM_TT_LEFT_SHIFT || token->type == TM32ASM_TT_RIGHT_SHIFT)
        {
            TM32ASM_TokenType op = token->type;
            (*currentIndex)++;
            int64_t right;
            if (!TM32ASM_ParseAdditive(preprocessor, tokenStream, currentIndex, endIndex, &right))
            {
                return false;
            }
            
            if (op == TM32ASM_TT_LEFT_SHIFT)
            {
                *result <<= right;
            }
            else
            {
                *result >>= right;
            }
        }
        else
        {
            break;
        }
    }
    
    return true;
}

/**
 * @brief   Parses bitwise AND.
 */
static bool TM32ASM_ParseBitwiseAnd (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (!TM32ASM_ParseShift(preprocessor, tokenStream, currentIndex, endIndex, result))
    {
        return false;
    }
    
    while (*currentIndex < endIndex && *currentIndex < tokenStream->tokenCount)
    {
        TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
        if (token->type == TM32ASM_TT_BITWISE_AND)
        {
            (*currentIndex)++;
            int64_t right;
            if (!TM32ASM_ParseShift(preprocessor, tokenStream, currentIndex, endIndex, &right))
            {
                return false;
            }
            *result &= right;
        }
        else
        {
            break;
        }
    }
    
    return true;
}

/**
 * @brief   Parses bitwise XOR.
 */
static bool TM32ASM_ParseBitwiseXor (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (!TM32ASM_ParseBitwiseAnd(preprocessor, tokenStream, currentIndex, endIndex, result))
    {
        return false;
    }
    
    while (*currentIndex < endIndex && *currentIndex < tokenStream->tokenCount)
    {
        TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
        if (token->type == TM32ASM_TT_BITWISE_XOR)
        {
            (*currentIndex)++;
            int64_t right;
            if (!TM32ASM_ParseBitwiseAnd(preprocessor, tokenStream, currentIndex, endIndex, &right))
            {
                return false;
            }
            *result ^= right;
        }
        else
        {
            break;
        }
    }
    
    return true;
}

/**
 * @brief   Parses bitwise OR.
 */
static bool TM32ASM_ParseBitwiseOr (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    if (!TM32ASM_ParseBitwiseXor(preprocessor, tokenStream, currentIndex, endIndex, result))
    {
        return false;
    }
    
    while (*currentIndex < endIndex && *currentIndex < tokenStream->tokenCount)
    {
        TM32ASM_Token* token = tokenStream->tokens[*currentIndex];
        if (token->type == TM32ASM_TT_BITWISE_OR)
        {
            (*currentIndex)++;
            int64_t right;
            if (!TM32ASM_ParseBitwiseXor(preprocessor, tokenStream, currentIndex, endIndex, &right))
            {
                return false;
            }
            *result |= right;
        }
        else
        {
            break;
        }
    }
    
    return true;
}

/**
 * @brief   Parses a complete expression.
 */
static bool TM32ASM_ParseExpression (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t*                 currentIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    return TM32ASM_ParseBitwiseOr(preprocessor, tokenStream, currentIndex, endIndex, result);
}

static char* TM32ASM_DuplicateString (const char* str)
{
    if (str == NULL) return NULL;
    
    size_t len = strlen(str);
    char* result = TM32ASM_Create(len + 1, char);
    if (result != NULL)
    {
        strcpy(result, str);
    }
    return result;
}

bool TM32ASM_EvaluateExpression (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t                  startIndex,
    size_t                  endIndex,
    int64_t*                result
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(tokenStream, false);
    TM32ASM_ReturnValueIfNull(result, false);
    
    if (startIndex >= endIndex || startIndex >= tokenStream->tokenCount)
    {
        TM32ASM_LogError("Invalid expression range");
        return false;
    }
    
    size_t currentIndex = startIndex;
    bool success = TM32ASM_ParseExpression(preprocessor, tokenStream, &currentIndex, endIndex, result);
    
    if (success && currentIndex != endIndex)
    {
        TM32ASM_LogError("Unexpected tokens at end of expression");
        return false;
    }
    
    return success;
}
