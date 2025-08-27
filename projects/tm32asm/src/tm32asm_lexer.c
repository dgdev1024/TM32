/**
 * @file    tm32asm_lexer.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/27
 * 
 * @brief   Contains the implementation of the TM32ASM lexer functions.
 */

/* Include Files **************************************************************/

#include <tm32asm_lexer.h>

/* Private Function Prototypes ************************************************/

/**
 * @brief   Skips whitespace characters in the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 */
static void TM32ASM_SkipWhitespace (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Skips a comment line in the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 */
static void TM32ASM_SkipComment (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Reads an identifier or keyword from the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the identifier token if successful; `NULL` otherwise.
 */
static TM32ASM_Token* TM32ASM_ReadIdentifier (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Reads a numeric literal from the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the numeric token if successful; `NULL` otherwise.
 */
static TM32ASM_Token* TM32ASM_ReadNumber (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Reads a string literal from the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the string token if successful; `NULL` otherwise.
 */
static TM32ASM_Token* TM32ASM_ReadString (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Reads a character literal from the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the character token if successful; `NULL` otherwise.
 */
static TM32ASM_Token* TM32ASM_ReadCharacter (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Reads an operator token from the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the operator token if successful; `NULL` otherwise.
 */
static TM32ASM_Token* TM32ASM_ReadOperator (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Reads a parameter token from the source code (@ symbols).
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the parameter token if successful; `NULL` otherwise.
 */
static TM32ASM_Token* TM32ASM_ReadParameter (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Peeks at the current character without advancing the position.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  The current character, or '\0' if at end of source.
 */
static char TM32ASM_PeekChar (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Peeks at the next character without advancing the position.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  The next character, or '\0' if at or past end of source.
 */
static char __attribute__((unused)) TM32ASM_PeekNextChar (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Advances to the next character and returns it.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  The current character before advancing, or '\0' if at end.
 */
static char TM32ASM_ConsumeChar (
    TM32ASM_Lexer* lexer
);

/* Public Functions ***********************************************************/

TM32ASM_Lexer* TM32ASM_CreateLexer ()
{
    TM32ASM_Lexer* lexer = TM32ASM_CreateZero(1, TM32ASM_Lexer);
    if (lexer == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM lexer");
        return NULL;
    }
    
    return lexer;
}

void TM32ASM_DestroyLexer (TM32ASM_Lexer* lexer)
{
    if (lexer != NULL)
    {
        TM32ASM_Destroy(lexer->source);
        TM32ASM_Destroy(lexer);
    }
}

bool TM32ASM_LoadSourceFile (
    TM32ASM_Lexer*  lexer,
    const char*     filename
)
{
    TM32ASM_ReturnValueIfNull(lexer, false);
    TM32ASM_ReturnValueIfNull(filename, false);

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        TM32ASM_LogErrno("Could not open source file '%s'", filename);
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize < 0)
    {
        TM32ASM_LogErrno("Could not determine size of source file '%s'", filename);
        fclose(file);
        return false;
    }

    // Allocate buffer for source code
    char* source = TM32ASM_Create(fileSize + 1, char);
    if (source == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for source code");
        fclose(file);
        return false;
    }

    // Read the file
    size_t bytesRead = fread(source, 1, fileSize, file);
    source[bytesRead] = '\0';
    fclose(file);

    // Clean up existing source
    TM32ASM_Destroy(lexer->source);

    // Set up lexer
    lexer->source = source;
    lexer->sourceLength = bytesRead;
    lexer->currentPos = 0;
    lexer->currentLine = 1;
    lexer->filename = filename;

    return true;
}

bool TM32ASM_LoadSourceString (
    TM32ASM_Lexer*  lexer,
    const char*     source,
    const char*     filename
)
{
    TM32ASM_ReturnValueIfNull(lexer, false);
    TM32ASM_ReturnValueIfNull(source, false);

    // Clean up existing source
    TM32ASM_Destroy(lexer->source);

    // Copy the source string
    lexer->source = strdup(source);
    if (lexer->source == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for source code");
        return false;
    }

    // Set up lexer
    lexer->sourceLength = strlen(source);
    lexer->currentPos = 0;
    lexer->currentLine = 1;
    lexer->filename = filename ? filename : "<string>";

    return true;
}

void TM32ASM_ResetLexer (TM32ASM_Lexer* lexer)
{
    if (lexer != NULL)
    {
        lexer->currentPos = 0;
        lexer->currentLine = 1;
    }
}

bool TM32ASM_HasMoreTokens (TM32ASM_Lexer* lexer)
{
    TM32ASM_ReturnValueIfNull(lexer, false);
    return lexer->currentPos < lexer->sourceLength;
}

TM32ASM_Token* TM32ASM_NextToken (TM32ASM_Lexer* lexer)
{
    TM32ASM_ReturnValueIfNull(lexer, NULL);

    // Skip whitespace and comments
    while (TM32ASM_HasMoreTokens(lexer))
    {
        TM32ASM_SkipWhitespace(lexer);
        
        if (!TM32ASM_HasMoreTokens(lexer))
        {
            break;
        }

        char current = TM32ASM_PeekChar(lexer);
        
        // Handle comments
        if (current == ';')
        {
            TM32ASM_SkipComment(lexer);
            continue;
        }

        // Handle identifiers and keywords
        if (isalpha(current) || current == '_' || current == '.')
        {
            return TM32ASM_ReadIdentifier(lexer);
        }

        // Handle numbers
        if (isdigit(current))
        {
            return TM32ASM_ReadNumber(lexer);
        }

        // Handle string literals
        if (current == '"')
        {
            return TM32ASM_ReadString(lexer);
        }

        // Handle character literals
        if (current == '\'')
        {
            return TM32ASM_ReadCharacter(lexer);
        }

        // Handle parameter tokens (@ symbols)
        if (current == '@')
        {
            return TM32ASM_ReadParameter(lexer);
        }

        // Handle operators and punctuation
        TM32ASM_Token* operatorToken = TM32ASM_ReadOperator(lexer);
        if (operatorToken != NULL)
        {
            return operatorToken;
        }

        // Unknown character - skip it and log error
        TM32ASM_LogError("Unknown character '%c' at line %u in %s", 
                        current, lexer->currentLine, lexer->filename);
        TM32ASM_ConsumeChar(lexer);
    }

    return NULL;
}

/* Private Functions **********************************************************/

static char TM32ASM_PeekChar (TM32ASM_Lexer* lexer)
{
    if (lexer->currentPos >= lexer->sourceLength)
    {
        return '\0';
    }
    return lexer->source[lexer->currentPos];
}

static char __attribute__((unused)) TM32ASM_PeekNextChar (TM32ASM_Lexer* lexer)
{
    if (lexer->currentPos + 1 >= lexer->sourceLength)
    {
        return '\0';
    }
    return lexer->source[lexer->currentPos + 1];
}

static char TM32ASM_ConsumeChar (TM32ASM_Lexer* lexer)
{
    if (lexer->currentPos >= lexer->sourceLength)
    {
        return '\0';
    }
    
    char current = lexer->source[lexer->currentPos];
    lexer->currentPos++;
    
    if (current == '\n')
    {
        lexer->currentLine++;
    }
    
    return current;
}

static void TM32ASM_SkipWhitespace (TM32ASM_Lexer* lexer)
{
    while (TM32ASM_HasMoreTokens(lexer))
    {
        char current = TM32ASM_PeekChar(lexer);
        if (!isspace(current))
        {
            break;
        }
        TM32ASM_ConsumeChar(lexer);
    }
}

static void TM32ASM_SkipComment (TM32ASM_Lexer* lexer)
{
    // Skip to end of line
    while (TM32ASM_HasMoreTokens(lexer))
    {
        char current = TM32ASM_ConsumeChar(lexer);
        if (current == '\n')
        {
            break;
        }
    }
}

static TM32ASM_Token* TM32ASM_ReadIdentifier (TM32ASM_Lexer* lexer)
{
    size_t startPos = lexer->currentPos;
    uint32_t line = lexer->currentLine;
    
    // Read identifier characters
    while (TM32ASM_HasMoreTokens(lexer))
    {
        char current = TM32ASM_PeekChar(lexer);
        if (!isalnum(current) && current != '_' && current != '.')
        {
            break;
        }
        TM32ASM_ConsumeChar(lexer);
    }
    
    // Extract the lexeme
    size_t length = lexer->currentPos - startPos;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for identifier lexeme");
        return NULL;
    }
    
    strncpy(lexeme, lexer->source + startPos, length);
    lexeme[length] = '\0';
    
    // Create token
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, TM32ASM_TT_IDENTIFIER);
    TM32ASM_Destroy(lexeme);
    
    if (token == NULL)
    {
        return NULL;
    }
    
    token->filename = lexer->filename;
    token->line = line;
    
    // Check if it's a keyword
    TM32ASM_Token* keywordToken = NULL;
    if (TM32ASM_GetKeyword(token, &keywordToken))
    {
        TM32ASM_DestroyToken(token);
        return keywordToken;
    }
    
    return token;
}

static TM32ASM_Token* TM32ASM_ReadNumber (TM32ASM_Lexer* lexer)
{
    size_t startPos = lexer->currentPos;
    uint32_t line = lexer->currentLine;
    TM32ASM_TokenType tokenType = TM32ASM_TT_DECIMAL;
    
    char first = TM32ASM_PeekChar(lexer);
    
    // Handle different number bases
    if (first == '0')
    {
        TM32ASM_ConsumeChar(lexer);
        
        if (TM32ASM_HasMoreTokens(lexer))
        {
            char second = TM32ASM_PeekChar(lexer);
            
            if (second == 'b' || second == 'B')
            {
                // Binary number
                TM32ASM_ConsumeChar(lexer);
                tokenType = TM32ASM_TT_BINARY;
                while (TM32ASM_HasMoreTokens(lexer))
                {
                    char current = TM32ASM_PeekChar(lexer);
                    if (current != '0' && current != '1')
                    {
                        break;
                    }
                    TM32ASM_ConsumeChar(lexer);
                }
            }
            else if (second == 'x' || second == 'X')
            {
                // Hexadecimal number
                TM32ASM_ConsumeChar(lexer);
                tokenType = TM32ASM_TT_HEXADECIMAL;
                while (TM32ASM_HasMoreTokens(lexer))
                {
                    char current = TM32ASM_PeekChar(lexer);
                    if (!isxdigit(current))
                    {
                        break;
                    }
                    TM32ASM_ConsumeChar(lexer);
                }
            }
            else if (isdigit(second))
            {
                // Octal number
                tokenType = TM32ASM_TT_OCTAL;
                while (TM32ASM_HasMoreTokens(lexer))
                {
                    char current = TM32ASM_PeekChar(lexer);
                    if (current < '0' || current > '7')
                    {
                        break;
                    }
                    TM32ASM_ConsumeChar(lexer);
                }
            }
        }
    }
    else
    {
        // Decimal number
        while (TM32ASM_HasMoreTokens(lexer))
        {
            char current = TM32ASM_PeekChar(lexer);
            if (!isdigit(current))
            {
                break;
            }
            TM32ASM_ConsumeChar(lexer);
        }
    }
    
    // Check for fixed-point decimal
    if (TM32ASM_HasMoreTokens(lexer) && TM32ASM_PeekChar(lexer) == '.')
    {
        TM32ASM_ConsumeChar(lexer);
        tokenType = TM32ASM_TT_FIXED_POINT;
        
        // Read fractional part
        while (TM32ASM_HasMoreTokens(lexer))
        {
            char current = TM32ASM_PeekChar(lexer);
            if (!isdigit(current))
            {
                break;
            }
            TM32ASM_ConsumeChar(lexer);
        }
    }
    
    // Extract the lexeme
    size_t length = lexer->currentPos - startPos;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for number lexeme");
        return NULL;
    }
    
    strncpy(lexeme, lexer->source + startPos, length);
    lexeme[length] = '\0';
    
    // Create token
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, tokenType);
    TM32ASM_Destroy(lexeme);
    
    if (token == NULL)
    {
        return NULL;
    }
    
    token->filename = lexer->filename;
    token->line = line;
    
    return token;
}

static TM32ASM_Token* TM32ASM_ReadString (TM32ASM_Lexer* lexer)
{
    size_t startPos = lexer->currentPos;
    uint32_t line = lexer->currentLine;
    
    // Skip opening quote
    TM32ASM_ConsumeChar(lexer);
    
    while (TM32ASM_HasMoreTokens(lexer))
    {
        char current = TM32ASM_ConsumeChar(lexer);
        
        if (current == '"')
        {
            // End of string
            break;
        }
        else if (current == '\\')
        {
            // Escape sequence - consume next character
            if (TM32ASM_HasMoreTokens(lexer))
            {
                TM32ASM_ConsumeChar(lexer);
            }
        }
        else if (current == '\n')
        {
            // Unterminated string literal
            TM32ASM_LogError("Unterminated string literal at line %u", line);
            return NULL;
        }
    }
    
    // Extract the lexeme (including quotes)
    size_t length = lexer->currentPos - startPos;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for string lexeme");
        return NULL;
    }
    
    strncpy(lexeme, lexer->source + startPos, length);
    lexeme[length] = '\0';
    
    // Create token
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, TM32ASM_TT_STRING);
    TM32ASM_Destroy(lexeme);
    
    if (token == NULL)
    {
        return NULL;
    }
    
    token->filename = lexer->filename;
    token->line = line;
    
    return token;
}

static TM32ASM_Token* TM32ASM_ReadCharacter (TM32ASM_Lexer* lexer)
{
    size_t startPos = lexer->currentPos;
    uint32_t line = lexer->currentLine;
    
    // Skip opening quote
    TM32ASM_ConsumeChar(lexer);
    
    if (!TM32ASM_HasMoreTokens(lexer))
    {
        TM32ASM_LogError("Unterminated character literal at line %u", line);
        return NULL;
    }
    
    char current = TM32ASM_ConsumeChar(lexer);
    
    if (current == '\\')
    {
        // Escape sequence - consume next character
        if (!TM32ASM_HasMoreTokens(lexer))
        {
            TM32ASM_LogError("Unterminated character literal at line %u", line);
            return NULL;
        }
        TM32ASM_ConsumeChar(lexer);
    }
    else if (current == '\n')
    {
        TM32ASM_LogError("Unterminated character literal at line %u", line);
        return NULL;
    }
    
    // Expect closing quote
    if (!TM32ASM_HasMoreTokens(lexer) || TM32ASM_ConsumeChar(lexer) != '\'')
    {
        TM32ASM_LogError("Unterminated character literal at line %u", line);
        return NULL;
    }
    
    // Extract the lexeme (including quotes)
    size_t length = lexer->currentPos - startPos;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for character lexeme");
        return NULL;
    }
    
    strncpy(lexeme, lexer->source + startPos, length);
    lexeme[length] = '\0';
    
    // Create token
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, TM32ASM_TT_CHARACTER);
    TM32ASM_Destroy(lexeme);
    
    if (token == NULL)
    {
        return NULL;
    }
    
    token->filename = lexer->filename;
    token->line = line;
    
    return token;
}

static TM32ASM_Token* TM32ASM_ReadOperator (TM32ASM_Lexer* lexer)
{
    // Peek at the first character to see if it's an operator we recognize
    char first = TM32ASM_PeekChar(lexer);
    
    // Check if this is a character we handle as an operator
    if (first != '+' && first != '-' && first != '*' && first != '/' && 
        first != '%' && first != '&' && first != '|' && first != '^' && 
        first != '~' && first != '=' && first != '<' && first != '>' && 
        first != '!' && first != '(' && first != ')' && first != '[' && 
        first != ']' && first != '{' && first != '}' && first != ',' && 
        first != ':')
    {
        return NULL; // Not an operator character, don't consume it
    }
    
    size_t startPos = lexer->currentPos;
    uint32_t line = lexer->currentLine;
    first = TM32ASM_ConsumeChar(lexer); // Now consume it since we know it's valid
    TM32ASM_TokenType tokenType;
    
    // Handle multi-character operators
    if (TM32ASM_HasMoreTokens(lexer))
    {
        char second = TM32ASM_PeekChar(lexer);
        
        switch (first)
        {
            case '*':
                if (second == '*')
                {
                    TM32ASM_ConsumeChar(lexer);
                    // Check for **=
                    if (TM32ASM_HasMoreTokens(lexer) && TM32ASM_PeekChar(lexer) == '=')
                    {
                        TM32ASM_ConsumeChar(lexer);
                        tokenType = TM32ASM_TT_ASSIGN_EXPONENT;
                    }
                    else
                    {
                        tokenType = TM32ASM_TT_EXPONENT;
                    }
                    break;
                }
                else if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_TIMES;
                    break;
                }
                tokenType = TM32ASM_TT_TIMES;
                break;
                
            case '+':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_PLUS;
                    break;
                }
                tokenType = TM32ASM_TT_PLUS;
                break;
                
            case '-':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_MINUS;
                    break;
                }
                tokenType = TM32ASM_TT_MINUS;
                break;
                
            case '/':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_DIVIDE;
                    break;
                }
                tokenType = TM32ASM_TT_DIVIDE;
                break;
                
            case '%':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_MODULO;
                    break;
                }
                tokenType = TM32ASM_TT_MODULO;
                break;
                
            case '&':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_BITWISE_AND;
                    break;
                }
                tokenType = TM32ASM_TT_BITWISE_AND;
                break;
                
            case '|':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_BITWISE_OR;
                    break;
                }
                tokenType = TM32ASM_TT_BITWISE_OR;
                break;
                
            case '^':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_ASSIGN_BITWISE_XOR;
                    break;
                }
                tokenType = TM32ASM_TT_BITWISE_XOR;
                break;
                
            case '<':
                if (second == '<')
                {
                    TM32ASM_ConsumeChar(lexer);
                    // Check for <<=
                    if (TM32ASM_HasMoreTokens(lexer) && TM32ASM_PeekChar(lexer) == '=')
                    {
                        TM32ASM_ConsumeChar(lexer);
                        tokenType = TM32ASM_TT_ASSIGN_LEFT_SHIFT;
                    }
                    else
                    {
                        tokenType = TM32ASM_TT_LEFT_SHIFT;
                    }
                    break;
                }
                else if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_COMPARE_LESS_EQUAL;
                    break;
                }
                tokenType = TM32ASM_TT_COMPARE_LESS;
                break;
                
            case '>':
                if (second == '>')
                {
                    TM32ASM_ConsumeChar(lexer);
                    // Check for >>=
                    if (TM32ASM_HasMoreTokens(lexer) && TM32ASM_PeekChar(lexer) == '=')
                    {
                        TM32ASM_ConsumeChar(lexer);
                        tokenType = TM32ASM_TT_ASSIGN_RIGHT_SHIFT;
                    }
                    else
                    {
                        tokenType = TM32ASM_TT_RIGHT_SHIFT;
                    }
                    break;
                }
                else if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_COMPARE_GREATER_EQUAL;
                    break;
                }
                tokenType = TM32ASM_TT_COMPARE_GREATER;
                break;
                
            case '=':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_COMPARE_EQUAL;
                    break;
                }
                tokenType = TM32ASM_TT_ASSIGN_EQUAL;
                break;
                
            case '!':
                if (second == '=')
                {
                    TM32ASM_ConsumeChar(lexer);
                    tokenType = TM32ASM_TT_COMPARE_NOT_EQUAL;
                    break;
                }
                goto default_case; // No single ! operator
                
            default:
                goto default_case;
        }
    }
    else
    {
        default_case:
        // Single character operators and punctuation
        switch (first)
        {
            case '+': tokenType = TM32ASM_TT_PLUS; break;
            case '-': tokenType = TM32ASM_TT_MINUS; break;
            case '*': tokenType = TM32ASM_TT_TIMES; break;
            case '/': tokenType = TM32ASM_TT_DIVIDE; break;
            case '%': tokenType = TM32ASM_TT_MODULO; break;
            case '&': tokenType = TM32ASM_TT_BITWISE_AND; break;
            case '|': tokenType = TM32ASM_TT_BITWISE_OR; break;
            case '^': tokenType = TM32ASM_TT_BITWISE_XOR; break;
            case '~': tokenType = TM32ASM_TT_BITWISE_NOT; break;
            case '=': tokenType = TM32ASM_TT_ASSIGN_EQUAL; break;
            case '<': tokenType = TM32ASM_TT_COMPARE_LESS; break;
            case '>': tokenType = TM32ASM_TT_COMPARE_GREATER; break;
            case '(': tokenType = TM32ASM_TT_OPEN_PARENTHESIS; break;
            case ')': tokenType = TM32ASM_TT_CLOSE_PARENTHESIS; break;
            case '[': tokenType = TM32ASM_TT_OPEN_BRACKET; break;
            case ']': tokenType = TM32ASM_TT_CLOSE_BRACKET; break;
            case '{': tokenType = TM32ASM_TT_OPEN_BRACE; break;
            case '}': tokenType = TM32ASM_TT_CLOSE_BRACE; break;
            case ',': tokenType = TM32ASM_TT_COMMA; break;
            case ':': tokenType = TM32ASM_TT_COLON; break;
            default:
                TM32ASM_LogError("Unexpected character '%c' at line %u", first, line);
                return NULL;
        }
    }
    
    // Extract the lexeme
    size_t length = lexer->currentPos - startPos;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for operator lexeme");
        return NULL;
    }
    
    strncpy(lexeme, lexer->source + startPos, length);
    lexeme[length] = '\0';
    
    // Create token
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, tokenType);
    TM32ASM_Destroy(lexeme);
    
    if (token == NULL)
    {
        return NULL;
    }
    
    token->filename = lexer->filename;
    token->line = line;
    
    return token;
}

