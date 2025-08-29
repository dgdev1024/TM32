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
#include <math.h>
#include <inttypes.h>
#include <errno.h>

/* Private Macros *************************************************************/

#define TM32ASM_PREPROCESSOR_SYMBOL_GROWTH_FACTOR       1.5f
#define TM32ASM_PREPROCESSOR_CONDITIONAL_GROWTH_FACTOR  1.5f
#define TM32ASM_PREPROCESSOR_LOOP_GROWTH_FACTOR         1.5f
#define TM32ASM_PREPROCESSOR_INCLUDE_GROWTH_FACTOR      1.5f

/* Private Function Prototypes ************************************************/

/**
 * @brief   Initializes the preprocessor's internal arrays and state.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if initialization was successful; `false` otherwise.
 */
static bool TM32ASM_InitializePreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Cleans up all resources associated with the preprocessor.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 */
static void TM32ASM_CleanupPreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Destroys a symbol and frees its associated memory.
 * 
 * @param   symbol  A pointer to the symbol to destroy.
 */
static void TM32ASM_DestroySymbol (
    TM32ASM_Symbol* symbol
);

/**
 * @brief   Finds a symbol by name in the preprocessor's symbol table.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   name            The name of the symbol to find.
 * 
 * @return  A pointer to the symbol if found; `NULL` otherwise.
 */
static TM32ASM_Symbol* TM32ASM_FindSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
);

/**
 * @brief   Adds a symbol to the preprocessor's symbol table.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   symbol          A pointer to the symbol to add.
 * 
 * @return  `true` if the symbol was added successfully; `false` otherwise.
 */
static bool TM32ASM_AddSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Symbol*         symbol
);

/**
 * @brief   Ensures the preprocessor's symbol table has sufficient capacity.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   requiredCapacity The required capacity.
 * 
 * @return  `true` if capacity is sufficient or was expanded; `false` otherwise.
 */
static bool TM32ASM_EnsureSymbolCapacity (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  requiredCapacity
);

/**
 * @brief   Checks if a token can start an expression that needs evaluation.
 */
static bool TM32ASM_TokenStartsExpression (
    const TM32ASM_Token* token
);

/**
 * @brief   Finds the end of an expression starting at the given index.
 */
static size_t TM32ASM_FindExpressionEnd (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  startIndex
);

/**
 * @brief   Processes an expression and replaces it with its evaluated result.
 */
static bool TM32ASM_ProcessExpression (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  startIndex,
    size_t                  endIndex
);

/**
 * @brief   Processes a single token from the input stream.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessToken (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Processes a preprocessor directive.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   directive       A pointer to the directive token.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          directive
);

/**
 * @brief   Processes a '.let' directive to define a mutable variable.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessLetDirective (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Processes a '.const' directive to define an immutable constant.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessConstDirective (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Consumes tokens from the input stream to parse a variable assignment.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   isConstant      Whether this is a constant declaration.
 * @param   varName         Output parameter for the variable name (caller must free).
 * @param   hasAssignment   Output parameter indicating if there's an assignment.
 * @param   value           Output parameter for the assigned value (caller must free).
 * 
 * @return  `true` if parsing was successful; `false` otherwise.
 */
static bool TM32ASM_ParseVariableDeclaration (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    isConstant,
    char**                  varName,
    bool*                   hasAssignment,
    TM32ASM_Value*          value
);

/**
 * @brief   Performs a compound assignment operation.
 * 
 * @param   lhs         The left-hand side value.
 * @param   rhs         The right-hand side value.
 * @param   operator    The compound assignment operator type.
 * @param   result      Output parameter for the result.
 * 
 * @return  `true` if the operation was successful; `false` otherwise.
 */
static bool TM32ASM_PerformCompoundAssignment (
    const TM32ASM_Value*    lhs,
    const TM32ASM_Value*    rhs,
    TM32ASM_TokenType       operator,
    TM32ASM_Value*          result
);

/**
 * @brief   Processes a '.if' directive to begin conditional assembly.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessIfDirective (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Processes an '.else' directive to switch conditional assembly state.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessElseDirective (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Processes an '.endif' directive to end conditional assembly.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessEndifDirective (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Processes an '.elseif' directive for conditional assembly.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if processing was successful; `false` otherwise.
 */
static bool TM32ASM_ProcessElseifDirective (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Checks if the current conditional state allows token processing.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if tokens should be processed; `false` if they should be skipped.
 */
static bool TM32ASM_ShouldProcessTokens (
    const TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Reports an error with context information.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   format          The error message format string.
 * @param   ...             Additional arguments for the format string.
 */
static void TM32ASM_ReportError (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             format,
    ...
);

/**
 * @brief   Reports a warning with context information.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   format          The warning message format string.
 * @param   ...             Additional arguments for the format string.
 */
static void TM32ASM_ReportWarning (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             format,
    ...
);

/**
 * @brief   Substitutes a variable or constant identifier with its value.
 * 
 * @param   preprocessor    The preprocessor.
 * @param   token           The identifier token to potentially substitute.
 * 
 * @return  A new token with the substituted value, or NULL if no substitution.
 */
static TM32ASM_Token* TM32ASM_SubstituteVariable (
    TM32ASM_Preprocessor*   preprocessor,
    const TM32ASM_Token*    token
);

/* Private Functions **********************************************************/

static bool TM32ASM_InitializePreprocessor (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // Initialize symbol table
    preprocessor->symbols = TM32ASM_CreateZero(TM32ASM_PREPROCESSOR_INITIAL_SYMBOL_CAPACITY, TM32ASM_Symbol);
    if (preprocessor->symbols == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for preprocessor symbol table");
        return false;
    }
    preprocessor->symbolCount = 0;
    preprocessor->symbolCapacity = TM32ASM_PREPROCESSOR_INITIAL_SYMBOL_CAPACITY;

    // Initialize conditional compilation stack
    preprocessor->conditionalStack = TM32ASM_CreateZero(TM32ASM_PREPROCESSOR_INITIAL_CONDITIONAL_CAPACITY, TM32ASM_ConditionalContext);
    if (preprocessor->conditionalStack == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for preprocessor conditional stack");
        return false;
    }
    preprocessor->conditionalDepth = 0;
    preprocessor->conditionalCapacity = TM32ASM_PREPROCESSOR_INITIAL_CONDITIONAL_CAPACITY;

    // Initialize loop context stack
    preprocessor->loopStack = TM32ASM_CreateZero(TM32ASM_PREPROCESSOR_INITIAL_LOOP_CAPACITY, TM32ASM_LoopContext);
    if (preprocessor->loopStack == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for preprocessor loop stack");
        return false;
    }
    preprocessor->loopDepth = 0;
    preprocessor->loopCapacity = TM32ASM_PREPROCESSOR_INITIAL_LOOP_CAPACITY;

    // Initialize include context stack
    preprocessor->includeStack = TM32ASM_CreateZero(TM32ASM_PREPROCESSOR_INITIAL_INCLUDE_CAPACITY, TM32ASM_IncludeContext);
    if (preprocessor->includeStack == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for preprocessor include stack");
        return false;
    }
    preprocessor->includeDepth = 0;
    preprocessor->includeCapacity = TM32ASM_PREPROCESSOR_INITIAL_INCLUDE_CAPACITY;

    // Initialize other fields
    preprocessor->includedFiles = NULL;
    preprocessor->includedFileCount = 0;
    preprocessor->includePaths = NULL;
    preprocessor->includePathCount = 0;
    preprocessor->inputStream = NULL;
    preprocessor->outputStream = NULL;
    preprocessor->currentTokenIndex = 0;
    preprocessor->treatWarningsAsErrors = false;
    preprocessor->hasErrors = false;
    preprocessor->currentLine = 1;
    preprocessor->currentFilename = NULL;
    preprocessor->macroExpansionDepth = 0;
    preprocessor->maxMacroExpansionDepth = TM32ASM_PREPROCESSOR_MAX_MACRO_EXPANSION_DEPTH;
    preprocessor->uniqueCounter = 0;

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

    // Cleanup symbols
    if (preprocessor->symbols != NULL)
    {
        for (size_t i = 0; i < preprocessor->symbolCount; i++)
        {
            TM32ASM_DestroySymbol(&preprocessor->symbols[i]);
        }
        TM32ASM_Destroy(preprocessor->symbols);
    }

    // Cleanup conditional stack
    TM32ASM_Destroy(preprocessor->conditionalStack);

    // Cleanup loop stack
    if (preprocessor->loopStack != NULL)
    {
        for (size_t i = 0; i < preprocessor->loopDepth; i++)
        {
            TM32ASM_Destroy(preprocessor->loopStack[i].variableName);
            TM32ASM_DestroyTokenStream(preprocessor->loopStack[i].bodyTokens);
        }
        TM32ASM_Destroy(preprocessor->loopStack);
    }

    // Cleanup include stack
    if (preprocessor->includeStack != NULL)
    {
        for (size_t i = 0; i < preprocessor->includeDepth; i++)
        {
            TM32ASM_Destroy(preprocessor->includeStack[i].filename);
        }
        TM32ASM_Destroy(preprocessor->includeStack);
    }

    // Cleanup included files list
    if (preprocessor->includedFiles != NULL)
    {
        for (size_t i = 0; i < preprocessor->includedFileCount; i++)
        {
            TM32ASM_Destroy(preprocessor->includedFiles[i]);
        }
        TM32ASM_Destroy(preprocessor->includedFiles);
    }

    // Cleanup include paths
    if (preprocessor->includePaths != NULL)
    {
        for (size_t i = 0; i < preprocessor->includePathCount; i++)
        {
            TM32ASM_Destroy(preprocessor->includePaths[i]);
        }
        TM32ASM_Destroy(preprocessor->includePaths);
    }

    // Note: inputStream is not owned by the preprocessor, so we don't destroy it
    // outputStream will be returned to the caller, so we don't destroy it here
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
    TM32ASM_Destroy(symbol->expression);
    
    if (symbol->parameters != NULL)
    {
        for (size_t i = 0; i < symbol->parameterCount; i++)
        {
            TM32ASM_Destroy(symbol->parameters[i]);
        }
        TM32ASM_Destroy(symbol->parameters);
    }

    TM32ASM_DestroyTokenStream(symbol->body);

    // Clear the symbol structure
    memset(symbol, 0, sizeof(TM32ASM_Symbol));
}

static TM32ASM_Symbol* TM32ASM_FindSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(name, NULL);

    for (size_t i = 0; i < preprocessor->symbolCount; i++)
    {
        if (preprocessor->symbols[i].name != NULL && 
            strcmp(preprocessor->symbols[i].name, name) == 0)
        {
            return &preprocessor->symbols[i];
        }
    }

    return NULL;
}

static bool TM32ASM_AddSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Symbol*         symbol
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(symbol, false);

    if (!TM32ASM_EnsureSymbolCapacity(preprocessor, preprocessor->symbolCount + 1))
    {
        return false;
    }

    preprocessor->symbols[preprocessor->symbolCount] = *symbol;
    preprocessor->symbolCount++;

    return true;
}

static bool TM32ASM_EnsureSymbolCapacity (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  requiredCapacity
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    if (requiredCapacity <= preprocessor->symbolCapacity)
    {
        return true;
    }

    size_t newCapacity = (size_t)(preprocessor->symbolCapacity * TM32ASM_PREPROCESSOR_SYMBOL_GROWTH_FACTOR);
    if (newCapacity < requiredCapacity)
    {
        newCapacity = requiredCapacity;
    }

    TM32ASM_Symbol* newSymbols = TM32ASM_Resize(preprocessor->symbols, newCapacity, TM32ASM_Symbol);
    if (newSymbols == NULL)
    {
        TM32ASM_LogErrno("Could not resize preprocessor symbol table");
        return false;
    }

    // Initialize new elements to zero
    for (size_t i = preprocessor->symbolCapacity; i < newCapacity; i++)
    {
        memset(&newSymbols[i], 0, sizeof(TM32ASM_Symbol));
    }

    preprocessor->symbols = newSymbols;
    preprocessor->symbolCapacity = newCapacity;

    return true;
}

/* Private Expression Processing Functions ************************************/

static bool TM32ASM_TokenStartsExpression (
    const TM32ASM_Token* token
)
{
    if (token == NULL)
    {
        return false;
    }

    switch (token->type)
    {
        case TM32ASM_TT_BINARY:
        case TM32ASM_TT_OCTAL:
        case TM32ASM_TT_DECIMAL:
        case TM32ASM_TT_HEXADECIMAL:
        case TM32ASM_TT_FIXED_POINT:
        case TM32ASM_TT_CHARACTER:
        case TM32ASM_TT_IDENTIFIER:
        case TM32ASM_TT_OPEN_PARENTHESIS:
        case TM32ASM_TT_PLUS:
        case TM32ASM_TT_MINUS:
        case TM32ASM_TT_LOGICAL_NOT:
        case TM32ASM_TT_BITWISE_NOT:
            return true;
        default:
            return false;
    }
}

static size_t TM32ASM_FindExpressionEnd (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  startIndex
)
{
    if (preprocessor == NULL || preprocessor->inputStream == NULL)
    {
        return startIndex;
    }

    size_t currentIndex = startIndex;
    size_t parenDepth = 0;
    size_t ternaryDepth = 0; // Track ternary operator depth
    bool foundOperator = false;

    while (currentIndex < preprocessor->inputStream->tokenCount)
    {
        TM32ASM_Token* token = preprocessor->inputStream->tokens[currentIndex];
        if (token == NULL)
        {
            break;
        }

        switch (token->type)
        {
            case TM32ASM_TT_OPEN_PARENTHESIS:
                parenDepth++;
                currentIndex++;
                break;

            case TM32ASM_TT_CLOSE_PARENTHESIS:
                if (parenDepth > 0)
                {
                    parenDepth--;
                    currentIndex++;
                }
                else
                {
                    // End of expression
                    return currentIndex;
                }
                break;

            case TM32ASM_TT_QUESTION:
                if (parenDepth == 0)
                {
                    ternaryDepth++;
                }
                currentIndex++;
                break;

            case TM32ASM_TT_COMMA:
                if (parenDepth == 0)
                {
                    // End of expression (operand separator)
                    return currentIndex;
                }
                currentIndex++;
                break;

            case TM32ASM_TT_COLON:
                if (parenDepth == 0)
                {
                    if (ternaryDepth > 0)
                    {
                        ternaryDepth--;
                        currentIndex++;
                    }
                    else
                    {
                        // End of expression (operand separator, not part of ternary)
                        return currentIndex;
                    }
                }
                else
                {
                    currentIndex++;
                }
                break;

            case TM32ASM_TT_BINARY:
            case TM32ASM_TT_OCTAL:
            case TM32ASM_TT_DECIMAL:
            case TM32ASM_TT_HEXADECIMAL:
            case TM32ASM_TT_FIXED_POINT:
            case TM32ASM_TT_CHARACTER:
                currentIndex++;
                break;

            case TM32ASM_TT_IDENTIFIER:
                // Check if this identifier is followed by a colon (label definition)
                // If so, end the expression here
                if (currentIndex + 1 < preprocessor->inputStream->tokenCount)
                {
                    TM32ASM_Token* nextToken = preprocessor->inputStream->tokens[currentIndex + 1];
                    if (nextToken != NULL && nextToken->type == TM32ASM_TT_COLON)
                    {
                        // This is a label definition, end expression
                        return currentIndex;
                    }
                }
                currentIndex++;
                break;

            case TM32ASM_TT_PLUS:
            case TM32ASM_TT_MINUS:
            case TM32ASM_TT_TIMES:
            case TM32ASM_TT_EXPONENT:
            case TM32ASM_TT_DIVIDE:
            case TM32ASM_TT_MODULO:
            case TM32ASM_TT_BITWISE_AND:
            case TM32ASM_TT_BITWISE_OR:
            case TM32ASM_TT_BITWISE_XOR:
            case TM32ASM_TT_BITWISE_NOT:
            case TM32ASM_TT_LEFT_SHIFT:
            case TM32ASM_TT_RIGHT_SHIFT:
            case TM32ASM_TT_COMPARE_EQUAL:
            case TM32ASM_TT_COMPARE_NOT_EQUAL:
            case TM32ASM_TT_COMPARE_LESS:
            case TM32ASM_TT_COMPARE_GREATER:
            case TM32ASM_TT_COMPARE_LESS_EQUAL:
            case TM32ASM_TT_COMPARE_GREATER_EQUAL:
            case TM32ASM_TT_LOGICAL_AND:
            case TM32ASM_TT_LOGICAL_OR:
            case TM32ASM_TT_LOGICAL_NOT:
                foundOperator = true;
                currentIndex++;
                break;

            default:
                // Unknown token type, end expression
                return currentIndex;
        }
    }

    return currentIndex;
}

