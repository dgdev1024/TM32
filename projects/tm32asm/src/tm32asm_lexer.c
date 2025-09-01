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
 * @brief   Checks if a character is a valid start for an identifier.
 * 
 * @param   ch  The character to check.
 * 
 * @return  `true` if the character can start an identifier; `false` otherwise.
 */
static bool TM32ASM_IsIdentifierStart (char ch);

/**
 * @brief   Checks if a character is valid within an identifier.
 * 
 * @param   ch  The character to check.
 * 
 * @return  `true` if the character can be part of an identifier; `false` otherwise.
 */
static bool TM32ASM_IsIdentifierChar (char ch);

/**
 * @brief   Checks if a character is a valid digit for the given base.
 * 
 * @param   ch    The character to check.
 * @param   base  The numeric base (2, 8, 10, or 16).
 * 
 * @return  `true` if the character is a valid digit; `false` otherwise.
 */
static bool TM32ASM_IsValidDigit (char ch, int base);

/**
 * @brief   Advances the lexer position by one character.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  The current character before advancing, or '\0' if at end of source.
 */
static char TM32ASM_AdvanceChar (TM32ASM_Lexer* lexer);

/**
 * @brief   Peeks at the current character without advancing the position.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  The current character, or '\0' if at end of source.
 */
static char TM32ASM_PeekChar (const TM32ASM_Lexer* lexer);

/**
 * @brief   Peeks at the character at the specified offset from current position.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * @param   offset  The offset from the current position.
 * 
 * @return  The character at the offset, or '\0' if beyond end of source.
 */
static char TM32ASM_PeekCharAt (const TM32ASM_Lexer* lexer, size_t offset);

/**
 * @brief   Skips whitespace characters.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 */
static void TM32ASM_SkipWhitespace (TM32ASM_Lexer* lexer);

/**
 * @brief   Skips a comment (from ';' to end of line).
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 */
static void TM32ASM_SkipComment (TM32ASM_Lexer* lexer);

/**
 * @brief   Tokenizes an identifier or keyword.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_TokenizeIdentifier (TM32ASM_Lexer* lexer);

/**
 * @brief   Tokenizes a numeric literal.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_TokenizeNumber (TM32ASM_Lexer* lexer);

/**
 * @brief   Tokenizes a character literal.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_TokenizeCharacter (TM32ASM_Lexer* lexer);

/**
 * @brief   Tokenizes a string literal.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_TokenizeString (TM32ASM_Lexer* lexer);

/**
 * @brief   Tokenizes a graphics literal.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_TokenizeGraphics (TM32ASM_Lexer* lexer);

/**
 * @brief   Tokenizes an operator or punctuation.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_TokenizeOperator (TM32ASM_Lexer* lexer);

/**
 * @brief   Tokenizes a newline character.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created newline token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_TokenizeNewline (TM32ASM_Lexer* lexer);

/**
 * @brief   Creates an end-of-file token.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the created EOF token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_CreateEOFToken (TM32ASM_Lexer* lexer);

/**
 * @brief   Creates a token with the current lexer position information.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * @param   lexeme  The token's lexeme string.
 * @param   type    The token type.
 * 
 * @return  A pointer to the created token, or `NULL` on error.
 */
static TM32ASM_Token* TM32ASM_CreateTokenWithPosition (
    const TM32ASM_Lexer*  lexer,
    const char*           lexeme,
    TM32ASM_TokenType     type
);



/* Private Functions **********************************************************/

static bool TM32ASM_IsIdentifierStart (char ch)
{
    return isalpha(ch) || ch == '_' || ch == '.';
}

static bool TM32ASM_IsIdentifierChar (char ch)
{
    return isalnum(ch) || ch == '_';
}

static bool TM32ASM_IsValidDigit (char ch, int base)
{
    switch (base)
    {
        case 2:  return ch >= '0' && ch <= '1';
        case 8:  return ch >= '0' && ch <= '7';
        case 10: return ch >= '0' && ch <= '9';
        case 16: return isdigit(ch) || (tolower(ch) >= 'a' && tolower(ch) <= 'f');
        default: return false;
    }
}

static char TM32ASM_AdvanceChar (TM32ASM_Lexer* lexer)
{
    if (lexer->currentPosition >= lexer->sourceLength)
    {
        return '\0';
    }
    
    char currentChar = lexer->sourceCode[lexer->currentPosition++];
    
    if (currentChar == '\n')
    {
        lexer->currentLine++;
        lexer->currentColumn = 1;
    }
    else
    {
        lexer->currentColumn++;
    }
    
    return currentChar;
}

