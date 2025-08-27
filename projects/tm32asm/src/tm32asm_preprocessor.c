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
 * @param   input_stream    The input token stream.
 * @param   output_stream   The output token stream.
 * @param   token           The token to process.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_ProcessToken (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    input_stream,
    TM32ASM_TokenStream*    output_stream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles a directive token.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   input_stream    The input token stream.
 * @param   output_stream   The output token stream.
 * @param   token           The directive token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    input_stream,
    TM32ASM_TokenStream*    output_stream,
    TM32ASM_Token*          token
);

/**
 * @brief   Handles symbol substitution for identifier tokens.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   output_stream   The output token stream.
 * @param   token           The identifier token.
 * 
 * @return  `true` if processing was successful, `false` if errors occurred.
 */
static bool TM32ASM_HandleSymbolSubstitution (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    output_stream,
    TM32ASM_Token*          token
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
    preprocessor->symbol_count = 0;
    preprocessor->symbol_capacity = INITIAL_SYMBOL_CAPACITY;
    
    // Initialize macro table
    preprocessor->macros = TM32ASM_Create(INITIAL_MACRO_CAPACITY, TM32ASM_Macro);
    if (preprocessor->macros == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for macro table");
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->macro_count = 0;
    preprocessor->macro_capacity = INITIAL_MACRO_CAPACITY;
    
    // Initialize conditional stack
    preprocessor->conditionals = TM32ASM_Create(INITIAL_CONDITIONAL_CAPACITY, TM32ASM_ConditionalState);
    if (preprocessor->conditionals == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for conditional stack");
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->conditional_count = 0;
    preprocessor->conditional_capacity = INITIAL_CONDITIONAL_CAPACITY;
    
    // Initialize include stack
    preprocessor->include_stack = TM32ASM_Create(INITIAL_INCLUDE_CAPACITY, const char*);
    if (preprocessor->include_stack == NULL) {
        TM32ASM_LogErrno("Could not allocate memory for include stack");
        TM32ASM_Destroy(preprocessor->conditionals);
        TM32ASM_Destroy(preprocessor->macros);
        TM32ASM_Destroy(preprocessor->symbols);
        TM32ASM_Destroy(preprocessor);
        return NULL;
    }
    preprocessor->include_depth = 0;
    preprocessor->include_capacity = INITIAL_INCLUDE_CAPACITY;
    
    // Initialize processing state
    preprocessor->processing_active = true;
    preprocessor->macro_depth = 0;
    preprocessor->max_macro_depth = MAX_MACRO_DEPTH;
    
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
            for (uint32_t i = 0; i < preprocessor->symbol_count; ++i) {
                TM32ASM_DestroySymbol(&preprocessor->symbols[i]);
            }
            TM32ASM_Destroy(preprocessor->symbols);
        }
        
        // Destroy all macros
        if (preprocessor->macros != NULL) {
            for (uint32_t i = 0; i < preprocessor->macro_count; ++i) {
                TM32ASM_DestroyMacro(&preprocessor->macros[i]);
            }
            TM32ASM_Destroy(preprocessor->macros);
        }
        
        // Destroy conditional stack
        TM32ASM_Destroy(preprocessor->conditionals);
        
        // Destroy include stack
        TM32ASM_Destroy(preprocessor->include_stack);
        
        TM32ASM_Destroy(preprocessor);
        TM32ASM_LogDebug("Destroyed preprocessor instance");
    }
}

bool TM32ASM_ProcessTokenStream (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    input_stream,
    TM32ASM_TokenStream*    output_stream
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(input_stream, false);
    TM32ASM_ReturnValueIfNull(output_stream, false);
    
    TM32ASM_LogDebug("Processing token stream");
    
    // Process each token in the input stream
    TM32ASM_Token* token;
    while ((token = TM32ASM_ConsumeNextToken(input_stream)) != NULL) {
        if (!TM32ASM_ProcessToken(preprocessor, input_stream, output_stream, token)) {
            TM32ASM_LogError("Failed to process token: %s", token->lexeme);
            return false;
        }
    }
    
    // Check for unclosed conditionals
    if (preprocessor->conditional_count > 0) {
        TM32ASM_ConditionalState* current = &preprocessor->conditionals[preprocessor->conditional_count - 1];
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
    bool                    is_constant,
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
        if (existing->is_constant) {
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
    if (preprocessor->symbol_count >= preprocessor->symbol_capacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->symbols, 
                               &preprocessor->symbol_capacity, 
                               sizeof(TM32ASM_Symbol))) {
            TM32ASM_LogError("Failed to grow symbol table");
            return false;
        }
    }
    
    // Add new symbol
    TM32ASM_Symbol* symbol = &preprocessor->symbols[preprocessor->symbol_count++];
    symbol->name = TM32ASM_Create(strlen(name) + 1, char);
    if (symbol->name == NULL) {
        TM32ASM_LogError("Failed to allocate memory for symbol name");
        --preprocessor->symbol_count;
        return false;
    }
    strcpy(symbol->name, name);
    symbol->value = value;
    symbol->is_constant = is_constant;
    symbol->line = line;
    symbol->filename = filename;
    
    TM32ASM_LogDebug("Defined %s symbol '%s'", is_constant ? "constant" : "variable", name);
    return true;
}