static bool TM32ASM_ProcessExpression (
    TM32ASM_Preprocessor*   preprocessor,
    size_t                  startIndex,
    size_t                  endIndex
)
{
    if (preprocessor == NULL || preprocessor->outputStream == NULL)
    {
        return false;
    }

    // Check if this is actually a complex expression (contains operators)
    bool hasOperators = false;
    size_t parenDepth = 0;

    for (size_t i = startIndex; i < endIndex; i++)
    {
        TM32ASM_Token* token = preprocessor->inputStream->tokens[i];
        if (token == NULL) continue;

        switch (token->type)
        {
            case TM32ASM_TT_OPEN_PARENTHESIS:
                parenDepth++;
                break;
            case TM32ASM_TT_CLOSE_PARENTHESIS:
                parenDepth--;
                break;
            case TM32ASM_TT_PLUS:
            case TM32ASM_TT_MINUS:
            case TM32ASM_TT_TIMES:
            case TM32ASM_TT_EXPONENT:
            case TM32ASM_TT_DIVIDE:
            case TM32ASM_TT_MODULO:
            case TM32ASM_TT_BITWISE_AND:
            case TM32ASM_TT_BITWISE_OR:
            case TM32ASM_TT_BITWISE_XOR:
            case TM32ASM_TT_BITWISE_NOT:
            case TM32ASM_TT_LEFT_SHIFT:
            case TM32ASM_TT_RIGHT_SHIFT:
            case TM32ASM_TT_COMPARE_EQUAL:
            case TM32ASM_TT_COMPARE_NOT_EQUAL:
            case TM32ASM_TT_COMPARE_LESS:
            case TM32ASM_TT_COMPARE_GREATER:
            case TM32ASM_TT_COMPARE_LESS_EQUAL:
            case TM32ASM_TT_COMPARE_GREATER_EQUAL:
            case TM32ASM_TT_LOGICAL_AND:
            case TM32ASM_TT_LOGICAL_OR:
            case TM32ASM_TT_LOGICAL_NOT:
            case TM32ASM_TT_QUESTION:
                hasOperators = true;
                break;
            default:
                break;
        }
    }

    // Check if we need to evaluate this expression
    bool needsEvaluation = hasOperators;
    
    // Also evaluate single character literals (they need to be converted to numeric values)
    if (!needsEvaluation && startIndex + 1 == endIndex)
    {
        TM32ASM_Token* singleToken = preprocessor->inputStream->tokens[startIndex];
        if (singleToken != NULL && singleToken->type == TM32ASM_TT_CHARACTER)
        {
            needsEvaluation = true;
        }
        // Also evaluate single identifiers ONLY if they're defined symbols (not forward references)
        else if (singleToken != NULL && singleToken->type == TM32ASM_TT_IDENTIFIER)
        {
            TM32ASM_Symbol* symbol = TM32ASM_FindSymbol(preprocessor, singleToken->lexeme);
            if (symbol != NULL && symbol->isDefined)
            {
                needsEvaluation = true;
            }
            // For undefined identifiers (likely label references), don't evaluate
        }
    }

    // If no evaluation needed, just copy tokens as-is
    if (!needsEvaluation)
    {
        for (size_t i = startIndex; i < endIndex; i++)
        {
            TM32ASM_Token* token = preprocessor->inputStream->tokens[i];
            if (token != NULL)
            {
                TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
                if (copiedToken == NULL)
                {
                    TM32ASM_ReportError(preprocessor, "Failed to copy token");
                    return false;
                }
                
                if (TM32ASM_PushTokenBack(preprocessor->outputStream, copiedToken) == NULL)
                {
                    TM32ASM_DestroyToken(copiedToken);
                    TM32ASM_ReportError(preprocessor, "Failed to add token to output stream");
                    return false;
                }
            }
        }
        return true;
    }

    // Evaluate the expression
    TM32ASM_Value result;
    if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputStream, startIndex, endIndex, &result))
    {
        return false;
    }

    // Convert result back to a token
    char* resultStr = TM32ASM_ValueToString(&result);
    TM32ASM_DestroyValue(&result);

    if (resultStr == NULL)
    {
        TM32ASM_ReportError(preprocessor, "Failed to convert expression result to string");
        return false;
    }

    // Determine the appropriate token type for the result
    TM32ASM_TokenType tokenType = TM32ASM_TT_DECIMAL;
    if (strchr(resultStr, '.') != NULL)
    {
        tokenType = TM32ASM_TT_FIXED_POINT;
    }

    // Create a new token for the result
    TM32ASM_Token* resultToken = TM32ASM_CreateToken(resultStr, tokenType);
    free(resultStr);  // Free the string since TM32ASM_CreateToken makes a copy
    
    if (resultToken == NULL)
    {
        TM32ASM_ReportError(preprocessor, "Failed to create result token");
        return false;
    }

    // Set position information from the first token in the expression
    if (startIndex < preprocessor->inputStream->tokenCount)
    {
        TM32ASM_Token* firstToken = preprocessor->inputStream->tokens[startIndex];
        if (firstToken != NULL)
        {
            resultToken->line = firstToken->line;
            resultToken->filename = firstToken->filename;
        }
    }

    // Add the result token to the output stream
    if (TM32ASM_PushTokenBack(preprocessor->outputStream, resultToken) == NULL)
    {
        TM32ASM_DestroyToken(resultToken);
        TM32ASM_ReportError(preprocessor, "Failed to add result token to output stream");
        return false;
    }

    return true;
}

/* Private Preprocessor Functions ********************************************/

static bool TM32ASM_ProcessToken (
    TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(preprocessor->inputStream, false);

    // Check if we've reached the end of the input stream
    if (preprocessor->currentTokenIndex >= preprocessor->inputStream->tokenCount)
    {
        return true; // End of stream, not an error
    }

    TM32ASM_Token* token = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];

    // Update current position for error reporting
    if (token != NULL)
    {
        preprocessor->currentLine = token->line;
        if (token->filename != NULL)
        {
            preprocessor->currentFilename = token->filename;
        }
    }

    // Check if we should process tokens based on conditional state
    if (!TM32ASM_ShouldProcessTokens(preprocessor))
    {
        // Skip this token if we're in a conditional block that should be skipped
        // Exception: we still need to process conditional directives themselves
        if (token != NULL && token->type == TM32ASM_TT_DIRECTIVE)
        {
            const char* lexeme = token->lexeme;
            if (lexeme != NULL && 
                (strcmp(lexeme, ".if") == 0 || strcmp(lexeme, ".ifdef") == 0 || 
                 strcmp(lexeme, ".ifndef") == 0 || strcmp(lexeme, ".elseif") == 0 || 
                 strcmp(lexeme, ".else") == 0 || strcmp(lexeme, ".endif") == 0))
            {
                return TM32ASM_ProcessDirective(preprocessor, token);
            }
        }
        // Skip this token - advance the index manually since we're not processing it
        preprocessor->currentTokenIndex++;
        return true;
    }

    // Process the token based on its type
    if (token != NULL && token->type == TM32ASM_TT_DIRECTIVE)
    {
        return TM32ASM_ProcessDirective(preprocessor, token);
    }
    else
    {
        // For non-directive tokens, advance the index
        preprocessor->currentTokenIndex++;
        
        // For non-directive tokens, check if we need to evaluate expressions
        // This happens when we encounter instruction operands
        if (preprocessor->outputStream != NULL)
        {
            // Special handling for label definitions: if this is an identifier followed by a colon,
            // don't try to evaluate it as an expression
            if (token != NULL && token->type == TM32ASM_TT_IDENTIFIER && 
                preprocessor->currentTokenIndex < preprocessor->inputStream->tokenCount)
            {
                TM32ASM_Token* nextToken = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];
                if (nextToken != NULL && nextToken->type == TM32ASM_TT_COLON)
                {
                    // This is a label definition (identifier:), just copy the identifier as-is
                    TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
                    if (copiedToken == NULL)
                    {
                        TM32ASM_ReportError(preprocessor, "Failed to copy token");
                        return false;
                    }
                    
                    if (TM32ASM_PushTokenBack(preprocessor->outputStream, copiedToken) == NULL)
                    {
                        TM32ASM_DestroyToken(copiedToken);
                        TM32ASM_ReportError(preprocessor, "Failed to add token to output stream");
                        return false;
                    }
                    return true;
                }
            }
            
            // Check if this token starts an expression that needs evaluation
            if (TM32ASM_TokenStartsExpression(token))
            {
                // Find the end of the expression
                size_t expressionStart = preprocessor->currentTokenIndex - 1; // Current token
                size_t expressionEnd = TM32ASM_FindExpressionEnd(preprocessor, expressionStart);
                
                if (expressionEnd > expressionStart)
                {
                    // Process the expression
                    if (!TM32ASM_ProcessExpression(preprocessor, expressionStart, expressionEnd))
                    {
                        return false;
                    }
                    // Skip tokens that were part of the expression
                    preprocessor->currentTokenIndex = expressionEnd;
                }
                else
                {
                    // Check if this identifier is a variable/constant that needs substitution
                    if (token->type == TM32ASM_TT_IDENTIFIER)
                    {
                        TM32ASM_Token* substitutedToken = TM32ASM_SubstituteVariable(preprocessor, token);
                        if (substitutedToken != NULL)
                        {
                            if (TM32ASM_PushTokenBack(preprocessor->outputStream, substitutedToken) == NULL)
                            {
                                TM32ASM_DestroyToken(substitutedToken);
                                TM32ASM_ReportError(preprocessor, "Failed to add substituted token to output stream");
                                return false;
                            }
                        }
                        else
                        {
                            // Just copy the original token if no substitution was made
                            TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
                            if (copiedToken == NULL)
                            {
                                TM32ASM_ReportError(preprocessor, "Failed to copy token");
                                return false;
                            }
                            
                            if (TM32ASM_PushTokenBack(preprocessor->outputStream, copiedToken) == NULL)
                            {
                                TM32ASM_DestroyToken(copiedToken);
                                TM32ASM_ReportError(preprocessor, "Failed to add token to output stream");
                                return false;
                            }
                        }
                    }
                    else
                    {
                        // Just copy the token as-is
                        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
                        if (copiedToken == NULL)
                        {
                            TM32ASM_ReportError(preprocessor, "Failed to copy token");
                            return false;
                        }
                        
                        if (TM32ASM_PushTokenBack(preprocessor->outputStream, copiedToken) == NULL)
                        {
                            TM32ASM_DestroyToken(copiedToken);
                            TM32ASM_ReportError(preprocessor, "Failed to add token to output stream");
                            return false;
                        }
                    }
                }
            }
            else
            {
                // Check if this identifier is a variable/constant that needs substitution
                if (token != NULL && token->type == TM32ASM_TT_IDENTIFIER)
                {
                    TM32ASM_Token* substitutedToken = TM32ASM_SubstituteVariable(preprocessor, token);
                    if (substitutedToken != NULL)
                    {
                        if (TM32ASM_PushTokenBack(preprocessor->outputStream, substitutedToken) == NULL)
                        {
                            TM32ASM_DestroyToken(substitutedToken);
                            TM32ASM_ReportError(preprocessor, "Failed to add substituted token to output stream");
                            return false;
                        }
                    }
                    else
                    {
                        // Just copy the original token if no substitution was made
                        TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
                        if (copiedToken == NULL)
                        {
                            TM32ASM_ReportError(preprocessor, "Failed to copy token");
                            return false;
                        }
                        
                        if (TM32ASM_PushTokenBack(preprocessor->outputStream, copiedToken) == NULL)
                        {
                            TM32ASM_DestroyToken(copiedToken);
                            TM32ASM_ReportError(preprocessor, "Failed to add token to output stream");
                            return false;
                        }
                    }
                }
                else
                {
                    // Just copy the token as-is for non-identifiers
                    TM32ASM_Token* copiedToken = TM32ASM_CopyToken(token);
                    if (copiedToken == NULL)
                    {
                        TM32ASM_ReportError(preprocessor, "Failed to copy token");
                        return false;
                    }
                    
                    if (TM32ASM_PushTokenBack(preprocessor->outputStream, copiedToken) == NULL)
                    {
                        TM32ASM_DestroyToken(copiedToken);
                        TM32ASM_ReportError(preprocessor, "Failed to add token to output stream");
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

static bool TM32ASM_ProcessDirective (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_Token*          directive
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(directive, false);

    // TM32ASM_LogDebug("Processing directive: %s", directive->lexeme ? directive->lexeme : "<unknown>");

    // Handle preprocessor directives
    switch ((TM32ASM_DirectiveType) directive->param)
    {
        case TM32ASM_DT_LET:
            return TM32ASM_ProcessLetDirective(preprocessor);

        case TM32ASM_DT_CONST:
            return TM32ASM_ProcessConstDirective(preprocessor);

        case TM32ASM_DT_IF:
            return TM32ASM_ProcessIfDirective(preprocessor);

        case TM32ASM_DT_ELSE:
            return TM32ASM_ProcessElseDirective(preprocessor);

        case TM32ASM_DT_ENDIF:
            return TM32ASM_ProcessEndifDirective(preprocessor);

        case TM32ASM_DT_ELSEIF:
            return TM32ASM_ProcessElseifDirective(preprocessor);

        default:
            // For other directives, advance the token index and pass through to output
            preprocessor->currentTokenIndex++;
            if (preprocessor->outputStream != NULL)
            {
                TM32ASM_Token* copiedToken = TM32ASM_CopyToken(directive);
                if (copiedToken == NULL)
                {
                    TM32ASM_ReportError(preprocessor, "Failed to copy directive token");
                    return false;
                }
                
                if (TM32ASM_PushTokenBack(preprocessor->outputStream, copiedToken) == NULL)
                {
                    TM32ASM_DestroyToken(copiedToken);
                    TM32ASM_ReportError(preprocessor, "Failed to add directive token to output stream");
                    return false;
                }
            }
            break;
    }

    return true;
}

static bool TM32ASM_ShouldProcessTokens (
    const TM32ASM_Preprocessor* preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // If we're not in any conditional blocks, always process tokens
    if (preprocessor->conditionalDepth == 0)
    {
        return true;
    }

    // Check the current conditional state
    const TM32ASM_ConditionalContext* currentContext = 
        &preprocessor->conditionalStack[preprocessor->conditionalDepth - 1];
    
    return currentContext->state == TM32ASM_CS_ACTIVE;
}

static void TM32ASM_ReportError (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             format,
    ...
)
{
    if (preprocessor == NULL || format == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, format);

    fprintf(stderr, "[PREPROCESSOR ERROR] ");
    
    if (preprocessor->currentFilename != NULL)
    {
        fprintf(stderr, "%s:%u: ", 
                preprocessor->currentFilename, 
                preprocessor->currentLine);
    }
    
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);

    preprocessor->hasErrors = true;
}

static void TM32ASM_ReportWarning (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             format,
    ...
)
{
    if (preprocessor == NULL || format == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, format);

    if (preprocessor->treatWarningsAsErrors)
    {
        fprintf(stderr, "[PREPROCESSOR ERROR] ");
        preprocessor->hasErrors = true;
    }
    else
    {
        fprintf(stderr, "[PREPROCESSOR WARNING] ");
    }
    
    if (preprocessor->currentFilename != NULL)
    {
        fprintf(stderr, "%s:%u: ", 
                preprocessor->currentFilename, 
                preprocessor->currentLine);
    }
    
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
}

/* Private Functions - Variable Processing ***********************************/

static bool TM32ASM_ProcessLetDirective (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // Store directive start position for output stream
    size_t directiveStartIndex = preprocessor->currentTokenIndex;

    // Skip past the .let directive token
    preprocessor->currentTokenIndex++;

    char* varName = NULL;
    bool hasAssignment = false;
    TM32ASM_Value value = {0};

    if (!TM32ASM_ParseVariableDeclaration(preprocessor, false, &varName, &hasAssignment, &value))
    {
        TM32ASM_Destroy(varName);
        TM32ASM_DestroyValue(&value);
        return false;
    }

    // Store directive end position for output stream
    size_t directiveEndIndex = preprocessor->currentTokenIndex;

    // Convert value to string for storage
    char* valueString = NULL;
    if (hasAssignment)
    {
        valueString = TM32ASM_ValueToString(&value);
        if (valueString == NULL)
        {
            TM32ASM_ReportError(preprocessor, "Failed to convert value to string for variable '%s'", varName);
            TM32ASM_Destroy(varName);
            TM32ASM_DestroyValue(&value);
            return false;
        }
    }

    // Define the variable
    bool success = TM32ASM_DefineVariable(preprocessor, varName, valueString);
    
    // Note: .let and .const directives are not added to the output stream
    // because preprocessing should resolve these directives completely,
    // substituting variable/constant values directly into the code.
    
    TM32ASM_Destroy(varName);
    TM32ASM_Destroy(valueString);
    TM32ASM_DestroyValue(&value);

    return success;
}

static bool TM32ASM_ProcessConstDirective (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // Store directive start position for output stream
    size_t directiveStartIndex = preprocessor->currentTokenIndex;
    
    // Skip past the .const directive token
    preprocessor->currentTokenIndex++;

    char* varName = NULL;
    bool hasAssignment = false;
    TM32ASM_Value value = {0};
    
    // Store current position to find directive end
    size_t beforeParseIndex = preprocessor->currentTokenIndex;

    if (!TM32ASM_ParseVariableDeclaration(preprocessor, true, &varName, &hasAssignment, &value))
    {
        TM32ASM_Destroy(varName);
        TM32ASM_DestroyValue(&value);
        return false;
    }

    if (!hasAssignment)
    {
        TM32ASM_ReportError(preprocessor, "Constants must be explicitly initialized: '%s'", varName);
        TM32ASM_Destroy(varName);
        TM32ASM_DestroyValue(&value);
        return false;
    }

    // Store directive end position for output stream
    size_t directiveEndIndex = preprocessor->currentTokenIndex;

    // Convert value to string for storage
    char* valueString = TM32ASM_ValueToString(&value);
    if (valueString == NULL)
    {
        TM32ASM_ReportError(preprocessor, "Failed to convert value to string for constant '%s'", varName);
        TM32ASM_Destroy(varName);
        TM32ASM_DestroyValue(&value);
        return false;
    }

    // Define the constant
    bool success = TM32ASM_DefineConstant(preprocessor, varName, valueString);
    
    // Note: .let and .const directives are not added to the output stream
    // because preprocessing should resolve these directives completely,
    // substituting variable/constant values directly into the code.
    
    TM32ASM_Destroy(varName);
    TM32ASM_Destroy(valueString);
    TM32ASM_DestroyValue(&value);

    return success;
}

static bool TM32ASM_ProcessIfDirective (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // Store the directive token before advancing
    TM32ASM_Token* directiveToken = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];

    // Skip past the .if directive token
    preprocessor->currentTokenIndex++;

    // Find the end of the conditional expression
    size_t startIndex = preprocessor->currentTokenIndex;
    size_t endIndex = TM32ASM_FindExpressionEnd(preprocessor, startIndex);

    if (startIndex >= endIndex)
    {
        TM32ASM_ReportError(preprocessor, "Expected expression after .if directive");
        return false;
    }

    // Evaluate the conditional expression
    TM32ASM_Value conditionValue = {0};
    if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputStream, startIndex, endIndex, &conditionValue))
    {
        TM32ASM_ReportError(preprocessor, "Failed to evaluate .if condition expression");
        return false;
    }

    // Convert to boolean: non-zero values are true
    bool condition = false;
    if (conditionValue.type == TM32ASM_VT_INTEGER && conditionValue.integerValue != 0)
    {
        condition = true;
    }
    else if (conditionValue.type == TM32ASM_VT_FIXED_POINT && conditionValue.fixedPointValue != 0.0)
    {
        condition = true;
    }
    
    TM32ASM_DestroyValue(&conditionValue);

    // Update current token index to past the expression
    preprocessor->currentTokenIndex = endIndex;

    // Check if we need to expand the conditional stack
    if (preprocessor->conditionalDepth >= preprocessor->conditionalCapacity)
    {
        size_t newCapacity = preprocessor->conditionalCapacity * 2;
        if (newCapacity == 0) newCapacity = 8;
        
        TM32ASM_ConditionalContext* newStack = TM32ASM_Resize(
            preprocessor->conditionalStack, 
            newCapacity, 
            TM32ASM_ConditionalContext
        );
        
        if (newStack == NULL)
        {
            TM32ASM_ReportError(preprocessor, "Failed to expand conditional stack");
            return false;
        }
        
        preprocessor->conditionalStack = newStack;
        preprocessor->conditionalCapacity = newCapacity;
    }

    // Push new conditional context onto the stack
    TM32ASM_ConditionalContext* newContext = &preprocessor->conditionalStack[preprocessor->conditionalDepth];
    newContext->state = condition ? TM32ASM_CS_ACTIVE : TM32ASM_CS_SKIPPING;
    newContext->hasElse = false;
    newContext->conditionMet = condition;  // Track if any condition in this if/elseif chain has been met
    
    preprocessor->conditionalDepth++;

    // Note: Conditional directives are not added to the output stream
    // because preprocessing should resolve these directives completely,
    // leaving only the final processed content.

    return true;
}