static char TM32ASM_PeekChar (const TM32ASM_Lexer* lexer)
{
    if (lexer->currentPosition >= lexer->sourceLength)
    {
        return '\0';
    }
    
    return lexer->sourceCode[lexer->currentPosition];
}

static char TM32ASM_PeekCharAt (const TM32ASM_Lexer* lexer, size_t offset)
{
    size_t position = lexer->currentPosition + offset;
    if (position >= lexer->sourceLength)
    {
        return '\0';
    }
    
    return lexer->sourceCode[position];
}

static void TM32ASM_SkipWhitespace (TM32ASM_Lexer* lexer)
{
    char ch;
    while ((ch = TM32ASM_PeekChar(lexer)) != '\0' && isspace(ch) && ch != '\n')
    {
        TM32ASM_AdvanceChar(lexer);
    }
}

static void TM32ASM_SkipComment (TM32ASM_Lexer* lexer)
{
    // Skip the ';' character
    TM32ASM_AdvanceChar(lexer);
    
    // Skip everything until newline or end of source
    while (TM32ASM_PeekChar(lexer) != '\n' && TM32ASM_PeekChar(lexer) != '\0')
    {
        TM32ASM_AdvanceChar(lexer);
    }
}

static TM32ASM_Token* TM32ASM_CreateTokenWithPosition (
    const TM32ASM_Lexer*  lexer,
    const char*           lexeme,
    TM32ASM_TokenType     type
)
{
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, type);
    if (token != NULL)
    {
        token->filename = lexer->filename;
        token->line = lexer->currentLine;
    }
    return token;
}

static TM32ASM_Token* TM32ASM_TokenizeIdentifier (TM32ASM_Lexer* lexer)
{
    size_t startPosition = lexer->currentPosition;
    
    // Advance past the first character (which we know is a valid identifier start)
    TM32ASM_AdvanceChar(lexer);
    
    // Advance past all remaining identifier characters
    while (TM32ASM_IsIdentifierChar(TM32ASM_PeekChar(lexer)))
    {
        TM32ASM_AdvanceChar(lexer);
    }
    
    // Extract the lexeme
    size_t length = lexer->currentPosition - startPosition;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for identifier lexeme");
        return NULL;
    }
    
    strncpy(lexeme, &lexer->sourceCode[startPosition], length);
    lexeme[length] = '\0';
    
    // Create token as identifier initially
    TM32ASM_Token* token = TM32ASM_CreateTokenWithPosition(lexer, lexeme, TM32ASM_TT_IDENTIFIER);
    TM32ASM_Destroy(lexeme);
    
    if (token == NULL)
    {
        return NULL;
    }
    
    // Check if it's actually a keyword
    TM32ASM_Token* keywordToken = NULL;
    if (TM32ASM_GetKeyword(token, &keywordToken))
    {
        TM32ASM_DestroyToken(token);
        return keywordToken;
    }
    
    return token;
}

