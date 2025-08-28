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
#define INITIAL_INCLUDE_CAPACITY    16
#define MAX_MACRO_DEPTH             64

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
    
    // Initialize include stack
    preprocessor->includeStack = TM32ASM_Create(INITIAL_INCLUDE_CAPACITY, const char*);
    if (preprocessor->includeStack == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for include stack");
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
    
    // Other directives are only processed if we're in an active context
    if (!TM32ASM_ShouldProcessTokens(preprocessor)) {
        return true; // Skip processing but don't error
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
        conditionTrue = (conditionToken->param != 0);
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
                // Use param if available, otherwise parse the lexeme
                int32_t value = symbol->value->param;
                if (value == 0 && symbol->value->lexeme != NULL) {
                    // Try to parse the lexeme as a fallback
                    value = atoi(symbol->value->lexeme);
                }
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