static bool TM32ASM_ProcessElseDirective (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // Store the directive token before advancing
    TM32ASM_Token* directiveToken = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];

    // Skip past the .else directive token
    preprocessor->currentTokenIndex++;

    // Check if we're in a conditional block
    if (preprocessor->conditionalDepth == 0)
    {
        TM32ASM_ReportError(preprocessor, ".else without matching .if");
        return false;
    }

    TM32ASM_ConditionalContext* currentContext = &preprocessor->conditionalStack[preprocessor->conditionalDepth - 1];
    
    // Check if we already have an else
    if (currentContext->hasElse)
    {
        TM32ASM_ReportError(preprocessor, "Multiple .else directives in same .if block");
        return false;
    }

    // Update the state based on whether any previous condition was met
    if (currentContext->conditionMet)
    {
        // A previous condition was already satisfied, so skip the else branch
        currentContext->state = TM32ASM_CS_SKIPPING;
    }
    else
    {
        // No previous condition was satisfied, so activate the else branch
        currentContext->state = TM32ASM_CS_ACTIVE;
    }
    
    currentContext->hasElse = true;

    // Note: Conditional directives are not added to the output stream
    // because preprocessing should resolve these directives completely,
    // leaving only the final processed content.

    return true;
}

static bool TM32ASM_ProcessEndifDirective (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // Store the directive token before advancing
    TM32ASM_Token* directiveToken = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];

    // Skip past the .endif directive token
    preprocessor->currentTokenIndex++;

    // Check if we're in a conditional block
    if (preprocessor->conditionalDepth == 0)
    {
        TM32ASM_ReportError(preprocessor, ".endif without matching .if");
        return false;
    }

    // Pop the conditional context from the stack
    preprocessor->conditionalDepth--;

    // Note: Conditional directives are not added to the output stream
    // because preprocessing should resolve these directives completely,
    // leaving only the final processed content.

    return true;
}

static bool TM32ASM_ProcessElseifDirective (
    TM32ASM_Preprocessor*   preprocessor
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);

    // Store the directive token before advancing
    TM32ASM_Token* directiveToken = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];

    // Skip past the .elseif directive token
    preprocessor->currentTokenIndex++;

    // Check if we're in a conditional block
    if (preprocessor->conditionalDepth == 0)
    {
        TM32ASM_ReportError(preprocessor, ".elseif without matching .if");
        return false;
    }

    TM32ASM_ConditionalContext* currentContext = &preprocessor->conditionalStack[preprocessor->conditionalDepth - 1];
    
    // Check if we already have an else
    if (currentContext->hasElse)
    {
        TM32ASM_ReportError(preprocessor, ".elseif after .else in same .if block");
        return false;
    }

    // Find the end of the conditional expression
    size_t startIndex = preprocessor->currentTokenIndex;
    size_t endIndex = TM32ASM_FindExpressionEnd(preprocessor, startIndex);

    if (startIndex >= endIndex)
    {
        TM32ASM_ReportError(preprocessor, "Expected expression after .elseif directive");
        return false;
    }

    // Evaluate the conditional expression only if we haven't already satisfied a condition
    bool shouldEvaluate = (currentContext->state == TM32ASM_CS_SKIPPING && !currentContext->conditionMet);
    bool condition = false;

    if (shouldEvaluate)
    {
        TM32ASM_Value conditionValue = {0};
        if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputStream, startIndex, endIndex, &conditionValue))
        {
            TM32ASM_ReportError(preprocessor, "Failed to evaluate .elseif condition expression");
            return false;
        }

        // Convert to boolean: non-zero values are true
        if (conditionValue.type == TM32ASM_VT_INTEGER && conditionValue.integerValue != 0)
        {
            condition = true;
        }
        else if (conditionValue.type == TM32ASM_VT_FIXED_POINT && conditionValue.fixedPointValue != 0.0)
        {
            condition = true;
        }
        
        TM32ASM_DestroyValue(&conditionValue);
    }

    // Update current token index to past the expression
    preprocessor->currentTokenIndex = endIndex;

    // Update the conditional state
    if (shouldEvaluate && condition)
    {
        // This elseif condition is true and no previous condition was met
        currentContext->state = TM32ASM_CS_ACTIVE;
        currentContext->conditionMet = true;
    }
    else if (!currentContext->conditionMet)
    {
        // No condition has been met yet, keep skipping
        currentContext->state = TM32ASM_CS_SKIPPING;
    }
    // If conditionMet is already true, keep the current state (should be skipping)

    // Note: Conditional directives are not added to the output stream
    // because preprocessing should resolve these directives completely,
    // leaving only the final processed content.

    return true;
}