static TM32ASM_Token* TM32ASM_TokenizeNumber (TM32ASM_Lexer* lexer)
{
    size_t startPosition = lexer->currentPosition;
    TM32ASM_TokenType tokenType = TM32ASM_TT_DECIMAL;
    int base = 10;
    
    // Check for different number formats
    if (TM32ASM_PeekChar(lexer) == '0')
    {
        TM32ASM_AdvanceChar(lexer);
        char nextChar = tolower(TM32ASM_PeekChar(lexer));
        
        if (nextChar == 'b')
        {
            // Binary literal
            tokenType = TM32ASM_TT_BINARY;
            base = 2;
            TM32ASM_AdvanceChar(lexer); // Skip 'b'
        }
        else if (nextChar == 'o')
        {
            // Octal literal
            tokenType = TM32ASM_TT_OCTAL;
            base = 8;
            TM32ASM_AdvanceChar(lexer); // Skip 'o'
        }
        else if (nextChar == 'x')
        {
            // Hexadecimal literal
            tokenType = TM32ASM_TT_HEXADECIMAL;
            base = 16;
            TM32ASM_AdvanceChar(lexer); // Skip 'x'
        }
        else
        {
            // Just a decimal number starting with 0 (including standalone 0)
            tokenType = TM32ASM_TT_DECIMAL;
            base = 10;
            // Note: We already consumed the '0', so continue with remaining digits
        }
    }
    else
    {
        // Regular decimal number, advance past the first digit
        TM32ASM_AdvanceChar(lexer);
    }
    
    // Read digits
    bool hasDigits = (lexer->currentPosition > startPosition); // We may have already consumed the first digit
    while (TM32ASM_IsValidDigit(TM32ASM_PeekChar(lexer), base))
    {
        TM32ASM_AdvanceChar(lexer);
        hasDigits = true;
    }
    
    // Check for fixed-point number (only for decimal)
    if (tokenType == TM32ASM_TT_DECIMAL && TM32ASM_PeekChar(lexer) == '.')
    {
        TM32ASM_AdvanceChar(lexer); // Skip '.'
        tokenType = TM32ASM_TT_FIXED_POINT;
        
        // Read fractional digits
        while (isdigit(TM32ASM_PeekChar(lexer)))
        {
            TM32ASM_AdvanceChar(lexer);
        }
    }
    
    if (!hasDigits)
    {
        TM32ASM_LogError("Invalid number format at line %u, column %u", 
                         lexer->currentLine, lexer->currentColumn);
        return NULL;
    }
    
    // Extract the lexeme
    size_t length = lexer->currentPosition - startPosition;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for number lexeme");
        return NULL;
    }
    
    strncpy(lexeme, &lexer->sourceCode[startPosition], length);
    lexeme[length] = '\0';
    
    TM32ASM_Token* token = TM32ASM_CreateTokenWithPosition(lexer, lexeme, tokenType);
    TM32ASM_Destroy(lexeme);
    
    return token;
}

static TM32ASM_Token* TM32ASM_TokenizeCharacter (TM32ASM_Lexer* lexer)
{
    size_t startPosition = lexer->currentPosition;
    
    // Skip opening quote
    TM32ASM_AdvanceChar(lexer);
    
    // Handle escape sequences and regular characters
    while (TM32ASM_PeekChar(lexer) != '\'' && TM32ASM_PeekChar(lexer) != '\0')
    {
        if (TM32ASM_PeekChar(lexer) == '\\')
        {
            TM32ASM_AdvanceChar(lexer); // Skip backslash
            if (TM32ASM_PeekChar(lexer) != '\0')
            {
                TM32ASM_AdvanceChar(lexer); // Skip escaped character
            }
        }
        else
        {
            TM32ASM_AdvanceChar(lexer);
        }
    }
    
    if (TM32ASM_PeekChar(lexer) != '\'')
    {
        TM32ASM_LogError("Unterminated character literal at line %u, column %u",
                         lexer->currentLine, lexer->currentColumn);
        return NULL;
    }
    
    // Skip closing quote
    TM32ASM_AdvanceChar(lexer);
    
    // Extract the lexeme
    size_t length = lexer->currentPosition - startPosition;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for character lexeme");
        return NULL;
    }
    
    strncpy(lexeme, &lexer->sourceCode[startPosition], length);
    lexeme[length] = '\0';
    
    TM32ASM_Token* token = TM32ASM_CreateTokenWithPosition(lexer, lexeme, TM32ASM_TT_CHARACTER);
    TM32ASM_Destroy(lexeme);
    
    return token;
}

static TM32ASM_Token* TM32ASM_TokenizeString (TM32ASM_Lexer* lexer)
{
    size_t startPosition = lexer->currentPosition;
    
    // Skip opening quote
    TM32ASM_AdvanceChar(lexer);
    
    // Handle escape sequences and regular characters
    while (TM32ASM_PeekChar(lexer) != '"' && TM32ASM_PeekChar(lexer) != '\0')
    {
        if (TM32ASM_PeekChar(lexer) == '\\')
        {
            TM32ASM_AdvanceChar(lexer); // Skip backslash
            if (TM32ASM_PeekChar(lexer) != '\0')
            {
                TM32ASM_AdvanceChar(lexer); // Skip escaped character
            }
        }
        else
        {
            TM32ASM_AdvanceChar(lexer);
        }
    }
    
    if (TM32ASM_PeekChar(lexer) != '"')
    {
        TM32ASM_LogError("Unterminated string literal at line %u, column %u",
                         lexer->currentLine, lexer->currentColumn);
        return NULL;
    }
    
    // Skip closing quote
    TM32ASM_AdvanceChar(lexer);
    
    // Extract the lexeme
    size_t length = lexer->currentPosition - startPosition;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for string lexeme");
        return NULL;
    }
    
    strncpy(lexeme, &lexer->sourceCode[startPosition], length);
    lexeme[length] = '\0';
    
    TM32ASM_Token* token = TM32ASM_CreateTokenWithPosition(lexer, lexeme, TM32ASM_TT_STRING);
    TM32ASM_Destroy(lexeme);
    
    return token;
}