TM32ASM_Symbol* TM32ASM_LookupSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(name, NULL);
    
    for (uint32_t i = 0; i < preprocessor->symbol_count; ++i) {
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
    uint32_t                param_count,
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
    if (preprocessor->macro_count >= preprocessor->macro_capacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->macros, 
                               &preprocessor->macro_capacity, 
                               sizeof(TM32ASM_Macro))) {
            TM32ASM_LogError("Failed to grow macro table");
            return false;
        }
    }
    
    // Add new macro
    TM32ASM_Macro* macro = &preprocessor->macros[preprocessor->macro_count++];
    macro->name = TM32ASM_Create(strlen(name) + 1, char);
    if (macro->name == NULL) {
        TM32ASM_LogError("Failed to allocate memory for macro name");
        --preprocessor->macro_count;
        return false;
    }
    strcpy(macro->name, name);
    
    macro->parameters = parameters;
    macro->param_count = param_count;
    macro->body = body;
    macro->line = line;
    macro->filename = filename;
    
    TM32ASM_LogDebug("Defined macro '%s' with %u parameters", name, param_count);
    return true;
}

TM32ASM_Macro* TM32ASM_LookupMacro (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(name, NULL);
    
    for (uint32_t i = 0; i < preprocessor->macro_count; ++i) {
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
    if (preprocessor->conditional_count >= preprocessor->conditional_capacity) {
        if (!TM32ASM_GrowArray((void**)&preprocessor->conditionals, 
                               &preprocessor->conditional_capacity, 
                               sizeof(TM32ASM_ConditionalState))) {
            TM32ASM_LogError("Failed to grow conditional stack");
            return false;
        }
    }
    
    // Push new conditional state
    TM32ASM_ConditionalState* state = &preprocessor->conditionals[preprocessor->conditional_count++];
    state->active = active;
    state->taken = active;
    state->in_else = false;
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
    
    if (preprocessor->conditional_count == 0) {
        TM32ASM_LogError("No conditional to pop");
        return false;
    }
    
    --preprocessor->conditional_count;
    TM32ASM_LogDebug("Popped conditional from stack");
    return true;
}

TM32ASM_ConditionalState* TM32ASM_GetCurrentConditional (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    
    if (preprocessor->conditional_count == 0) {
        return NULL;
    }
    
    return &preprocessor->conditionals[preprocessor->conditional_count - 1];
}

bool TM32ASM_ShouldProcessTokens (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, true);
    
    if (!preprocessor->processing_active) {
        return false;
    }
    
    // Check all conditional states - all must be active
    for (uint32_t i = 0; i < preprocessor->conditional_count; ++i) {
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
        for (uint32_t i = 0; i < macro->param_count; ++i) {
            TM32ASM_Destroy(macro->parameters[i]);
        }
        TM32ASM_Destroy(macro->parameters);
    }
    
    // Destroy macro body
    TM32ASM_DestroyTokenStream(macro->body);
}

static bool TM32ASM_ProcessToken (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    input_stream,
    TM32ASM_TokenStream*    output_stream,
    TM32ASM_Token*          token
)
{
    // Handle directive tokens
    if (token->type == TM32ASM_TT_DIRECTIVE) {
        return TM32ASM_HandleDirective(preprocessor, input_stream, output_stream, token);
    }
    
    // If we're not processing tokens (e.g., in a false conditional), skip them
    if (!TM32ASM_ShouldProcessTokens(preprocessor)) {
        TM32ASM_DestroyToken(token);
        return true;
    }
    
    // Handle identifier tokens for symbol substitution
    if (token->type == TM32ASM_TT_IDENTIFIER) {
        return TM32ASM_HandleSymbolSubstitution(preprocessor, output_stream, token);
    }
    
    // Pass through all other tokens
    TM32ASM_InsertToken(output_stream, token);
    return true;
}

static bool TM32ASM_HandleDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    input_stream,
    TM32ASM_TokenStream*    output_stream,
    TM32ASM_Token*          token
)
{
    // For now, just pass through all directives - specific directive handling
    // will be implemented in subsequent prompts
    TM32ASM_InsertToken(output_stream, token);
    return true;
}

static bool TM32ASM_HandleSymbolSubstitution (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    output_stream,
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
        
        TM32ASM_InsertToken(output_stream, value_token);
        TM32ASM_DestroyToken(token);
        
        TM32ASM_LogDebug("Substituted symbol '%s' with value '%s'", 
                         symbol->name, symbol->value->lexeme);
    } else {
        // Not a symbol, pass through as-is
        TM32ASM_InsertToken(output_stream, token);
    }
    
    return true;
}