static bool TM32ASM_ParseVariableDeclaration (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    isConstant,
    char**                  varName,
    bool*                   hasAssignment,
    TM32ASM_Value*          value
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(varName, false);
    TM32ASM_ReturnValueIfNull(hasAssignment, false);
    TM32ASM_ReturnValueIfNull(value, false);

    *varName = NULL;
    *hasAssignment = false;
    memset(value, 0, sizeof(TM32ASM_Value));

    // Check if we have enough tokens
    if (preprocessor->currentTokenIndex >= preprocessor->inputStream->tokenCount)
    {
        TM32ASM_ReportError(preprocessor, "Expected variable name after %s directive", 
                           isConstant ? ".const" : ".let");
        return false;
    }

    // Get the variable name token
    TM32ASM_Token* nameToken = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];
    if (nameToken == NULL || nameToken->type != TM32ASM_TT_IDENTIFIER)
    {
        TM32ASM_ReportError(preprocessor, "Expected identifier after %s directive", 
                           isConstant ? ".const" : ".let");
        return false;
    }

    // Copy the variable name
    *varName = TM32ASM_Create(strlen(nameToken->lexeme) + 1, char);
    if (*varName == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for variable name");
        return false;
    }
    strcpy(*varName, nameToken->lexeme);
    preprocessor->currentTokenIndex++;

    // Check for assignment (explicit with '=' or implicit without '=')
    if (preprocessor->currentTokenIndex < preprocessor->inputStream->tokenCount)
    {
        TM32ASM_Token* nextToken = preprocessor->inputStream->tokens[preprocessor->currentTokenIndex];
        if (nextToken != NULL && nextToken->type == TM32ASM_TT_ASSIGN_EQUAL)
        {
            *hasAssignment = true;
            preprocessor->currentTokenIndex++; // Skip the '=' token

            // Find the end of the expression
            size_t exprStart = preprocessor->currentTokenIndex;
            size_t exprEnd = TM32ASM_FindExpressionEnd(preprocessor, exprStart);

            if (exprStart >= exprEnd)
            {
                TM32ASM_ReportError(preprocessor, "Expected expression after '=' in %s declaration", 
                                   isConstant ? ".const" : ".let");
                return false;
            }

            // Evaluate the expression
            if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputStream, exprStart, exprEnd, value))
            {
                TM32ASM_ReportError(preprocessor, "Failed to evaluate expression in %s declaration", 
                                   isConstant ? ".const" : ".let");
                return false;
            }

            // Advance past the expression
            preprocessor->currentTokenIndex = exprEnd;
        }
        else if (nextToken != NULL && TM32ASM_TokenStartsExpression(nextToken))
        {
            // Implicit assignment (no '=' token, directly followed by expression)
            *hasAssignment = true;

            // Find the end of the expression
            size_t exprStart = preprocessor->currentTokenIndex;
            size_t exprEnd = TM32ASM_FindExpressionEnd(preprocessor, exprStart);

            if (exprStart >= exprEnd)
            {
                TM32ASM_ReportError(preprocessor, "Expected expression in %s declaration", 
                                   isConstant ? ".const" : ".let");
                return false;
            }

            // Evaluate the expression
            if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputStream, exprStart, exprEnd, value))
            {
                TM32ASM_ReportError(preprocessor, "Failed to evaluate expression in %s declaration", 
                                   isConstant ? ".const" : ".let");
                return false;
            }

            // Advance past the expression
            preprocessor->currentTokenIndex = exprEnd;
        }
        else if (nextToken != NULL)
        {
            // Check for compound assignment operators
            bool isCompoundAssignment = false;
            switch (nextToken->type)
            {
                case TM32ASM_TT_ASSIGN_PLUS:
                case TM32ASM_TT_ASSIGN_MINUS:
                case TM32ASM_TT_ASSIGN_TIMES:
                case TM32ASM_TT_ASSIGN_EXPONENT:
                case TM32ASM_TT_ASSIGN_DIVIDE:
                case TM32ASM_TT_ASSIGN_MODULO:
                case TM32ASM_TT_ASSIGN_BITWISE_AND:
                case TM32ASM_TT_ASSIGN_BITWISE_OR:
                case TM32ASM_TT_ASSIGN_BITWISE_XOR:
                case TM32ASM_TT_ASSIGN_LEFT_SHIFT:
                case TM32ASM_TT_ASSIGN_RIGHT_SHIFT:
                    isCompoundAssignment = true;
                    break;
                default:
                    break;
            }

            if (isCompoundAssignment)
            {
                if (isConstant)
                {
                    TM32ASM_ReportError(preprocessor, "Compound assignment operators cannot be used with constants");
                    return false;
                }

                *hasAssignment = true;
                
                // For compound assignment, we need to evaluate the expression with the existing variable value
                TM32ASM_Symbol* existingSymbol = TM32ASM_FindSymbol(preprocessor, *varName);
                if (existingSymbol == NULL || existingSymbol->type != TM32ASM_ST_VARIABLE)
                {
                    TM32ASM_ReportError(preprocessor, "Variable '%s' must be defined before using compound assignment", *varName);
                    return false;
                }

                // Get current value
                TM32ASM_Value currentValue = {0};
                if (existingSymbol->value != NULL)
                {
                    // Parse the existing value - try fixed-point first, then integer
                    char* endPtr;
                    double doubleValue = strtod(existingSymbol->value, &endPtr);
                    if (*endPtr == '\0' && strchr(existingSymbol->value, '.') != NULL)
                    {
                        // Successfully parsed as fixed-point
                        currentValue.type = TM32ASM_VT_FIXED_POINT;
                        currentValue.fixedPointValue = doubleValue;
                    }
                    else
                    {
                        // Try parsing as integer
                        int64_t intValue = strtoll(existingSymbol->value, &endPtr, 10);
                        if (*endPtr == '\0')
                        {
                            currentValue.type = TM32ASM_VT_INTEGER;
                            currentValue.integerValue = intValue;
                        }
                        else
                        {
                            TM32ASM_ReportError(preprocessor, "Invalid value format for variable '%s': %s", *varName, existingSymbol->value);
                            return false;
                        }
                    }
                }
                else
                {
                    currentValue.type = TM32ASM_VT_INTEGER;
                    currentValue.integerValue = 0;
                }

                TM32ASM_TokenType operatorType = nextToken->type;
                preprocessor->currentTokenIndex++; // Skip the compound operator

                // Find and evaluate the RHS expression
                size_t exprStart = preprocessor->currentTokenIndex;
                size_t exprEnd = TM32ASM_FindExpressionEnd(preprocessor, exprStart);

                if (exprStart >= exprEnd)
                {
                    TM32ASM_ReportError(preprocessor, "Expected expression after compound assignment operator");
                    TM32ASM_DestroyValue(&currentValue);
                    return false;
                }

                TM32ASM_Value rhsValue = {0};
                if (!TM32ASM_EvaluateExpression(preprocessor, preprocessor->inputStream, exprStart, exprEnd, &rhsValue))
                {
                    TM32ASM_ReportError(preprocessor, "Failed to evaluate right-hand side of compound assignment");
                    TM32ASM_DestroyValue(&currentValue);
                    return false;
                }

                // Perform the compound operation
                if (!TM32ASM_PerformCompoundAssignment(&currentValue, &rhsValue, operatorType, value))
                {
                    TM32ASM_ReportError(preprocessor, "Failed to perform compound assignment operation");
                    TM32ASM_DestroyValue(&currentValue);
                    TM32ASM_DestroyValue(&rhsValue);
                    return false;
                }

                TM32ASM_DestroyValue(&currentValue);
                TM32ASM_DestroyValue(&rhsValue);
                preprocessor->currentTokenIndex = exprEnd;
            }
        }
    }

    // If no assignment and it's an uninitialized variable, set to 0
    if (!*hasAssignment && !isConstant)
    {
        *hasAssignment = true;
        value->type = TM32ASM_VT_INTEGER;
        value->integerValue = 0;
    }

    return true;
}

static bool TM32ASM_PerformCompoundAssignment (
    const TM32ASM_Value*    lhs,
    const TM32ASM_Value*    rhs,
    TM32ASM_TokenType       operator,
    TM32ASM_Value*          result
)
{
    TM32ASM_ReturnValueIfNull(lhs, false);
    TM32ASM_ReturnValueIfNull(rhs, false);
    TM32ASM_ReturnValueIfNull(result, false);

    // For simplicity, convert both operands to the same type for the operation
    // We'll prioritize fixed-point over integer
    TM32ASM_Value leftVal = *lhs;
    TM32ASM_Value rightVal = *rhs;

    // Convert to compatible types
    if (leftVal.type == TM32ASM_VT_STRING || rightVal.type == TM32ASM_VT_STRING)
    {
        // String operations not supported for compound assignment
        return false;
    }

    // Convert to fixed-point if either operand is fixed-point
    if (leftVal.type == TM32ASM_VT_FIXED_POINT || rightVal.type == TM32ASM_VT_FIXED_POINT)
    {
        if (leftVal.type == TM32ASM_VT_INTEGER)
        {
            leftVal.type = TM32ASM_VT_FIXED_POINT;
            leftVal.fixedPointValue = (double) leftVal.integerValue;
        }
        if (rightVal.type == TM32ASM_VT_INTEGER)
        {
            rightVal.type = TM32ASM_VT_FIXED_POINT;
            rightVal.fixedPointValue = (double) rightVal.integerValue;
        }
    }

    // Perform the operation
    switch (operator)
    {
        case TM32ASM_TT_ASSIGN_PLUS:
            if (leftVal.type == TM32ASM_VT_INTEGER)
            {
                result->type = TM32ASM_VT_INTEGER;
                result->integerValue = leftVal.integerValue + rightVal.integerValue;
            }
            else
            {
                result->type = TM32ASM_VT_FIXED_POINT;
                result->fixedPointValue = leftVal.fixedPointValue + rightVal.fixedPointValue;
            }
            break;

        case TM32ASM_TT_ASSIGN_MINUS:
            if (leftVal.type == TM32ASM_VT_INTEGER)
            {
                result->type = TM32ASM_VT_INTEGER;
                result->integerValue = leftVal.integerValue - rightVal.integerValue;
            }
            else
            {
                result->type = TM32ASM_VT_FIXED_POINT;
                result->fixedPointValue = leftVal.fixedPointValue - rightVal.fixedPointValue;
            }
            break;

        case TM32ASM_TT_ASSIGN_TIMES:
            if (leftVal.type == TM32ASM_VT_INTEGER)
            {
                result->type = TM32ASM_VT_INTEGER;
                result->integerValue = leftVal.integerValue * rightVal.integerValue;
            }
            else
            {
                result->type = TM32ASM_VT_FIXED_POINT;
                result->fixedPointValue = leftVal.fixedPointValue * rightVal.fixedPointValue;
            }
            break;

        case TM32ASM_TT_ASSIGN_EXPONENT:
            if (leftVal.type == TM32ASM_VT_INTEGER)
            {
                result->type = TM32ASM_VT_INTEGER;
                result->integerValue = (int64_t) pow((double) leftVal.integerValue, (double) rightVal.integerValue);
            }
            else
            {
                result->type = TM32ASM_VT_FIXED_POINT;
                result->fixedPointValue = pow(leftVal.fixedPointValue, rightVal.fixedPointValue);
            }
            break;

        case TM32ASM_TT_ASSIGN_DIVIDE:
            if (rightVal.type == TM32ASM_VT_INTEGER && rightVal.integerValue == 0)
            {
                return false; // Division by zero
            }
            if (rightVal.type == TM32ASM_VT_FIXED_POINT && rightVal.fixedPointValue == 0.0)
            {
                return false; // Division by zero
            }

            if (leftVal.type == TM32ASM_VT_INTEGER)
            {
                result->type = TM32ASM_VT_INTEGER;
                result->integerValue = leftVal.integerValue / rightVal.integerValue;
            }
            else
            {
                result->type = TM32ASM_VT_FIXED_POINT;
                result->fixedPointValue = leftVal.fixedPointValue / rightVal.fixedPointValue;
            }
            break;

        case TM32ASM_TT_ASSIGN_MODULO:
            // Modulo is only defined for integers
            if (leftVal.type != TM32ASM_VT_INTEGER || rightVal.type != TM32ASM_VT_INTEGER)
            {
                return false;
            }
            if (rightVal.integerValue == 0)
            {
                return false; // Division by zero
            }
            result->type = TM32ASM_VT_INTEGER;
            result->integerValue = leftVal.integerValue % rightVal.integerValue;
            break;

        case TM32ASM_TT_ASSIGN_BITWISE_AND:
            // Bitwise operations are only defined for integers
            if (leftVal.type != TM32ASM_VT_INTEGER || rightVal.type != TM32ASM_VT_INTEGER)
            {
                return false;
            }
            result->type = TM32ASM_VT_INTEGER;
            result->integerValue = leftVal.integerValue & rightVal.integerValue;
            break;

        case TM32ASM_TT_ASSIGN_BITWISE_OR:
            if (leftVal.type != TM32ASM_VT_INTEGER || rightVal.type != TM32ASM_VT_INTEGER)
            {
                return false;
            }
            result->type = TM32ASM_VT_INTEGER;
            result->integerValue = leftVal.integerValue | rightVal.integerValue;
            break;

        case TM32ASM_TT_ASSIGN_BITWISE_XOR:
            if (leftVal.type != TM32ASM_VT_INTEGER || rightVal.type != TM32ASM_VT_INTEGER)
            {
                return false;
            }
            result->type = TM32ASM_VT_INTEGER;
            result->integerValue = leftVal.integerValue ^ rightVal.integerValue;
            break;

        case TM32ASM_TT_ASSIGN_LEFT_SHIFT:
            if (leftVal.type != TM32ASM_VT_INTEGER || rightVal.type != TM32ASM_VT_INTEGER)
            {
                return false;
            }
            result->type = TM32ASM_VT_INTEGER;
            result->integerValue = leftVal.integerValue << rightVal.integerValue;
            break;

        case TM32ASM_TT_ASSIGN_RIGHT_SHIFT:
            if (leftVal.type != TM32ASM_VT_INTEGER || rightVal.type != TM32ASM_VT_INTEGER)
            {
                return false;
            }
            result->type = TM32ASM_VT_INTEGER;
            result->integerValue = leftVal.integerValue >> rightVal.integerValue;
            break;

        default:
            return false;
    }

    return true;
}

/**
 * Substitutes a variable or constant identifier with its value.
 *
 * @param   preprocessor    The preprocessor.
 * @param   token           The identifier token to potentially substitute.
 * @return                  A new token with the substituted value, or NULL if no substitution.
 */
static TM32ASM_Token* TM32ASM_SubstituteVariable (TM32ASM_Preprocessor* preprocessor, const TM32ASM_Token* token)
{
    printf("DEBUG: SubstituteVariable called with token: '%s'\n", token ? token->lexeme : "NULL");
    if (preprocessor == NULL || token == NULL || token->type != TM32ASM_TT_IDENTIFIER)
    {
        printf("DEBUG: Early return - invalid parameters\n");
        return NULL;
    }

    // Look up the identifier in the symbol table
    TM32ASM_Symbol* symbol = TM32ASM_FindSymbol(preprocessor, token->lexeme);
    if (symbol == NULL || (symbol->type != TM32ASM_ST_VARIABLE && symbol->type != TM32ASM_ST_CONSTANT))
    {
        printf("DEBUG: Early return - symbol not found or wrong type\n");
        return NULL; // Not a variable or constant
    }

    if (symbol->value == NULL)
    {
        printf("DEBUG: Early return - no value stored\n");
        return NULL; // No value stored
    }

    printf("DEBUG: Found symbol '%s' with value '%s'\n", symbol->name, symbol->value);

    // Determine if this is an integer or fixed-point value by trying to parse
    char* endPtr = NULL;
    errno = 0;
    
    // Try parsing as an integer first
    long long intValue = strtoll(symbol->value, &endPtr, 0);
    if (errno == 0 && endPtr != symbol->value && *endPtr == '\0')
    {
        // Successfully parsed as integer - create decimal token
        printf("DEBUG: Parsed '%s' as integer: %lld\n", symbol->value, intValue);
        return TM32ASM_CreateToken(symbol->value, TM32ASM_TT_DECIMAL);
    }
    else
    {
        printf("DEBUG: Failed to parse '%s' as integer, trying float\n", symbol->value);
        // Try parsing as a fixed-point number
        errno = 0;
        double floatValue = strtod(symbol->value, &endPtr);
        if (errno == 0 && endPtr != symbol->value && *endPtr == '\0')
        {
            // Successfully parsed as floating-point
            printf("DEBUG: Parsed '%s' as float: %f, floor: %f\n", symbol->value, floatValue, floor(floatValue));
            // Check if fractional part is zero - if so, treat as integer
            if (floatValue == floor(floatValue))
            {
                // No fractional part, create as integer
                char integerStr[32];
                snprintf(integerStr, sizeof(integerStr), "%.0f", floatValue);
                printf("DEBUG: Converting to integer: '%s'\n", integerStr);
                return TM32ASM_CreateToken(integerStr, TM32ASM_TT_DECIMAL);
            }
            else
            {
                // Has fractional part, keep as fixed-point
                printf("DEBUG: Keeping as fixed-point: '%s'\n", symbol->value);
                return TM32ASM_CreateToken(symbol->value, TM32ASM_TT_FIXED_POINT);
            }
        }
        else
        {
            // Could not parse as number, treat as identifier (don't substitute)
            return NULL;
        }
    }
}

/**
 * @brief   Finds the matching closing parenthesis for an opening parenthesis.
 * 
 * @param   tokens          Token stream to search in.
 * @param   openParenIndex  Index of the opening parenthesis.
 * @param   endIndex        End index to search up to.
 * 
 * @return  Index of matching closing parenthesis, or SIZE_MAX if not found.
 */
static size_t TM32ASM_FindMatchingParen (
    TM32ASM_TokenStream*    tokens,
    size_t                  openParenIndex,
    size_t                  endIndex
);