static TM32ASM_Token* TM32ASM_TokenizeGraphics (TM32ASM_Lexer* lexer)
{
    size_t startPosition = lexer->currentPosition;
    
    // Skip opening backtick
    TM32ASM_AdvanceChar(lexer);
    
    // Read graphics data (digits only)
    while (isdigit(TM32ASM_PeekChar(lexer)))
    {
        TM32ASM_AdvanceChar(lexer);
    }
    
    if (TM32ASM_PeekChar(lexer) != '`')
    {
        TM32ASM_LogError("Unterminated graphics literal at line %u, column %u",
                         lexer->currentLine, lexer->currentColumn);
        return NULL;
    }
    
    // Skip closing backtick
    TM32ASM_AdvanceChar(lexer);
    
    // Extract the lexeme
    size_t length = lexer->currentPosition - startPosition;
    char* lexeme = TM32ASM_Create(length + 1, char);
    if (lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for graphics lexeme");
        return NULL;
    }
    
    strncpy(lexeme, &lexer->sourceCode[startPosition], length);
    lexeme[length] = '\0';
    
    TM32ASM_Token* token = TM32ASM_CreateTokenWithPosition(lexer, lexeme, TM32ASM_TT_GRAPHICS);
    TM32ASM_Destroy(lexeme);
    
    return token;
}

static TM32ASM_Token* TM32ASM_TokenizeNewline (TM32ASM_Lexer* lexer)
{
    // Advance past the newline character
    TM32ASM_AdvanceChar(lexer);
    
    // Create the newline token
    return TM32ASM_CreateTokenWithPosition(lexer, "\\n", TM32ASM_TT_NEWLINE);
}

static TM32ASM_Token* TM32ASM_CreateEOFToken (TM32ASM_Lexer* lexer)
{
    // Create the EOF token at current position
    return TM32ASM_CreateTokenWithPosition(lexer, "<EOF>", TM32ASM_TT_END_OF_FILE);
}

