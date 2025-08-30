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
    preprocessor->macroExpansionDepth = 0;
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
    
    // Clean up stacks
    TM32ASM_Destroy(preprocessor->fileStack);
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
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting file inclusion pass");
    }
    
    // TODO: Implement file inclusion logic
    // This will scan for .include directives and recursively process included files
    
    return true;
}

static bool TM32ASM_PassSymbolDeclaration (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Starting symbol declaration pass");
    }
    
    // TODO: Implement symbol declaration logic
    // This will scan for .macro, .define/.def, .let, and .const directives
    // and record them in the symbol table without evaluation
    
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
    
    // TODO: Implement macro expansion logic
    // This will expand parametric macros first, then simple text substitution macros
    
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
    
    // TODO: Implement variable and constant evaluation logic
    // This will evaluate .const expressions and initialize .let variables
    
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
    
    // TODO: Implement control flow processing logic
    // This will handle .if/.elif/.else/.endif, .repeat, .while, .for blocks
    
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
    
    // TODO: Implement finalization logic
    // For now, just copy all input tokens to output as a basic pass-through
    if (preprocessor->inputTokenStream != NULL)
    {
        for (size_t i = 0; i < preprocessor->inputTokenStream->tokenCount; i++)
        {
            TM32ASM_Token* token = preprocessor->inputTokenStream->tokens[i];
            if (token != NULL)
            {
                if (TM32ASM_InsertToken(preprocessor->outputTokenStream, token) == NULL)
                {
                    TM32ASM_LogError("Failed to copy token to output stream");
                    return false;
                }
            }
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
    
    // Execute all preprocessor passes in sequence
    static const struct {
        TM32ASM_PreprocessorPass pass;
        bool (*function)(TM32ASM_Preprocessor*);
        const char* name;
    } passes[] = {
        { TM32ASM_PP_FILE_INCLUSION,      TM32ASM_PassFileInclusion,      "File Inclusion" },
        { TM32ASM_PP_SYMBOL_DECLARATION,  TM32ASM_PassSymbolDeclaration,  "Symbol Declaration" },
        { TM32ASM_PP_MACRO_EXPANSION,     TM32ASM_PassMacroExpansion,     "Macro Expansion" },
        { TM32ASM_PP_VARIABLE_EVALUATION, TM32ASM_PassVariableEvaluation, "Variable Evaluation" },
        { TM32ASM_PP_CONTROL_FLOW,        TM32ASM_PassControlFlow,        "Control Flow Processing" },
        { TM32ASM_PP_FINALIZATION,        TM32ASM_PassFinalization,       "Finalization" }
    };
    
    const size_t numPasses = sizeof(passes) / sizeof(passes[0]);
    
    for (size_t i = 0; i < numPasses; i++)
    {
        preprocessor->currentPass = passes[i].pass;
        
        if (preprocessor->verbose)
        {
            TM32ASM_LogInfo("Executing pass %zu: %s", i + 1, passes[i].name);
        }
        
        if (!passes[i].function(preprocessor))
        {
            TM32ASM_LogError("Preprocessor pass '%s' failed", passes[i].name);
            preprocessor->errorOccurred = true;
            return false;
        }
        
        if (preprocessor->errorOccurred)
        {
            TM32ASM_LogError("Error occurred during pass '%s'", passes[i].name);
            return false;
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