/**
 * @brief   Parses function arguments from a token stream.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   tokens          Token stream containing the arguments.
 * @param   startIndex      Start index of the arguments.
 * @param   endIndex        End index of the arguments.
 * @param   args            Array to store parsed argument values.
 * @param   maxArgs         Maximum number of arguments to parse.
 * 
 * @return  Number of arguments parsed, or SIZE_MAX on error.
 */
static size_t TM32ASM_ParseFunctionArguments (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          args,
    size_t                  maxArgs
);

/**
 * @brief   Compares two values for ordering.
 * 
 * @param   a   First value to compare.
 * @param   b   Second value to compare.
 * 
 * @return  -1 if a < b, 0 if a == b, 1 if a > b.
 */
static int TM32ASM_CompareValues (
    const TM32ASM_Value*    a,
    const TM32ASM_Value*    b
);

/**
 * @brief   Resolves forward references by evaluating unresolved expressions.
 * 
 * @param   preprocessor    The preprocessor instance.
 * 
 * @return  true if all forward references were resolved, false otherwise.
 */
static bool TM32ASM_ResolveForwardReferences (
    TM32ASM_Preprocessor*   preprocessor
);

/**
 * @brief   Checks if an expression contains unresolved symbols.
 * 
 * @param   preprocessor    The preprocessor instance.
 * @param   expression      The expression string to check.
 * 
 * @return  true if the expression contains unresolved symbols, false otherwise.
 */
static bool TM32ASM_HasUnresolvedSymbols (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             expression
);

/* Public Functions ***********************************************************/

TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ()
{
    TM32ASM_Preprocessor* preprocessor = TM32ASM_CreateZero(1, TM32ASM_Preprocessor);
    if (preprocessor == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM preprocessor");
        return NULL;
    }
    
    if (!TM32ASM_InitializePreprocessor(preprocessor))
    {
        TM32ASM_CleanupPreprocessor(preprocessor);
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

TM32ASM_TokenStream* TM32ASM_ProcessTokenStream (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);
    TM32ASM_ReturnValueIfNull(inputStream, NULL);

    // Set up the input stream
    preprocessor->inputStream = inputStream;
    preprocessor->currentTokenIndex = 0;
    preprocessor->hasErrors = false;

    // Create the output stream
    preprocessor->outputStream = TM32ASM_CreateTokenStream();
    if (preprocessor->outputStream == NULL)
    {
        TM32ASM_ReportError(preprocessor, "Failed to create output token stream");
        return NULL;
    }

    // Process all tokens from the input stream
    while (preprocessor->currentTokenIndex < inputStream->tokenCount && !preprocessor->hasErrors)
    {
        if (!TM32ASM_ProcessToken(preprocessor))
        {
            TM32ASM_ReportError(preprocessor, "Failed to process token at index %zu", preprocessor->currentTokenIndex - 1);
            break;
        }
    }

    // Check for unclosed conditional blocks
    if (preprocessor->conditionalDepth > 0)
    {
        TM32ASM_ReportError(preprocessor, "Unclosed conditional block(s) at end of input");
    }

    // Check for unclosed loop blocks
    if (preprocessor->loopDepth > 0)
    {
        TM32ASM_ReportError(preprocessor, "Unclosed loop block(s) at end of input");
    }

    // Check for unclosed include blocks
    if (preprocessor->includeDepth > 0)
    {
        TM32ASM_ReportError(preprocessor, "Unclosed include block(s) at end of input");
    }

    // Resolve forward references after all symbols have been processed
    if (!preprocessor->hasErrors)
    {
        TM32ASM_ResolveForwardReferences(preprocessor);
    }

    if (preprocessor->hasErrors)
    {
        TM32ASM_DestroyTokenStream(preprocessor->outputStream);
        preprocessor->outputStream = NULL;
        return NULL;
    }

    TM32ASM_TokenStream* result = preprocessor->outputStream;
    preprocessor->outputStream = NULL; // Transfer ownership
    preprocessor->inputStream = NULL; // Clear reference

    return result;
}

bool TM32ASM_AddIncludePath (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             path
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(path, false);

    // Reallocate the include paths array if necessary
    char** newPaths = TM32ASM_Resize(preprocessor->includePaths, preprocessor->includePathCount + 1, char*);
    if (newPaths == NULL)
    {
        TM32ASM_LogErrno("Could not resize include paths array");
        return false;
    }

    preprocessor->includePaths = newPaths;

    // Duplicate the path string
    preprocessor->includePaths[preprocessor->includePathCount] = TM32ASM_Create(strlen(path) + 1, char);
    if (preprocessor->includePaths[preprocessor->includePathCount] == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for include path");
        return false;
    }

    strcpy(preprocessor->includePaths[preprocessor->includePathCount], path);
    preprocessor->includePathCount++;

    return true;
}

bool TM32ASM_DefineVariable (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(name, false);

    // Check if the symbol already exists
    TM32ASM_Symbol* existing = TM32ASM_FindSymbol(preprocessor, name);
    if (existing != NULL)
    {
        if (existing->type != TM32ASM_ST_VARIABLE)
        {
            TM32ASM_ReportError(preprocessor, "Symbol '%s' is already defined as a different type", name);
            return false;
        }

        // Update the existing variable's value
        TM32ASM_Destroy(existing->value);
        if (value != NULL)
        {
            existing->value = TM32ASM_Create(strlen(value) + 1, char);
            if (existing->value == NULL)
            {
                TM32ASM_LogErrno("Could not allocate memory for variable value");
                return false;
            }
            strcpy(existing->value, value);
        }
        else
        {
            existing->value = NULL;
        }

        existing->isDefined = true;
        return true;
    }

    // Create a new variable symbol
    TM32ASM_Symbol symbol = {0};
    symbol.name = TM32ASM_Create(strlen(name) + 1, char);
    if (symbol.name == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for variable name");
        return false;
    }
    strcpy(symbol.name, name);

    if (value != NULL)
    {
        symbol.value = TM32ASM_Create(strlen(value) + 1, char);
        if (symbol.value == NULL)
        {
            TM32ASM_Destroy(symbol.name);
            TM32ASM_LogErrno("Could not allocate memory for variable value");
            return false;
        }
        strcpy(symbol.value, value);
    }
    else
    {
        symbol.value = NULL;
    }

    symbol.type = TM32ASM_ST_VARIABLE;
    symbol.isDefined = true;
    symbol.isResolved = true;  // Direct value assignment is considered resolved
    symbol.expression = NULL;

    return TM32ASM_AddSymbol(preprocessor, &symbol);
}

bool TM32ASM_DefineConstant (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(name, false);
    TM32ASM_ReturnValueIfNull(value, false);

    // Check if the symbol already exists
    TM32ASM_Symbol* existing = TM32ASM_FindSymbol(preprocessor, name);
    if (existing != NULL)
    {
        TM32ASM_ReportError(preprocessor, "Symbol '%s' is already defined", name);
        return false;
    }

    // Create a new constant symbol
    TM32ASM_Symbol symbol = {0};
    symbol.name = TM32ASM_Create(strlen(name) + 1, char);
    if (symbol.name == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for constant name");
        return false;
    }
    strcpy(symbol.name, name);

    symbol.value = TM32ASM_Create(strlen(value) + 1, char);
    if (symbol.value == NULL)
    {
        TM32ASM_Destroy(symbol.name);
        TM32ASM_LogErrno("Could not allocate memory for constant value");
        return false;
    }
    strcpy(symbol.value, value);

    symbol.type = TM32ASM_ST_CONSTANT;
    symbol.isDefined = true;
    symbol.isResolved = true;  // Direct value assignment is considered resolved
    symbol.expression = NULL;

    return TM32ASM_AddSymbol(preprocessor, &symbol);
}

bool TM32ASM_DefineSimpleMacro (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(name, false);
    TM32ASM_ReturnValueIfNull(value, false);

    // Check if the symbol already exists
    TM32ASM_Symbol* existing = TM32ASM_FindSymbol(preprocessor, name);
    if (existing != NULL)
    {
        if (existing->type != TM32ASM_ST_SIMPLE_MACRO)
        {
            TM32ASM_ReportError(preprocessor, "Symbol '%s' is already defined as a different type", name);
            return false;
        }

        // Update the existing macro's value
        TM32ASM_Destroy(existing->value);
        existing->value = TM32ASM_Create(strlen(value) + 1, char);
        if (existing->value == NULL)
        {
            TM32ASM_LogErrno("Could not allocate memory for macro value");
            return false;
        }
        strcpy(existing->value, value);

        existing->isDefined = true;
        return true;
    }

    // Create a new simple macro symbol
    TM32ASM_Symbol symbol = {0};
    symbol.name = TM32ASM_Create(strlen(name) + 1, char);
    if (symbol.name == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for macro name");
        return false;
    }
    strcpy(symbol.name, name);

    symbol.value = TM32ASM_Create(strlen(value) + 1, char);
    if (symbol.value == NULL)
    {
        TM32ASM_Destroy(symbol.name);
        TM32ASM_LogErrno("Could not allocate memory for macro value");
        return false;
    }
    strcpy(symbol.value, value);

    symbol.type = TM32ASM_ST_SIMPLE_MACRO;
    symbol.isDefined = true;
    symbol.isResolved = true;  // Macros are always resolved
    symbol.expression = NULL;

    return TM32ASM_AddSymbol(preprocessor, &symbol);
}

void TM32ASM_SetTreatWarningsAsErrors (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    treatWarningsAsErrors
)
{
    if (preprocessor != NULL)
    {
        preprocessor->treatWarningsAsErrors = treatWarningsAsErrors;
    }
}

bool TM32ASM_ResolveAllForwardReferences (
    TM32ASM_Preprocessor*   preprocessor
)
{
    return TM32ASM_ResolveForwardReferences(preprocessor);
}

bool TM32ASM_HasErrors (
    const TM32ASM_Preprocessor* preprocessor
)
{
    return (preprocessor != NULL) ? preprocessor->hasErrors : true;
}

/* Private Expression Evaluation Functions ***********************************/

/**
 * @brief   Evaluates a ternary conditional expression.
 */
static bool TM32ASM_EvaluateTernary (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a logical OR expression.
 */
static bool TM32ASM_EvaluateLogicalOr (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a logical AND expression.
 */
static bool TM32ASM_EvaluateLogicalAnd (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a comparison expression.
 */
static bool TM32ASM_EvaluateComparison (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a bitwise OR expression.
 */
static bool TM32ASM_EvaluateBitwiseOr (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a bitwise XOR expression.
 */
static bool TM32ASM_EvaluateBitwiseXor (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a bitwise AND expression.
 */
static bool TM32ASM_EvaluateBitwiseAnd (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a bitwise shift expression.
 */
static bool TM32ASM_EvaluateBitwiseShift (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates an addition/subtraction expression.
 */
static bool TM32ASM_EvaluateAdditionSubtraction (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a multiplication/division/modulo expression.
 */
static bool TM32ASM_EvaluateMultiplicationDivision (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates an exponentiation expression.
 */
static bool TM32ASM_EvaluateExponentiation (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a unary expression.
 */
static bool TM32ASM_EvaluateUnary (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Evaluates a primary expression (literals, identifiers, parentheses).
 */
static bool TM32ASM_EvaluatePrimary (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Finds the main operator in an expression at the given precedence level.
 */
static int TM32ASM_FindMainOperator (
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    const TM32ASM_TokenType* operators,
    size_t                  operatorCount
);

/**
 * @brief   Checks if a token is a binary operator.
 */
static bool TM32ASM_IsBinaryOperator (
    TM32ASM_TokenType tokenType
);

/**
 * @brief   Applies a binary operation to two values.
 */
static bool TM32ASM_ApplyBinaryOperation (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenType       operator,
    const TM32ASM_Value*    left,
    const TM32ASM_Value*    right,
    TM32ASM_Value*          result
);

/**
 * @brief   Applies a unary operation to a value.
 */
static bool TM32ASM_ApplyUnaryOperation (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenType       operator,
    const TM32ASM_Value*    operand,
    TM32ASM_Value*          result
);

/**
 * @brief   Converts a numeric token to a value.
 */
static bool TM32ASM_TokenToValue (
    TM32ASM_Preprocessor*   preprocessor,
    const TM32ASM_Token*    token,
    TM32ASM_Value*          result
);

/**
 * @brief   Skips whitespace and finds the matching closing parenthesis.
 */
static int TM32ASM_FindMatchingParenthesis (
    TM32ASM_TokenStream*    tokens,
    size_t                  openIndex,
    size_t                  endIndex
);

/* Public Expression Evaluation Functions ************************************/

TM32ASM_Value TM32ASM_CreateIntegerValue (
    int64_t value
)
{
    TM32ASM_Value result;
    result.type = TM32ASM_VT_INTEGER;
    result.integerValue = value;
    return result;
}

TM32ASM_Value TM32ASM_CreateFixedPointValue (
    double value
)
{
    TM32ASM_Value result;
    result.type = TM32ASM_VT_FIXED_POINT;
    result.fixedPointValue = value;
    return result;
}

TM32ASM_Value TM32ASM_CreateStringValue (
    const char* value
)
{
    TM32ASM_Value result;
    result.type = TM32ASM_VT_STRING;
    result.stringValue = (value != NULL) ? strdup(value) : NULL;
    return result;
}

void TM32ASM_DestroyValue (
    TM32ASM_Value* value
)
{
    if (value != NULL && value->type == TM32ASM_VT_STRING)
    {
        free(value->stringValue);
        value->stringValue = NULL;
    }
}

char* TM32ASM_ValueToString (
    const TM32ASM_Value* value
)
{
    if (value == NULL)
    {
        return NULL;
    }

    switch (value->type)
    {
        case TM32ASM_VT_INTEGER:
        {
            char* result = malloc(32);
            if (result != NULL)
            {
                snprintf(result, 32, "%" PRId64, value->integerValue);
            }
            return result;
        }

        case TM32ASM_VT_FIXED_POINT:
        {
            char* result = malloc(64);
            if (result != NULL)
            {
                // Check if the fixed-point value has no fractional part
                if (value->fixedPointValue == floor(value->fixedPointValue))
                {
                    // Format as integer if no fractional part
                    snprintf(result, 64, "%.0f", value->fixedPointValue);
                }
                else
                {
                    // Format with decimal places if has fractional part
                    snprintf(result, 64, "%.6f", value->fixedPointValue);
                }
            }
            return result;
        }

        case TM32ASM_VT_STRING:
        {
            return (value->stringValue != NULL) ? strdup(value->stringValue) : NULL;
        }

        default:
            return NULL;
    }
}

bool TM32ASM_ViewVariable (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    TM32ASM_Value*          result
)
{
    if (preprocessor == NULL || name == NULL || result == NULL)
    {
        return false;
    }

    // Find the symbol in the symbol table
    TM32ASM_Symbol* symbol = TM32ASM_FindSymbol(preprocessor, name);
    if (symbol == NULL || !symbol->isDefined)
    {
        return false; // Symbol not found or not defined
    }

    // Only variables and constants can be viewed
    if (symbol->type != TM32ASM_ST_VARIABLE && symbol->type != TM32ASM_ST_CONSTANT)
    {
        return false; // Not a variable or constant
    }

    // Check if the symbol has a value
    if (symbol->value == NULL)
    {
        // Return a default value (0) for variables with no explicit value
        *result = TM32ASM_CreateIntegerValue(0);
        return true;
    }

    // Try to parse the value as different types
    char* endPtr;
    
    // Try integer first (including different bases)
    int64_t intValue = strtoll(symbol->value, &endPtr, 0);
    if (*endPtr == '\0')
    {
        *result = TM32ASM_CreateIntegerValue(intValue);
        return true;
    }

    // Try fixed-point
    double doubleValue = strtod(symbol->value, &endPtr);
    if (*endPtr == '\0')
    {
        *result = TM32ASM_CreateFixedPointValue(doubleValue);
        return true;
    }

    // If neither integer nor fixed-point, treat as string
    *result = TM32ASM_CreateStringValue(symbol->value);
    return true;
}

const TM32ASM_Symbol* TM32ASM_GetSymbolTable (
    const TM32ASM_Preprocessor* preprocessor,
    size_t*                     symbolCount
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, NULL);

    // Optionally return the symbol count
    if (symbolCount != NULL)
    {
        *symbolCount = preprocessor->symbolCount;
    }

    // Return read-only access to the symbol table
    return preprocessor->symbols;
}

bool TM32ASM_EvaluateExpression (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    if (preprocessor == NULL || tokens == NULL || result == NULL)
    {
        return false;
    }

    if (startIndex >= endIndex)
    {
        return false;
    }

    // Start with the lowest precedence: ternary operator
    return TM32ASM_EvaluateTernary(preprocessor, tokens, startIndex, endIndex, result);
}

/* Private Expression Evaluation Implementation ******************************/

static bool TM32ASM_EvaluateTernary (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    // Find question mark for ternary operator
    int questionIndex = -1;
    int colonIndex = -1;
    int parenDepth = 0;

    for (size_t i = startIndex; i < endIndex; i++)
    {
        TM32ASM_Token* token = TM32ASM_GetTokenAt(tokens, i);
        if (token == NULL) continue;

        switch (token->type)
        {
            case TM32ASM_TT_OPEN_PARENTHESIS:
                parenDepth++;
                break;
            case TM32ASM_TT_CLOSE_PARENTHESIS:
                parenDepth--;
                break;
            case TM32ASM_TT_QUESTION:
                if (parenDepth == 0 && questionIndex == -1)
                {
                    questionIndex = (int)i;
                }
                break;
            case TM32ASM_TT_COLON:
                if (parenDepth == 0 && questionIndex != -1 && colonIndex == -1)
                {
                    colonIndex = (int)i;
                }
                break;
            default:
                break;
        }
    }

    if (questionIndex != -1 && colonIndex != -1 && questionIndex < colonIndex)
    {
        // Evaluate condition
        TM32ASM_Value condition;
        if (!TM32ASM_EvaluateLogicalOr(preprocessor, tokens, startIndex, questionIndex, &condition))
        {
            return false;
        }

        bool isTrue = false;
        if (condition.type == TM32ASM_VT_INTEGER)
        {
            isTrue = (condition.integerValue != 0);
        }
        else if (condition.type == TM32ASM_VT_FIXED_POINT)
        {
            isTrue = (condition.fixedPointValue != 0.0);
        }
        TM32ASM_DestroyValue(&condition);

        // Evaluate true or false branch
        if (isTrue)
        {
            return TM32ASM_EvaluateLogicalOr(preprocessor, tokens, questionIndex + 1, colonIndex, result);
        }
        else
        {
            return TM32ASM_EvaluateLogicalOr(preprocessor, tokens, colonIndex + 1, endIndex, result);
        }
    }

    // No ternary operator, continue to logical OR
    return TM32ASM_EvaluateLogicalOr(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateLogicalOr (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_LOGICAL_OR };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 1);

    if (operatorIndex != -1)
    {
        TM32ASM_Value left, right;
        if (!TM32ASM_EvaluateLogicalOr(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateLogicalAnd(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, TM32ASM_TT_LOGICAL_OR, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateLogicalAnd(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateLogicalAnd (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_LOGICAL_AND };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 1);

    if (operatorIndex != -1)
    {
        TM32ASM_Value left, right;
        if (!TM32ASM_EvaluateLogicalAnd(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateComparison(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, TM32ASM_TT_LOGICAL_AND, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateComparison(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateComparison (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = {
        TM32ASM_TT_COMPARE_EQUAL, TM32ASM_TT_COMPARE_NOT_EQUAL,
        TM32ASM_TT_COMPARE_LESS, TM32ASM_TT_COMPARE_GREATER,
        TM32ASM_TT_COMPARE_LESS_EQUAL, TM32ASM_TT_COMPARE_GREATER_EQUAL
    };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 6);

    if (operatorIndex != -1)
    {
        TM32ASM_Token* operatorToken = TM32ASM_GetTokenAt(tokens, operatorIndex);
        TM32ASM_Value left, right;
        
        if (!TM32ASM_EvaluateBitwiseOr(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateBitwiseOr(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, operatorToken->type, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateBitwiseOr(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateBitwiseOr (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_BITWISE_OR };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 1);

    if (operatorIndex != -1)
    {
        TM32ASM_Value left, right;
        if (!TM32ASM_EvaluateBitwiseOr(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateBitwiseXor(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, TM32ASM_TT_BITWISE_OR, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateBitwiseXor(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateBitwiseXor (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_BITWISE_XOR };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 1);

    if (operatorIndex != -1)
    {
        TM32ASM_Value left, right;
        if (!TM32ASM_EvaluateBitwiseXor(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateBitwiseAnd(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, TM32ASM_TT_BITWISE_XOR, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateBitwiseAnd(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateBitwiseAnd (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_BITWISE_AND };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 1);

    if (operatorIndex != -1)
    {
        TM32ASM_Value left, right;
        if (!TM32ASM_EvaluateBitwiseAnd(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateBitwiseShift(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, TM32ASM_TT_BITWISE_AND, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateBitwiseShift(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateBitwiseShift (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_LEFT_SHIFT, TM32ASM_TT_RIGHT_SHIFT };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 2);

    if (operatorIndex != -1)
    {
        TM32ASM_Token* operatorToken = TM32ASM_GetTokenAt(tokens, operatorIndex);
        TM32ASM_Value left, right;
        
        if (!TM32ASM_EvaluateBitwiseShift(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateAdditionSubtraction(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, operatorToken->type, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateAdditionSubtraction(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateAdditionSubtraction (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_PLUS, TM32ASM_TT_MINUS };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 2);

    if (operatorIndex != -1)
    {
        TM32ASM_Token* operatorToken = TM32ASM_GetTokenAt(tokens, operatorIndex);
        TM32ASM_Value left, right;
        
        if (!TM32ASM_EvaluateAdditionSubtraction(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateMultiplicationDivision(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, operatorToken->type, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateMultiplicationDivision(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateMultiplicationDivision (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_TIMES, TM32ASM_TT_DIVIDE, TM32ASM_TT_MODULO };
    int operatorIndex = TM32ASM_FindMainOperator(tokens, startIndex, endIndex, operators, 3);

    if (operatorIndex != -1)
    {
        TM32ASM_Token* operatorToken = TM32ASM_GetTokenAt(tokens, operatorIndex);
        TM32ASM_Value left, right;
        
        if (!TM32ASM_EvaluateMultiplicationDivision(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateExponentiation(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, operatorToken->type, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateExponentiation(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateExponentiation (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    // Exponentiation is right-associative, so we find the leftmost operator
    const TM32ASM_TokenType operators[] = { TM32ASM_TT_EXPONENT };
    int operatorIndex = -1;
    int parenDepth = 0;

    for (int i = (int)startIndex; i < (int)endIndex; i++)
    {
        TM32ASM_Token* token = TM32ASM_GetTokenAt(tokens, i);
        if (token == NULL) continue;

        switch (token->type)
        {
            case TM32ASM_TT_OPEN_PARENTHESIS:
                parenDepth++;
                break;
            case TM32ASM_TT_CLOSE_PARENTHESIS:
                parenDepth--;
                break;
            case TM32ASM_TT_EXPONENT:
                if (parenDepth == 0)
                {
                    operatorIndex = i;
                    break;
                }
                break;
            default:
                break;
        }

        if (operatorIndex != -1) break;
    }

    if (operatorIndex != -1)
    {
        TM32ASM_Value left, right;
        if (!TM32ASM_EvaluateUnary(preprocessor, tokens, startIndex, operatorIndex, &left))
        {
            return false;
        }

        if (!TM32ASM_EvaluateExponentiation(preprocessor, tokens, operatorIndex + 1, endIndex, &right))
        {
            TM32ASM_DestroyValue(&left);
            return false;
        }

        bool success = TM32ASM_ApplyBinaryOperation(preprocessor, TM32ASM_TT_EXPONENT, &left, &right, result);
        TM32ASM_DestroyValue(&left);
        TM32ASM_DestroyValue(&right);
        return success;
    }

    return TM32ASM_EvaluateUnary(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluateUnary (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    if (startIndex >= endIndex)
    {
        return false;
    }

    TM32ASM_Token* firstToken = TM32ASM_GetTokenAt(tokens, startIndex);
    if (firstToken == NULL)
    {
        return false;
    }

    // Check for unary operators
    switch (firstToken->type)
    {
        case TM32ASM_TT_PLUS:
        case TM32ASM_TT_MINUS:
        case TM32ASM_TT_LOGICAL_NOT:
        case TM32ASM_TT_BITWISE_NOT:
        {
            TM32ASM_Value operand;
            if (!TM32ASM_EvaluateUnary(preprocessor, tokens, startIndex + 1, endIndex, &operand))
            {
                return false;
            }

            bool success = TM32ASM_ApplyUnaryOperation(preprocessor, firstToken->type, &operand, result);
            TM32ASM_DestroyValue(&operand);
            return success;
        }

        default:
            break;
    }

    return TM32ASM_EvaluatePrimary(preprocessor, tokens, startIndex, endIndex, result);
}

static bool TM32ASM_EvaluatePrimary (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    if (startIndex >= endIndex)
    {
        return false;
    }

    // Handle parentheses
    if (startIndex + 2 < endIndex)
    {
        TM32ASM_Token* firstToken = TM32ASM_GetTokenAt(tokens, startIndex);
        TM32ASM_Token* lastToken = TM32ASM_GetTokenAt(tokens, endIndex - 1);

        if (firstToken != NULL && lastToken != NULL &&
            firstToken->type == TM32ASM_TT_OPEN_PARENTHESIS &&
            lastToken->type == TM32ASM_TT_CLOSE_PARENTHESIS)
        {
            return TM32ASM_EvaluateExpression(preprocessor, tokens, startIndex + 1, endIndex - 1, result);
        }
    }

    // Handle single token
    if (startIndex + 1 == endIndex)
    {
        TM32ASM_Token* token = TM32ASM_GetTokenAt(tokens, startIndex);
        if (token == NULL)
        {
            return false;
        }

        switch (token->type)
        {
            case TM32ASM_TT_BINARY:
            case TM32ASM_TT_OCTAL:
            case TM32ASM_TT_DECIMAL:
            case TM32ASM_TT_HEXADECIMAL:
            case TM32ASM_TT_FIXED_POINT:
            case TM32ASM_TT_CHARACTER:
                return TM32ASM_TokenToValue(preprocessor, token, result);

            case TM32ASM_TT_IDENTIFIER:
            {
                // Check if this is a function call
                if (startIndex + 2 < endIndex)
                {
                    TM32ASM_Token* nextToken = TM32ASM_GetTokenAt(tokens, startIndex + 1);
                    if (nextToken != NULL && nextToken->type == TM32ASM_TT_OPEN_PARENTHESIS)
                    {
                        return TM32ASM_EvaluateFunction(preprocessor, tokens, startIndex, endIndex, result);
                    }
                }

                // Look up symbol in symbol table
                TM32ASM_Symbol* symbol = TM32ASM_FindSymbol(preprocessor, token->lexeme);
                if (symbol != NULL && symbol->isDefined)
                {
                    // For now, parse the value as a number
                    if (symbol->value != NULL)
                    {
                        char* endPtr;
                        int64_t intValue = strtoll(symbol->value, &endPtr, 0);
                        if (*endPtr == '\0')
                        {
                            *result = TM32ASM_CreateIntegerValue(intValue);
                            return true;
                        }

                        double doubleValue = strtod(symbol->value, &endPtr);
                        if (*endPtr == '\0')
                        {
                            *result = TM32ASM_CreateFixedPointValue(doubleValue);
                            return true;
                        }
                    }
                }

                fprintf(stderr, "Error: Undefined symbol '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }

            default:
                fprintf(stderr, "Error: Invalid token in expression\n");
                preprocessor->hasErrors = true;
                return false;
        }
    }

    fprintf(stderr, "Error: Invalid expression\n");
    preprocessor->hasErrors = true;
    return false;
}

bool TM32ASM_EvaluateFunction (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
)
{
    if (preprocessor == NULL || tokens == NULL || result == NULL || startIndex >= endIndex)
    {
        return false;
    }

    TM32ASM_Token* funcNameToken = TM32ASM_GetTokenAt(tokens, startIndex);
    TM32ASM_Token* openParenToken = TM32ASM_GetTokenAt(tokens, startIndex + 1);
    
    if (funcNameToken == NULL || funcNameToken->type != TM32ASM_TT_IDENTIFIER ||
        openParenToken == NULL || openParenToken->type != TM32ASM_TT_OPEN_PARENTHESIS)
    {
        return false;
    }

    // Find matching closing parenthesis
    size_t closeParenIndex = TM32ASM_FindMatchingParen(tokens, startIndex + 1, endIndex);
    if (closeParenIndex == SIZE_MAX)
    {
        fprintf(stderr, "Error: Missing closing parenthesis in function call\n");
        preprocessor->hasErrors = true;
        return false;
    }

    const char* funcName = funcNameToken->lexeme;
    
    // Parse function arguments
    TM32ASM_Value args[16]; // Support up to 16 arguments
    size_t argCount = 0;
    
    if (closeParenIndex > startIndex + 2) // Has arguments
    {
        argCount = TM32ASM_ParseFunctionArguments(preprocessor, tokens, startIndex + 2, closeParenIndex, args, 16);
        if (argCount == SIZE_MAX)
        {
            return false; // Error already reported
        }
    }

    // Mathematical functions
    if (strcmp(funcName, "abs") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: abs() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type == TM32ASM_VT_INTEGER)
        {
            *result = TM32ASM_CreateIntegerValue(args[0].intValue < 0 ? -args[0].intValue : args[0].intValue);
        }
        else if (args[0].type == TM32ASM_VT_FIXED_POINT)
        {
            *result = TM32ASM_CreateFixedPointValue(args[0].fixedPointValue < 0.0 ? -args[0].fixedPointValue : args[0].fixedPointValue);
        }
        else
        {
            fprintf(stderr, "Error: abs() requires numeric argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        return true;
    }
    else if (strcmp(funcName, "min") == 0)
    {
        if (argCount < 2)
        {
            fprintf(stderr, "Error: min() requires at least 2 arguments\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        TM32ASM_Value minVal = args[0];
        for (size_t i = 1; i < argCount; i++)
        {
            if (TM32ASM_CompareValues(&args[i], &minVal) < 0)
            {
                minVal = args[i];
            }
        }
        *result = minVal;
        return true;
    }
    else if (strcmp(funcName, "max") == 0)
    {
        if (argCount < 2)
        {
            fprintf(stderr, "Error: max() requires at least 2 arguments\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        TM32ASM_Value maxVal = args[0];
        for (size_t i = 1; i < argCount; i++)
        {
            if (TM32ASM_CompareValues(&args[i], &maxVal) > 0)
            {
                maxVal = args[i];
            }
        }
        *result = maxVal;
        return true;
    }
    else if (strcmp(funcName, "sqrt") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: sqrt() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        double val = 0.0;
        if (args[0].type == TM32ASM_VT_INTEGER)
        {
            val = (double)args[0].intValue;
        }
        else if (args[0].type == TM32ASM_VT_FIXED_POINT)
        {
            val = args[0].fixedPointValue;
        }
        else
        {
            fprintf(stderr, "Error: sqrt() requires numeric argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (val < 0.0)
        {
            fprintf(stderr, "Error: sqrt() of negative number\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        *result = TM32ASM_CreateFixedPointValue(sqrt(val));
        return true;
    }
    else if (strcmp(funcName, "pow") == 0)
    {
        if (argCount != 2)
        {
            fprintf(stderr, "Error: pow() requires exactly 2 arguments\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        double base = 0.0, exponent = 0.0;
        
        if (args[0].type == TM32ASM_VT_INTEGER) base = (double)args[0].intValue;
        else if (args[0].type == TM32ASM_VT_FIXED_POINT) base = args[0].fixedPointValue;
        else
        {
            fprintf(stderr, "Error: pow() requires numeric arguments\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[1].type == TM32ASM_VT_INTEGER) exponent = (double)args[1].intValue;
        else if (args[1].type == TM32ASM_VT_FIXED_POINT) exponent = args[1].fixedPointValue;
        else
        {
            fprintf(stderr, "Error: pow() requires numeric arguments\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        *result = TM32ASM_CreateFixedPointValue(pow(base, exponent));
        return true;
    }
    // Byte manipulation functions
    else if (strcmp(funcName, "high") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: high() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type != TM32ASM_VT_INTEGER)
        {
            fprintf(stderr, "Error: high() requires integer argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        *result = TM32ASM_CreateIntegerValue((args[0].intValue >> 8) & 0xFF);
        return true;
    }
    else if (strcmp(funcName, "low") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: low() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type != TM32ASM_VT_INTEGER)
        {
            fprintf(stderr, "Error: low() requires integer argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        *result = TM32ASM_CreateIntegerValue(args[0].intValue & 0xFF);
        return true;
    }
    else if (strcmp(funcName, "word") == 0)
    {
        if (argCount != 2)
        {
            fprintf(stderr, "Error: word() requires exactly 2 arguments\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type != TM32ASM_VT_INTEGER || args[1].type != TM32ASM_VT_INTEGER)
        {
            fprintf(stderr, "Error: word() requires integer arguments\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        int64_t high = (args[0].intValue & 0xFF) << 8;
        int64_t low = args[1].intValue & 0xFF;
        *result = TM32ASM_CreateIntegerValue(high | low);
        return true;
    }
    // String functions
    else if (strcmp(funcName, "strlen") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: strlen() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type != TM32ASM_VT_STRING)
        {
            fprintf(stderr, "Error: strlen() requires string argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        *result = TM32ASM_CreateIntegerValue((int64_t)strlen(args[0].stringValue));
        return true;
    }
    else if (strcmp(funcName, "ord") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: ord() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type == TM32ASM_VT_CHARACTER)
        {
            *result = TM32ASM_CreateIntegerValue((int64_t)args[0].characterValue);
            return true;
        }
        else if (args[0].type == TM32ASM_VT_STRING && strlen(args[0].stringValue) > 0)
        {
            *result = TM32ASM_CreateIntegerValue((int64_t)args[0].stringValue[0]);
            return true;
        }
        else
        {
            fprintf(stderr, "Error: ord() requires character or non-empty string argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
    }
    else if (strcmp(funcName, "chr") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: chr() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type != TM32ASM_VT_INTEGER)
        {
            fprintf(stderr, "Error: chr() requires integer argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].intValue < 0 || args[0].intValue > 255)
        {
            fprintf(stderr, "Error: chr() argument out of range (0-255)\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        *result = TM32ASM_CreateCharacterValue((char)args[0].intValue);
        return true;
    }
    // Conditional functions
    else if (strcmp(funcName, "defined") == 0)
    {
        if (argCount != 1)
        {
            fprintf(stderr, "Error: defined() requires exactly 1 argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        if (args[0].type != TM32ASM_VT_IDENTIFIER)
        {
            fprintf(stderr, "Error: defined() requires identifier argument\n");
            preprocessor->hasErrors = true;
            return false;
        }
        
        TM32ASM_Symbol* symbol = TM32ASM_FindSymbol(preprocessor, args[0].stringValue);
        *result = TM32ASM_CreateIntegerValue(symbol != NULL && symbol->isDefined ? 1 : 0);
        return true;
    }
    else
    {
        fprintf(stderr, "Error: Unknown function '%s'\n", funcName);
        preprocessor->hasErrors = true;
        return false;
    }
}

/* Helper Functions ***********************************************************/

static int TM32ASM_FindMainOperator (
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    const TM32ASM_TokenType* operators,
    size_t                  operatorCount
)
{
    int parenDepth = 0;
    int lastOperatorIndex = -1;

    for (size_t i = startIndex; i < endIndex; i++)
    {
        TM32ASM_Token* token = TM32ASM_GetTokenAt(tokens, i);
        if (token == NULL) continue;

        switch (token->type)
        {
            case TM32ASM_TT_OPEN_PARENTHESIS:
                parenDepth++;
                break;
            case TM32ASM_TT_CLOSE_PARENTHESIS:
                parenDepth--;
                break;
            default:
                if (parenDepth == 0)
                {
                    for (size_t j = 0; j < operatorCount; j++)
                    {
                        if (token->type == operators[j])
                        {
                            // Special handling for + and - which can be unary or binary
                            if (token->type == TM32ASM_TT_PLUS || token->type == TM32ASM_TT_MINUS)
                            {
                                // Check if this could be a unary operator
                                bool isUnary = false;
                                
                                if (i == startIndex)
                                {
                                    // Operator at the beginning of expression is unary
                                    isUnary = true;
                                }
                                else
                                {
                                    // Check the previous token to determine if this is unary
                                    TM32ASM_Token* prevToken = TM32ASM_GetTokenAt(tokens, i - 1);
                                    if (prevToken != NULL)
                                    {
                                        // If the previous token is an operator or open parenthesis,
                                        // then this +/- is unary
                                        switch (prevToken->type)
                                        {
                                            case TM32ASM_TT_PLUS:
                                            case TM32ASM_TT_MINUS:
                                            case TM32ASM_TT_TIMES:
                                            case TM32ASM_TT_DIVIDE:
                                            case TM32ASM_TT_MODULO:
                                            case TM32ASM_TT_EXPONENT:
                                            case TM32ASM_TT_BITWISE_AND:
                                            case TM32ASM_TT_BITWISE_OR:
                                            case TM32ASM_TT_BITWISE_XOR:
                                            case TM32ASM_TT_BITWISE_NOT:
                                            case TM32ASM_TT_LEFT_SHIFT:
                                            case TM32ASM_TT_RIGHT_SHIFT:
                                            case TM32ASM_TT_COMPARE_EQUAL:
                                            case TM32ASM_TT_COMPARE_NOT_EQUAL:
                                            case TM32ASM_TT_COMPARE_LESS:
                                            case TM32ASM_TT_COMPARE_GREATER:
                                            case TM32ASM_TT_COMPARE_LESS_EQUAL:
                                            case TM32ASM_TT_COMPARE_GREATER_EQUAL:
                                            case TM32ASM_TT_LOGICAL_AND:
                                            case TM32ASM_TT_LOGICAL_OR:
                                            case TM32ASM_TT_LOGICAL_NOT:
                                            case TM32ASM_TT_OPEN_PARENTHESIS:
                                            case TM32ASM_TT_QUESTION:
                                            case TM32ASM_TT_COLON:
                                            case TM32ASM_TT_COMMA:
                                                isUnary = true;
                                                break;
                                            default:
                                                isUnary = false;
                                                break;
                                        }
                                    }
                                }
                                
                                // If it's unary, skip it for binary operator search
                                if (isUnary)
                                {
                                    continue;
                                }
                            }
                            
                            lastOperatorIndex = (int)i;
                        }
                    }
                }
                break;
        }
    }

    return lastOperatorIndex;
}

static bool TM32ASM_IsBinaryOperator (
    TM32ASM_TokenType tokenType
)
{
    switch (tokenType)
    {
        case TM32ASM_TT_PLUS:
        case TM32ASM_TT_MINUS:
        case TM32ASM_TT_TIMES:
        case TM32ASM_TT_EXPONENT:
        case TM32ASM_TT_DIVIDE:
        case TM32ASM_TT_MODULO:
        case TM32ASM_TT_BITWISE_AND:
        case TM32ASM_TT_BITWISE_OR:
        case TM32ASM_TT_BITWISE_XOR:
        case TM32ASM_TT_LEFT_SHIFT:
        case TM32ASM_TT_RIGHT_SHIFT:
        case TM32ASM_TT_COMPARE_EQUAL:
        case TM32ASM_TT_COMPARE_NOT_EQUAL:
        case TM32ASM_TT_COMPARE_LESS:
        case TM32ASM_TT_COMPARE_GREATER:
        case TM32ASM_TT_COMPARE_LESS_EQUAL:
        case TM32ASM_TT_COMPARE_GREATER_EQUAL:
        case TM32ASM_TT_LOGICAL_AND:
        case TM32ASM_TT_LOGICAL_OR:
            return true;
        default:
            return false;
    }
}

static bool TM32ASM_ApplyBinaryOperation (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenType       operator,
    const TM32ASM_Value*    left,
    const TM32ASM_Value*    right,
    TM32ASM_Value*          result
)
{
    if (left == NULL || right == NULL || result == NULL)
    {
        return false;
    }

    // Convert operands to common type
    int64_t leftInt = 0, rightInt = 0;
    double leftFloat = 0.0, rightFloat = 0.0;
    bool useFloating = false;

    if (left->type == TM32ASM_VT_INTEGER)
    {
        leftInt = left->integerValue;
        leftFloat = (double)left->integerValue;
    }
    else if (left->type == TM32ASM_VT_FIXED_POINT)
    {
        leftInt = (int64_t)left->fixedPointValue;
        leftFloat = left->fixedPointValue;
        useFloating = true;
    }
    else
    {
        fprintf(stderr, "Error: String values not supported in arithmetic operations\n");
        preprocessor->hasErrors = true;
        return false;
    }

    if (right->type == TM32ASM_VT_INTEGER)
    {
        rightInt = right->integerValue;
        rightFloat = (double)right->integerValue;
    }
    else if (right->type == TM32ASM_VT_FIXED_POINT)
    {
        rightInt = (int64_t)right->fixedPointValue;
        rightFloat = right->fixedPointValue;
        useFloating = true;
    }
    else
    {
        fprintf(stderr, "Error: String values not supported in arithmetic operations\n");
        preprocessor->hasErrors = true;
        return false;
    }

    // Perform operation
    switch (operator)
    {
        case TM32ASM_TT_PLUS:
            if (useFloating)
            {
                *result = TM32ASM_CreateFixedPointValue(leftFloat + rightFloat);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt + rightInt);
            }
            break;

        case TM32ASM_TT_MINUS:
            if (useFloating)
            {
                *result = TM32ASM_CreateFixedPointValue(leftFloat - rightFloat);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt - rightInt);
            }
            break;

        case TM32ASM_TT_TIMES:
            if (useFloating)
            {
                *result = TM32ASM_CreateFixedPointValue(leftFloat * rightFloat);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt * rightInt);
            }
            break;

        case TM32ASM_TT_DIVIDE:
            if (useFloating)
            {
                if (rightFloat == 0.0)
                {
                    fprintf(stderr, "Error: Division by zero\n");
                    preprocessor->hasErrors = true;
                    return false;
                }
            }
            else
            {
                if (rightInt == 0)
                {
                    fprintf(stderr, "Error: Division by zero\n");
                    preprocessor->hasErrors = true;
                    return false;
                }
            }
            if (useFloating)
            {
                *result = TM32ASM_CreateFixedPointValue(leftFloat / rightFloat);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt / rightInt);
            }
            break;

        case TM32ASM_TT_MODULO:
            if (rightInt == 0)
            {
                fprintf(stderr, "Error: Modulo by zero\n");
                preprocessor->hasErrors = true;
                return false;
            }
            *result = TM32ASM_CreateIntegerValue(leftInt % rightInt);
            break;

        case TM32ASM_TT_EXPONENT:
        {
            double result_val = pow(leftFloat, rightFloat);
            if (useFloating)
            {
                *result = TM32ASM_CreateFixedPointValue(result_val);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue((int64_t)result_val);
            }
            break;
        }

        case TM32ASM_TT_BITWISE_AND:
            *result = TM32ASM_CreateIntegerValue(leftInt & rightInt);
            break;

        case TM32ASM_TT_BITWISE_OR:
            *result = TM32ASM_CreateIntegerValue(leftInt | rightInt);
            break;

        case TM32ASM_TT_BITWISE_XOR:
            *result = TM32ASM_CreateIntegerValue(leftInt ^ rightInt);
            break;

        case TM32ASM_TT_LEFT_SHIFT:
            *result = TM32ASM_CreateIntegerValue(leftInt << rightInt);
            break;

        case TM32ASM_TT_RIGHT_SHIFT:
            *result = TM32ASM_CreateIntegerValue(leftInt >> rightInt);
            break;

        case TM32ASM_TT_COMPARE_EQUAL:
            if (useFloating)
            {
                *result = TM32ASM_CreateIntegerValue(leftFloat == rightFloat ? 1 : 0);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt == rightInt ? 1 : 0);
            }
            break;

        case TM32ASM_TT_COMPARE_NOT_EQUAL:
            if (useFloating)
            {
                *result = TM32ASM_CreateIntegerValue(leftFloat != rightFloat ? 1 : 0);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt != rightInt ? 1 : 0);
            }
            break;

        case TM32ASM_TT_COMPARE_LESS:
            if (useFloating)
            {
                *result = TM32ASM_CreateIntegerValue(leftFloat < rightFloat ? 1 : 0);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt < rightInt ? 1 : 0);
            }
            break;

        case TM32ASM_TT_COMPARE_GREATER:
            if (useFloating)
            {
                *result = TM32ASM_CreateIntegerValue(leftFloat > rightFloat ? 1 : 0);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt > rightInt ? 1 : 0);
            }
            break;

        case TM32ASM_TT_COMPARE_LESS_EQUAL:
            if (useFloating)
            {
                *result = TM32ASM_CreateIntegerValue(leftFloat <= rightFloat ? 1 : 0);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt <= rightInt ? 1 : 0);
            }
            break;

        case TM32ASM_TT_COMPARE_GREATER_EQUAL:
            if (useFloating)
            {
                *result = TM32ASM_CreateIntegerValue(leftFloat >= rightFloat ? 1 : 0);
            }
            else
            {
                *result = TM32ASM_CreateIntegerValue(leftInt >= rightInt ? 1 : 0);
            }
            break;

        case TM32ASM_TT_LOGICAL_AND:
        {
            bool leftBool = (leftInt != 0 || leftFloat != 0.0);
            bool rightBool = (rightInt != 0 || rightFloat != 0.0);
            *result = TM32ASM_CreateIntegerValue((leftBool && rightBool) ? 1 : 0);
            break;
        }

        case TM32ASM_TT_LOGICAL_OR:
        {
            bool leftBool = (leftInt != 0 || leftFloat != 0.0);
            bool rightBool = (rightInt != 0 || rightFloat != 0.0);
            *result = TM32ASM_CreateIntegerValue((leftBool || rightBool) ? 1 : 0);
            break;
        }

        default:
            fprintf(stderr, "Error: Unsupported binary operator\n");
            preprocessor->hasErrors = true;
            return false;
    }

    return true;
}

static bool TM32ASM_ApplyUnaryOperation (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenType       operator,
    const TM32ASM_Value*    operand,
    TM32ASM_Value*          result
)
{
    if (operand == NULL || result == NULL)
    {
        return false;
    }

    switch (operator)
    {
        case TM32ASM_TT_PLUS:
            *result = *operand;
            if (result->type == TM32ASM_VT_STRING && result->stringValue != NULL)
            {
                result->stringValue = strdup(operand->stringValue);
            }
            break;

        case TM32ASM_TT_MINUS:
            if (operand->type == TM32ASM_VT_INTEGER)
            {
                *result = TM32ASM_CreateIntegerValue(-operand->integerValue);
            }
            else if (operand->type == TM32ASM_VT_FIXED_POINT)
            {
                *result = TM32ASM_CreateFixedPointValue(-operand->fixedPointValue);
            }
            else
            {
                fprintf(stderr, "Error: Unary minus not supported for strings\n");
                preprocessor->hasErrors = true;
                return false;
            }
            break;

        case TM32ASM_TT_LOGICAL_NOT:
        {
            bool isFalse = false;
            if (operand->type == TM32ASM_VT_INTEGER)
            {
                isFalse = (operand->integerValue == 0);
            }
            else if (operand->type == TM32ASM_VT_FIXED_POINT)
            {
                isFalse = (operand->fixedPointValue == 0.0);
            }
            else
            {
                isFalse = (operand->stringValue == NULL || strlen(operand->stringValue) == 0);
            }
            *result = TM32ASM_CreateIntegerValue(isFalse ? 1 : 0);
            break;
        }

        case TM32ASM_TT_BITWISE_NOT:
            if (operand->type == TM32ASM_VT_INTEGER)
            {
                *result = TM32ASM_CreateIntegerValue(~operand->integerValue);
            }
            else
            {
                fprintf(stderr, "Error: Bitwise NOT only supported for integers\n");
                preprocessor->hasErrors = true;
                return false;
            }
            break;

        default:
            fprintf(stderr, "Error: Unsupported unary operator\n");
            preprocessor->hasErrors = true;
            return false;
    }

    return true;
}

static bool TM32ASM_TokenToValue (
    TM32ASM_Preprocessor*   preprocessor,
    const TM32ASM_Token*    token,
    TM32ASM_Value*          result
)
{
    if (token == NULL || result == NULL)
    {
        return false;
    }

    switch (token->type)
    {
        case TM32ASM_TT_BINARY:
        {
            char* endPtr;
            int64_t value = strtoll(token->lexeme + 2, &endPtr, 2); // Skip "0b"
            if (*endPtr != '\0')
            {
                fprintf(stderr, "Error: Invalid binary literal '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }
            *result = TM32ASM_CreateIntegerValue(value);
            break;
        }

        case TM32ASM_TT_OCTAL:
        {
            char* endPtr;
            int64_t value = strtoll(token->lexeme + 2, &endPtr, 8); // Skip "0o"
            if (*endPtr != '\0')
            {
                fprintf(stderr, "Error: Invalid octal literal '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }
            *result = TM32ASM_CreateIntegerValue(value);
            break;
        }

        case TM32ASM_TT_DECIMAL:
        {
            char* endPtr;
            int64_t value = strtoll(token->lexeme, &endPtr, 10);
            if (*endPtr != '\0')
            {
                fprintf(stderr, "Error: Invalid decimal literal '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }
            *result = TM32ASM_CreateIntegerValue(value);
            break;
        }

        case TM32ASM_TT_HEXADECIMAL:
        {
            char* endPtr;
            int64_t value = strtoll(token->lexeme, &endPtr, 16);
            if (*endPtr != '\0')
            {
                fprintf(stderr, "Error: Invalid hexadecimal literal '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }
            *result = TM32ASM_CreateIntegerValue(value);
            break;
        }

        case TM32ASM_TT_FIXED_POINT:
        {
            char* endPtr;
            double value = strtod(token->lexeme, &endPtr);
            if (*endPtr != '\0')
            {
                fprintf(stderr, "Error: Invalid fixed-point literal '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }
            *result = TM32ASM_CreateFixedPointValue(value);
            break;
        }

        case TM32ASM_TT_CHARACTER:
        {
            // Handle character literals like 'A', '\n', '\x41'
            const char* text = token->lexeme;
            if (strlen(text) < 3 || text[0] != '\'' || text[strlen(text) - 1] != '\'')
            {
                fprintf(stderr, "Error: Invalid character literal '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }

            int64_t value = 0;
            if (strlen(text) == 3)
            {
                // Simple character like 'A'
                value = (int64_t)text[1];
            }
            else if (text[1] == '\\')
            {
                // Escape sequence
                switch (text[2])
                {
                    case 'n': value = '\n'; break;
                    case 't': value = '\t'; break;
                    case 'r': value = '\r'; break;
                    case '\\': value = '\\'; break;
                    case '\'': value = '\''; break;
                    case '\"': value = '\"'; break;
                    case '0': value = '\0'; break;
                    case 'x':
                        if (strlen(text) >= 6)
                        {
                            char hexStr[3] = { text[3], text[4], '\0' };
                            char* endPtr;
                            value = strtol(hexStr, &endPtr, 16);
                            if (*endPtr != '\0')
                            {
                                fprintf(stderr, "Error: Invalid hex escape in character literal '%s'\n", token->lexeme);
                                preprocessor->hasErrors = true;
                                return false;
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Error: Invalid hex escape in character literal '%s'\n", token->lexeme);
                            preprocessor->hasErrors = true;
                            return false;
                        }
                        break;
                    default:
                        fprintf(stderr, "Error: Unknown escape sequence in character literal '%s'\n", token->lexeme);
                        preprocessor->hasErrors = true;
                        return false;
                }
            }
            else
            {
                fprintf(stderr, "Error: Invalid character literal '%s'\n", token->lexeme);
                preprocessor->hasErrors = true;
                return false;
            }

            *result = TM32ASM_CreateIntegerValue(value);
            break;
        }

        default:
            fprintf(stderr, "Error: Cannot convert token to value\n");
            preprocessor->hasErrors = true;
            return false;
    }

    return true;
}

static int TM32ASM_FindMatchingParenthesis (
    TM32ASM_TokenStream*    tokens,
    size_t                  openIndex,
    size_t                  endIndex
)
{
    int depth = 1;
    for (size_t i = openIndex + 1; i < endIndex; i++)
    {
        TM32ASM_Token* token = TM32ASM_GetTokenAt(tokens, i);
        if (token == NULL) continue;

        if (token->type == TM32ASM_TT_OPEN_PARENTHESIS)
        {
            depth++;
        }
        else if (token->type == TM32ASM_TT_CLOSE_PARENTHESIS)
        {
            depth--;
            if (depth == 0)
            {
                return (int)i;
            }
        }
    }

    return -1;
}

static size_t TM32ASM_FindMatchingParen (
    TM32ASM_TokenStream*    tokens,
    size_t                  openParenIndex,
    size_t                  endIndex
)
{
    int parenDepth = 1;
    
    for (size_t i = openParenIndex + 1; i < endIndex; i++)
    {
        TM32ASM_Token* token = TM32ASM_GetTokenAt(tokens, i);
        if (token == NULL) continue;
        
        if (token->type == TM32ASM_TT_OPEN_PARENTHESIS)
        {
            parenDepth++;
        }
        else if (token->type == TM32ASM_TT_CLOSE_PARENTHESIS)
        {
            parenDepth--;
            if (parenDepth == 0)
            {
                return i;
            }
        }
    }
    
    return SIZE_MAX; // No matching parenthesis found
}

static size_t TM32ASM_ParseFunctionArguments (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          args,
    size_t                  maxArgs
)
{
    if (startIndex >= endIndex)
    {
        return 0; // No arguments
    }
    
    size_t argCount = 0;
    size_t argStart = startIndex;
    int parenDepth = 0;
    
    for (size_t i = startIndex; i <= endIndex; i++)
    {
        TM32ASM_Token* token = (i < endIndex) ? TM32ASM_GetTokenAt(tokens, i) : NULL;
        
        if (token != NULL)
        {
            if (token->type == TM32ASM_TT_OPEN_PARENTHESIS)
            {
                parenDepth++;
            }
            else if (token->type == TM32ASM_TT_CLOSE_PARENTHESIS)
            {
                parenDepth--;
            }
        }
        
        // Found argument separator or end of arguments
        if ((token != NULL && token->type == TM32ASM_TT_COMMA && parenDepth == 0) || i == endIndex)
        {
            if (argCount >= maxArgs)
            {
                fprintf(stderr, "Error: Too many function arguments (max %zu)\n", maxArgs);
                preprocessor->hasErrors = true;
                return SIZE_MAX;
            }
            
            // Evaluate the argument expression
            if (!TM32ASM_EvaluateExpression(preprocessor, tokens, argStart, i, &args[argCount]))
            {
                return SIZE_MAX; // Error already reported
            }
            
            argCount++;
            argStart = i + 1;
        }
    }
    
    return argCount;
}

static int TM32ASM_CompareValues (
    const TM32ASM_Value*    a,
    const TM32ASM_Value*    b
)
{
    if (a == NULL || b == NULL)
    {
        return 0;
    }
    
    // Convert both values to the same type for comparison
    if (a->type == TM32ASM_VT_INTEGER && b->type == TM32ASM_VT_INTEGER)
    {
        if (a->intValue < b->intValue) return -1;
        if (a->intValue > b->intValue) return 1;
        return 0;
    }
    else if (a->type == TM32ASM_VT_FIXED_POINT && b->type == TM32ASM_VT_FIXED_POINT)
    {
        if (a->fixedPointValue < b->fixedPointValue) return -1;
        if (a->fixedPointValue > b->fixedPointValue) return 1;
        return 0;
    }
    else
    {
        // Convert to double for mixed comparisons
        double aVal = (a->type == TM32ASM_VT_INTEGER) ? (double)a->intValue : a->fixedPointValue;
        double bVal = (b->type == TM32ASM_VT_INTEGER) ? (double)b->intValue : b->fixedPointValue;
        
        if (aVal < bVal) return -1;
        if (aVal > bVal) return 1;
        return 0;
    }
}

TM32ASM_Value TM32ASM_CreateCharacterValue (
    char value
)
{
    TM32ASM_Value result;
    result.type = TM32ASM_VT_CHARACTER;
    result.characterValue = value;
    return result;
}

static bool TM32ASM_ResolveForwardReferences (
    TM32ASM_Preprocessor*   preprocessor
)
{
    if (preprocessor == NULL)
    {
        return false;
    }

    // For now, this is a placeholder implementation
    // Forward reference resolution would be implemented here
    // when we have a proper tokenizer for expression strings
    
    // Check if we have any unresolved symbols and report them
    for (size_t i = 0; i < preprocessor->symbolCount; i++)
    {
        TM32ASM_Symbol* symbol = &preprocessor->symbols[i];
        if (symbol->isDefined && !symbol->isResolved && symbol->expression != NULL)
        {
            fprintf(stderr, "Warning: Symbol '%s' has unresolved expression: %s\n", 
                   symbol->name, symbol->expression);
            
            // For now, just mark as resolved with empty value
            symbol->isResolved = true;
            if (symbol->value == NULL)
            {
                symbol->value = TM32ASM_Create(2, char);
                if (symbol->value != NULL)
                {
                    strcpy(symbol->value, "0");
                }
            }
        }
    }

    return !preprocessor->hasErrors;
}

bool TM32ASM_DefineVariableWithExpression (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             expression
)
{
    TM32ASM_ReturnValueIfNull(preprocessor, false);
    TM32ASM_ReturnValueIfNull(name, false);

    // Check if the symbol already exists
    TM32ASM_Symbol* existing = TM32ASM_FindSymbol(preprocessor, name);
    if (existing != NULL)
    {
        if (existing->type != TM32ASM_ST_VARIABLE)
        {
            TM32ASM_ReportError(preprocessor, "Symbol '%s' is already defined as a different type", name);
            return false;
        }

        // Update the existing variable
        TM32ASM_Destroy(existing->value);
        TM32ASM_Destroy(existing->expression);
        existing->value = NULL;
        existing->expression = NULL;
        existing->isResolved = false;

        if (expression != NULL)
        {
            // Store for later resolution - for now we'll use a simpler approach
            existing->expression = TM32ASM_Create(strlen(expression) + 1, char);
            if (existing->expression != NULL)
            {
                strcpy(existing->expression, expression);
            }
            existing->isResolved = false;
        }
        else
        {
            existing->value = NULL;
            existing->isResolved = true;
        }

        existing->isDefined = true;
        return true;
    }

    // Create a new symbol
    TM32ASM_Symbol symbol = {0};
    symbol.name = TM32ASM_Create(strlen(name) + 1, char);
    if (symbol.name == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for variable name");
        return false;
    }
    strcpy(symbol.name, name);

    symbol.type = TM32ASM_ST_VARIABLE;
    symbol.isDefined = true;
    symbol.isResolved = true;  // Will be set appropriately below
    symbol.expression = NULL;
    symbol.value = NULL;

    if (!TM32ASM_AddSymbol(preprocessor, &symbol))
    {
        TM32ASM_Destroy(symbol.name);
        return false;
    }

    TM32ASM_Symbol* newSymbol = TM32ASM_FindSymbol(preprocessor, name);
    if (newSymbol == NULL)
    {
        return false;
    }

    if (expression != NULL)
    {
        // Store for later resolution - for now we'll use a simpler approach
        newSymbol->expression = TM32ASM_Create(strlen(expression) + 1, char);
        if (newSymbol->expression != NULL)
        {
            strcpy(newSymbol->expression, expression);
        }
        newSymbol->isResolved = false;
    }
    else
    {
        newSymbol->value = NULL;
        newSymbol->isResolved = true;
    }

    newSymbol->isDefined = true;
    return true;
}

static bool TM32ASM_HasUnresolvedSymbols (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             expression
)
{
    if (preprocessor == NULL || expression == NULL)
    {
        return false;
    }

    // For now, we'll do a simple string search for identifiers
    // This is a simplified implementation
    return false; // Placeholder - will be implemented when needed
}