static TM32ASM_Token* TM32ASM_TokenizeOperator (TM32ASM_Lexer* lexer)
{
    char currentChar = TM32ASM_PeekChar(lexer);
    char nextChar = TM32ASM_PeekCharAt(lexer, 1);
    
    char lexeme[4] = {0}; // Max 3 characters + null terminator for operators like <<=
    TM32ASM_TokenType tokenType = TM32ASM_TT_UNKNOWN;
    size_t length = 1;
    
    // Check for three-character operators first
    if (currentChar == '*' && nextChar == '*' && TM32ASM_PeekCharAt(lexer, 2) == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_EXPONENT;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        lexeme[2] = TM32ASM_PeekCharAt(lexer, 2);
        length = 3;
    }
    else if (currentChar == '<' && nextChar == '<' && TM32ASM_PeekCharAt(lexer, 2) == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_LEFT_SHIFT;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        lexeme[2] = TM32ASM_PeekCharAt(lexer, 2);
        length = 3;
    }
    else if (currentChar == '>' && nextChar == '>' && TM32ASM_PeekCharAt(lexer, 2) == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_RIGHT_SHIFT;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        lexeme[2] = TM32ASM_PeekCharAt(lexer, 2);
        length = 3;
    }
    // Check for two-character operators second
    else if (currentChar == '*' && nextChar == '*')
    {
        tokenType = TM32ASM_TT_EXPONENT;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '<' && nextChar == '<')
    {
        tokenType = TM32ASM_TT_LEFT_SHIFT;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '>' && nextChar == '>')
    {
        tokenType = TM32ASM_TT_RIGHT_SHIFT;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '=' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_COMPARE_EQUAL;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '!' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_COMPARE_NOT_EQUAL;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '<' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_COMPARE_LESS_EQUAL;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '>' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_COMPARE_GREATER_EQUAL;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '&' && nextChar == '&')
    {
        tokenType = TM32ASM_TT_LOGICAL_AND;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '|' && nextChar == '|')
    {
        tokenType = TM32ASM_TT_LOGICAL_OR;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    // Assignment operators
    else if (currentChar == '+' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_PLUS;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '-' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_MINUS;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '*' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_TIMES;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '/' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_DIVIDE;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '%' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_MODULO;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '&' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_BITWISE_AND;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '|' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_BITWISE_OR;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    else if (currentChar == '^' && nextChar == '=')
    {
        tokenType = TM32ASM_TT_ASSIGN_BITWISE_XOR;
        lexeme[0] = currentChar;
        lexeme[1] = nextChar;
        length = 2;
    }
    // Single character operators
    else
    {
        lexeme[0] = currentChar;
        length = 1;
        
        switch (currentChar)
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
            case '!': tokenType = TM32ASM_TT_LOGICAL_NOT; break;
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
            case '?': tokenType = TM32ASM_TT_QUESTION; break;
            case '#':
                // These characters might be part of macro syntax but standalone
                // they're not recognized in this context
                TM32ASM_LogError("Unexpected character '%c' at line %u, column %u (may be part of macro syntax)",
                                 currentChar, lexer->currentLine, lexer->currentColumn);
                return NULL;
            case '@': 
                if (nextChar == '@')
                {
                    tokenType = TM32ASM_TT_AT;
                    lexeme[1] = nextChar;
                    length = 2;
                }
                else if (nextChar == '?')
                {
                    // Macro unique identifier token @?
                    tokenType = TM32ASM_TT_PARAM_MANGLE_UNIQUE;
                    lexeme[1] = nextChar;
                    length = 2;
                }
                else if (nextChar == '#')
                {
                    // Macro argument count token @#
                    tokenType = TM32ASM_TT_PARAM_COUNT;
                    lexeme[1] = nextChar;
                    length = 2;
                }
                else if (nextChar == '*')
                {
                    // Macro all arguments token @*
                    tokenType = TM32ASM_TT_PARAM_LIST;
                    lexeme[1] = nextChar;
                    length = 2;
                }
                else if (nextChar == '-')
                {
                    // Macro argument decrement token @-
                    tokenType = TM32ASM_TT_PARAM_LIST_NOT_FIRST;
                    lexeme[1] = nextChar;
                    length = 2;
                }
                else if (nextChar == '0')
                {
                    // Macro name parameter token @0
                    tokenType = TM32ASM_TT_PARAM_MACRO_NAME;
                    lexeme[1] = nextChar;
                    length = 2;
                }
                else if (isdigit(nextChar))
                {
                    // Macro positional parameter token @1, @2, etc.
                    tokenType = TM32ASM_TT_PARAM_POSITIONAL;
                    lexeme[1] = nextChar;
                    length = 2;
                    
                    // Check for multi-digit numbers (@10, @11, etc.)
                    size_t extraDigits = 0;
                    while (isdigit(TM32ASM_PeekCharAt(lexer, 2 + extraDigits)) && extraDigits < 2)
                    {
                        lexeme[2 + extraDigits] = TM32ASM_PeekCharAt(lexer, 2 + extraDigits);
                        extraDigits++;
                        length++;
                    }
                }
                else
                {
                    // Handle other @-prefixed identifiers
                    return TM32ASM_TokenizeIdentifier(lexer);
                }
                break;
            default:
                TM32ASM_LogError("Unknown operator '%c' at line %u, column %u",
                                 currentChar, lexer->currentLine, lexer->currentColumn);
                return NULL;
        }
    }
    
    // Advance past the operator
    for (size_t i = 0; i < length; i++)
    {
        TM32ASM_AdvanceChar(lexer);
    }
    
    // Create a null-terminated string for the lexeme
    char* lexemeStr = TM32ASM_Create(length + 1, char);
    if (lexemeStr == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for operator lexeme");
        return NULL;
    }
    
    strncpy(lexemeStr, lexeme, length);
    lexemeStr[length] = '\0';
    
    TM32ASM_Token* token = TM32ASM_CreateTokenWithPosition(lexer, lexemeStr, tokenType);
    TM32ASM_Destroy(lexemeStr);
    
    return token;
}

/* Public Functions ***********************************************************/

TM32ASM_Lexer* TM32ASM_CreateLexer ()
{
    TM32ASM_Lexer* lexer = TM32ASM_CreateZero(1, TM32ASM_Lexer);
    if (lexer == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the TM32ASM lexer");
        return NULL;
    }
    
    lexer->tokenStream = TM32ASM_CreateTokenStream();
    if (lexer->tokenStream == NULL)
    {
        TM32ASM_LogError("Could not create token stream for lexer");
        TM32ASM_Destroy(lexer);
        return NULL;
    }
    
    return lexer;
}

void TM32ASM_DestroyLexer (TM32ASM_Lexer* lexer)
{
    if (lexer != NULL)
    {
        if (lexer->sourceCode != NULL)
        {
            TM32ASM_Destroy(lexer->sourceCode);
        }
        
        if (lexer->tokenStream != NULL)
        {
            TM32ASM_DestroyTokenStream(lexer->tokenStream);
        }
        
        TM32ASM_Destroy(lexer);
    }
}

bool TM32ASM_LoadSourceFile (TM32ASM_Lexer* lexer, const char* filename)
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
    
    // Allocate memory for source code
    lexer->sourceCode = TM32ASM_Create(fileSize + 1, char);
    if (lexer->sourceCode == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for source code");
        fclose(file);
        return false;
    }
    
    // Read file contents
    size_t bytesRead = fread(lexer->sourceCode, 1, fileSize, file);
    fclose(file);
    
    if (bytesRead != (size_t)fileSize)
    {
        TM32ASM_LogError("Could not read entire source file '%s'", filename);
        TM32ASM_Destroy(lexer->sourceCode);
        return false;
    }
    
    lexer->sourceCode[fileSize] = '\0';
    lexer->sourceLength = fileSize;
    lexer->currentPosition = 0;
    lexer->currentLine = 1;
    lexer->currentColumn = 1;
    lexer->filename = filename;
    
    return true;
}