static TM32ASM_Token* TM32ASM_ReadParameter (TM32ASM_Lexer* lexer)
{
    size_t startPos = lexer->currentPos;
    uint32_t line = lexer->currentLine;
    TM32ASM_TokenType tokenType;
    
    // Consume the initial '@'
    TM32ASM_ConsumeChar(lexer);
    
    if (!TM32ASM_HasMoreTokens(lexer))
    {
        TM32ASM_LogError("Unexpected end of file after '@' at line %u", line);
        return NULL;
    }
    
    char nextChar = TM32ASM_PeekChar(lexer);
    
    // Handle different parameter token patterns
    switch (nextChar)
    {
        case '@':
            // @@ -> literal @ symbol
            TM32ASM_ConsumeChar(lexer);
            tokenType = TM32ASM_TT_AT;
            break;
            
        case '#':
            // @# -> parameter count
            TM32ASM_ConsumeChar(lexer);
            tokenType = TM32ASM_TT_PARAM_COUNT;
            break;
            
        case '*':
            // @* -> parameter list
            TM32ASM_ConsumeChar(lexer);
            tokenType = TM32ASM_TT_PARAM_LIST;
            break;
            
        case '-':
            // @- -> parameter list (not first)
            TM32ASM_ConsumeChar(lexer);
            tokenType = TM32ASM_TT_PARAM_LIST_NOT_FIRST;
            break;
            
        case '?':
            // @? -> unique name generation
            TM32ASM_ConsumeChar(lexer);
            tokenType = TM32ASM_TT_PARAM_MANGLE_UNIQUE;
            break;
            
        case '0':
            // @0 -> macro name
            TM32ASM_ConsumeChar(lexer);
            tokenType = TM32ASM_TT_PARAM_MACRO_NAME;
            break;
            
        default:
            // Check for positional parameters (@1, @2, @3, etc.)
            if (isdigit(nextChar))
            {
                // Read the number
                while (TM32ASM_HasMoreTokens(lexer) && isdigit(TM32ASM_PeekChar(lexer)))
                {
                    TM32ASM_ConsumeChar(lexer);
                }
                tokenType = TM32ASM_TT_PARAM_POSITIONAL;
            }
            else if (isalpha(nextChar) || nextChar == '_')
            {
                // Check for @NARG (alternative to @#)
                size_t tempPos = lexer->currentPos;
                
                // Read the identifier
                while (TM32ASM_HasMoreTokens(lexer))
                {
                    char ch = TM32ASM_PeekChar(lexer);
                    if (!isalnum(ch) && ch != '_')
                    {
                        break;
                    }
                    TM32ASM_ConsumeChar(lexer);
                }
                
                // Extract the identifier to check if it's NARG
                size_t identLength = lexer->currentPos - tempPos;
                if (identLength == 4 && 
                    strncasecmp(lexer->source + tempPos, "NARG", 4) == 0)
                {
                    tokenType = TM32ASM_TT_PARAM_COUNT;
                }
                else
                {
                    // Unknown parameter token - reset position
                    lexer->currentPos = tempPos;
                    TM32ASM_LogError("Unknown parameter token at line %u", line);
                    return NULL;
                }
            }
            else
            {
                TM32ASM_LogError("Invalid parameter token at line %u", line);
                return NULL;
            }
            break;
    }
    
    // Extract the lexeme
    size_t length = lexer->currentPos - startPos;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for parameter lexeme");
        return NULL;
    }
    
    strncpy(lexeme, lexer->source + startPos, length);
    lexeme[length] = '\0';
    
    // Create token
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, tokenType);
    TM32ASM_Destroy(lexeme);
    
    if (token == NULL)
    {
        return NULL;
    }
    
    token->filename = lexer->filename;
    token->line = line;
    
    return token;
}
