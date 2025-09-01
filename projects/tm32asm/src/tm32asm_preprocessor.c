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
                        TM32ASM_LogError("Symbol '%s' is already defined at line %u (redefinition at line %u)",
                                       nameToken->lexeme, existingSymbol->line, nameToken->line);
                        TM32ASM_DestroyTokenStream(outputStream);
                        return false;
                    }
                    
                    // Check for optional assignment operator
                    TM32ASM_Token* nextToken = TM32ASM_PeekNextToken(preprocessor->inputTokenStream, 0);
                    char* initialValue = NULL;
                    
                    if (nextToken != NULL && nextToken->type == TM32ASM_TT_ASSIGN_EQUAL)
                    {
                        // Consume the assignment operator
                        TM32ASM_ConsumeNextToken(preprocessor->inputTokenStream);
                        
                        // The initialization expression will be evaluated in the variable evaluation pass
                        // For now, just mark that it has an initialization value
                        initialValue = "0";  // Default for later evaluation
                    }
                    
                    // Create symbol entry for the variable
                    TM32ASM_Symbol symbol = {0};
                    symbol.name = nameToken->lexeme;  // Temporary assignment for AddSymbol
                    symbol.type = TM32ASM_ST_VARIABLE;
                    symbol.value = initialValue;  // Will be evaluated in variable evaluation pass
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
                    if (nameToken == NULL || nameToken->type != TM32ASM_TT_IDENTIFIER)
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
                    
                    // The initialization expression will be evaluated in the variable evaluation pass
                    // For now, just record that it needs evaluation
                    
                    // Create symbol entry for the constant
                    TM32ASM_Symbol symbol = {0};
                    symbol.name = nameToken->lexeme;  // Temporary assignment for AddSymbol
                    symbol.type = TM32ASM_ST_CONSTANT;
                    symbol.value = NULL;  // Will be evaluated in variable evaluation pass
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
    
    // TODO: Implement variable and constant evaluation logic
    // This will evaluate .let and .const expressions and substitute their values
    
    // For now, create a pass-through: copy input to output
    TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
    if (outputStream == NULL)
    {
        TM32ASM_LogError("Could not create output token stream for variable evaluation pass");
        return false;
    }
    
    // Copy all tokens from input to output
    for (size_t i = 0; i < preprocessor->inputTokenStream->tokenCount; i++)
    {
        TM32ASM_Token* token = preprocessor->inputTokenStream->tokens[i];
        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
        if (copiedToken == NULL)
        {
            TM32ASM_LogError("Could not copy token during variable evaluation pass");
            TM32ASM_DestroyTokenStream(outputStream);
            return false;
        }
        
        if (TM32ASM_PushTokenBack(outputStream, copiedToken) == NULL)
        {
            TM32ASM_LogError("Could not add token to output stream during variable evaluation pass");
            TM32ASM_DestroyToken(copiedToken);
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
        TM32ASM_LogInfo("Variable evaluation pass completed (pass-through mode)");
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
    
    // TODO: Implement control flow processing logic
    // This will handle .if/.elif/.else/.endif, .repeat, .while, .for blocks
    
    // For now, create a pass-through: copy input to output
    TM32ASM_TokenStream* outputStream = TM32ASM_CreateTokenStream();
    if (outputStream == NULL)
    {
        TM32ASM_LogError("Could not create output token stream for control flow pass");
        return false;
    }
    
    // Copy all tokens from input to output
    for (size_t i = 0; i < preprocessor->inputTokenStream->tokenCount; i++)
    {
        TM32ASM_Token* token = preprocessor->inputTokenStream->tokens[i];
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
    
    // Set the output token stream
    if (preprocessor->outputTokenStream != NULL)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputTokenStream);
    }
    preprocessor->outputTokenStream = outputStream;
    
    if (preprocessor->verbose)
    {
        TM32ASM_LogInfo("Control flow pass completed (pass-through mode)");
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