bool TM32ASM_TokenizeSource (TM32ASM_Lexer* lexer)
{
    TM32ASM_ReturnValueIfNull(lexer, false);
    TM32ASM_ReturnValueIfNull(lexer->sourceCode, false);
    TM32ASM_ReturnValueIfNull(lexer->tokenStream, false);
    
    while (lexer->currentPosition < lexer->sourceLength)
    {
        TM32ASM_SkipWhitespace(lexer);
        
        char currentChar = TM32ASM_PeekChar(lexer);
        
        // End of source
        if (currentChar == '\0')
        {
            break;
        }
        
        TM32ASM_Token* token = NULL;
        
        // Handle comments
        if (currentChar == ';')
        {
            TM32ASM_SkipComment(lexer);
            continue;
        }
        
        // Handle newlines
        if (currentChar == '\n')
        {
            token = TM32ASM_TokenizeNewline(lexer);
        }
        // Handle directives (start with '.')
        else if (currentChar == '.')
        {
            token = TM32ASM_TokenizeIdentifier(lexer);
        }
        // Handle identifiers and keywords
        else if (TM32ASM_IsIdentifierStart(currentChar))
        {
            token = TM32ASM_TokenizeIdentifier(lexer);
        }
        // Handle numeric literals
        else if (isdigit(currentChar))
        {
            token = TM32ASM_TokenizeNumber(lexer);
        }
        // Handle character literals
        else if (currentChar == '\'')
        {
            token = TM32ASM_TokenizeCharacter(lexer);
        }
        // Handle string literals
        else if (currentChar == '"')
        {
            token = TM32ASM_TokenizeString(lexer);
        }
        // Handle graphics literals
        else if (currentChar == '`')
        {
            token = TM32ASM_TokenizeGraphics(lexer);
        }
        // Handle operators and punctuation
        else
        {
            token = TM32ASM_TokenizeOperator(lexer);
        }
        
        if (token == NULL)
        {
            TM32ASM_LogError("Failed to tokenize character '%c' at line %u, column %u",
                             currentChar, lexer->currentLine, lexer->currentColumn);
            
            // Advance past the problematic character to avoid infinite loop
            TM32ASM_AdvanceChar(lexer);
            continue;
        }
        
        if (TM32ASM_InsertToken(lexer->tokenStream, token) == NULL)
        {
            TM32ASM_LogError("Failed to insert token into stream");
            TM32ASM_DestroyToken(token);
            return false;
        }
    }
    
    // Add EOF token at the end
    TM32ASM_Token* eofToken = TM32ASM_CreateEOFToken(lexer);
    if (eofToken == NULL)
    {
        TM32ASM_LogError("Failed to create EOF token");
        return false;
    }
    
    if (TM32ASM_InsertToken(lexer->tokenStream, eofToken) == NULL)
    {
        TM32ASM_LogError("Failed to insert EOF token into stream");
        TM32ASM_DestroyToken(eofToken);
        return false;
    }
    
    return true;
}

TM32ASM_TokenStream* TM32ASM_GetTokenStream (const TM32ASM_Lexer* lexer)
{
    TM32ASM_ReturnValueIfNull(lexer, NULL);
    return lexer->tokenStream;
}
