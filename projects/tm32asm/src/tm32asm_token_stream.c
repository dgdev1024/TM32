/**
 * @file    tm32asm_token_stream.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/27
 * 
 * @brief   Implementation of the token stream for the TM32 assembler.
 */

/* Include Files **************************************************************/

#include <tm32asm_token_stream.h>

/* Private Function Prototypes ************************************************/

/**
 * @brief   Resizes the given token stream to accommodate at least the specified
 *          number of tokens, using an optimized growth strategy.
 * 
 * @param   stream          A pointer to the token stream to resize.
 * @param   minCapacity     The minimum capacity required.
 * 
 * @return  `true` if the token stream was resized successfully or did not need
 *          resizing; `false` if the token stream could not be resized.
 */
static bool TM32ASM_ResizeTokenStreamTo (
    TM32ASM_TokenStream*    stream,
    size_t                  minCapacity
);

/**
 * @brief   Resizes the given token stream, if needed, whenever a new token is
 *          to be inserted or emplaced into it.
 * 
 * @param   stream  A pointer to the token stream to resize.
 * 
 * @return  `true` if the token stream was resized successfully or did not need
 *          resizing; `false` if the token stream could not be resized.
 */
static bool TM32ASM_ResizeTokenStream (
    TM32ASM_TokenStream* stream
);

/**
 * @brief   Creates a new token with validated parameters. Helper function to
 *          reduce code duplication and improve error handling.
 * 
 * @param   type    The type of the new token.
 * @param   lexeme  The lexeme (text) of the new token.
 * 
 * @return  A pointer to the newly created token if successful, or `NULL` if an
 *          error occurs.
 */
static TM32ASM_Token* TM32ASM_CreateTokenSafe (
    TM32ASM_TokenType   type,
    const char*         lexeme
);

/**
 * @brief   Validates common parameters for token stream operations.
 * 
 * @param   stream  A pointer to the token stream to validate.
 * @param   lexeme  The lexeme string to validate (can be NULL if not needed).
 * 
 * @return  `true` if parameters are valid; `false` otherwise.
 */
static inline bool TM32ASM_ValidateStreamParameters (
    TM32ASM_TokenStream*    stream,
    const char*             lexeme
)
{
    if (stream == NULL)
    {
        TM32ASM_LogError("Invalid token stream parameter (NULL)");
        return false;
    }
    
    if (lexeme != NULL && lexeme[0] == '\0')
    {
        TM32ASM_LogError("Empty lexeme string provided");
        return false;
    }
    
    return true;
}

/* Private Functions **********************************************************/

static bool TM32ASM_ResizeTokenStreamTo (
    TM32ASM_TokenStream*    stream,
    size_t                  minCapacity
)
{
    TM32ASM_HardAssert(stream != NULL);

    // If we already have sufficient capacity, no work needed
    if (minCapacity <= stream->tokenCapacity)
    {
        return true;
    }

    // Check for overflow conditions
    if (minCapacity > TM32ASM_TOKEN_STREAM_MAX_CAPACITY)
    {
        TM32ASM_LogError("Requested token stream capacity exceeds maximum allowed");
        return false;
    }

    // Calculate new capacity using growth factor, but ensure it meets minimum requirement
    size_t newCapacity = stream->tokenCapacity;
    if (newCapacity == 0)
    {
        newCapacity = TM32ASM_TOKEN_STREAM_INITIAL_CAPACITY;
    }

    // Use growth factor until we meet or exceed the minimum requirement
    while (newCapacity < minCapacity)
    {
        size_t nextCapacity = (size_t)(newCapacity * TM32ASM_TOKEN_STREAM_GROWTH_FACTOR);
        
        // Handle potential overflow or insufficient growth
        if (nextCapacity <= newCapacity || nextCapacity > TM32ASM_TOKEN_STREAM_MAX_CAPACITY)
        {
            newCapacity = minCapacity;
            break;
        }
        
        newCapacity = nextCapacity;
    }

    // Resize the array
    TM32ASM_Token** tokens = TM32ASM_Resize(stream->tokens, newCapacity, TM32ASM_Token*);
    if (tokens == NULL)
    {
        TM32ASM_LogErrno("Could not resize token stream array");
        return false;
    }

    stream->tokens = tokens;
    stream->tokenCapacity = newCapacity;
    return true;
}

bool TM32ASM_ResizeTokenStream (
    TM32ASM_TokenStream* stream
)
{
    return TM32ASM_ResizeTokenStreamTo(stream, stream->tokenCount + 1);
}

TM32ASM_Token* TM32ASM_CreateTokenSafe (
    TM32ASM_TokenType   type,
    const char*         lexeme
)
{
    // Input validation
    if (lexeme == NULL)
    {
        TM32ASM_LogError("Invalid lexeme parameter (NULL)");
        return NULL;
    }

    // Create the new token
    TM32ASM_Token* token = TM32ASM_CreateToken(lexeme, type);
    if (token == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for new token");
        return NULL;
    }

    return token;
}

/* Public Functions ***********************************************************/

TM32ASM_TokenStream* TM32ASM_CreateTokenStream ()
{
    TM32ASM_TokenStream* stream = TM32ASM_CreateZero(1, TM32ASM_TokenStream);
    if (stream == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for token stream");
        return NULL;
    }

    stream->tokens = TM32ASM_Create(TM32ASM_TOKEN_STREAM_INITIAL_CAPACITY,
        TM32ASM_Token*);
    if (stream->tokens == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for token stream array");
        TM32ASM_DestroyTokenStream(stream);
        return NULL;
    }

    // Initialize capacity properly
    stream->tokenCapacity = TM32ASM_TOKEN_STREAM_INITIAL_CAPACITY;
    
    return stream;
}

void TM32ASM_DestroyTokenStream (
    TM32ASM_TokenStream* stream
)
{
    if (stream != NULL)
    {
        for (size_t i = 0; i < stream->tokenCount; i++)
        {
            TM32ASM_DestroyToken(stream->tokens[i]);
        }

        TM32ASM_Destroy(stream->tokens);
        TM32ASM_Destroy(stream);
    }
}

bool TM32ASM_ReserveTokens (
    TM32ASM_TokenStream*    stream,
    size_t                  capacity
)
{
    TM32ASM_ReturnValueIfNull(stream, false);
    return TM32ASM_ResizeTokenStreamTo(stream, capacity);
}

TM32ASM_Token* TM32ASM_InsertToken (
    TM32ASM_TokenStream*    stream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);
    TM32ASM_ReturnValueIfNull(token, NULL);

    // Check the position of the write pointer. If it's out of bounds, correct
    // it to the end of the token stream and do an push-back instead.
    if (stream->tokenWritePointer > stream->tokenCount)
    {
        stream->tokenWritePointer = stream->tokenCount;
        return TM32ASM_PushTokenBack(stream, token);
    }

    // Resize the token stream, in case it's necessary.
    if (!TM32ASM_ResizeTokenStream(stream))
    {
        return NULL;
    }

    // Calculate elements to move for better readability
    size_t elementsToMove = stream->tokenCount - stream->tokenWritePointer;
    
    // Insert the new token into the token stream at the current write pointer,
    // pushing any existing tokens one place to the right.
    if (elementsToMove > 0)
    {
        memmove(&stream->tokens[stream->tokenWritePointer + 1],
                &stream->tokens[stream->tokenWritePointer],
                elementsToMove * sizeof(TM32ASM_Token*));
    }

    stream->tokens[stream->tokenWritePointer] = token;
    stream->tokenCount++;
    stream->tokenWritePointer++;

    return token;
}

TM32ASM_Token* TM32ASM_InsertTokenAt (
    TM32ASM_TokenStream*    stream,
    TM32ASM_Token*          token,
    size_t                  index
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);
    TM32ASM_ReturnValueIfNull(token, NULL);
    TM32ASM_ReturnValueIf(index > stream->tokenCount, NULL);

    // Resize the token stream, in case it's necessary.
    if (!TM32ASM_ResizeTokenStream(stream))
    {
        return NULL;
    }

    // Calculate elements to move for better readability
    size_t elementsToMove = stream->tokenCount - index;

    // Insert the token into the token stream at the specified index,
    // pushing any existing tokens one place to the right.
    if (elementsToMove > 0)
    {
        memmove(&stream->tokens[index + 1],
                &stream->tokens[index],
                elementsToMove * sizeof(TM32ASM_Token*));
    }

    stream->tokens[index] = token;
    stream->tokenCount++;

    return token;
}

TM32ASM_Token* TM32ASM_PushTokenBack (
    TM32ASM_TokenStream*    stream,
    TM32ASM_Token*          token
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);
    TM32ASM_ReturnValueIfNull(token, NULL);

    // Resize the token stream, in case it's necessary.
    if (!TM32ASM_ResizeTokenStream(stream))
    {
        return NULL;
    }

    // Push the new token onto the back of the token stream.
    stream->tokens[stream->tokenCount++] = token;

    return token;
}

TM32ASM_Token* TM32ASM_EmplaceToken (
    TM32ASM_TokenStream*    stream,
    TM32ASM_TokenType       type,
    const char*             lexeme
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);

    // Check the position of the write pointer. If it's out of bounds, correct
    // it to the end of the token stream and do an emplace-back instead.
    if (stream->tokenWritePointer > stream->tokenCount)
    {
        stream->tokenWritePointer = stream->tokenCount;
        return TM32ASM_EmplaceTokenBack(stream, type, lexeme);
    }

    // Resize the token stream, in case it's necessary.
    if (!TM32ASM_ResizeTokenStream(stream))
    {
        return NULL;
    }

    // Create the new token using the safe helper function.
    TM32ASM_Token* token = TM32ASM_CreateTokenSafe(type, lexeme);
    if (token == NULL)
    {
        return NULL;
    }

    // Calculate elements to move for better readability
    size_t elementsToMove = stream->tokenCount - stream->tokenWritePointer;
    
    // Insert the new token into the token stream at the current write pointer,
    // pushing any existing tokens one place to the right.
    if (elementsToMove > 0)
    {
        memmove(&stream->tokens[stream->tokenWritePointer + 1],
                &stream->tokens[stream->tokenWritePointer],
                elementsToMove * sizeof(TM32ASM_Token*));
    }
    
    stream->tokens[stream->tokenWritePointer++] = token;
    stream->tokenCount++;

    return token;
}

TM32ASM_Token* TM32ASM_EmplaceTokenBack (
    TM32ASM_TokenStream*    stream,
    TM32ASM_TokenType       type,
    const char*             lexeme
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);

    // Fast path: check if we have capacity without function call
    if (stream->tokenCount >= stream->tokenCapacity)
    {
        if (!TM32ASM_ResizeTokenStream(stream))
        {
            return NULL;
        }
    }

    // Create the new token using the safe helper function.
    TM32ASM_Token* token = TM32ASM_CreateTokenSafe(type, lexeme);
    if (token == NULL)
    {
        return NULL;
    }

    // Direct assignment - most common operation, optimized path
    stream->tokens[stream->tokenCount++] = token;
    return token;
}

TM32ASM_Token* TM32ASM_ConsumeNextToken (
    TM32ASM_TokenStream* stream
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);

    // Check if the read pointer is out of bounds.
    if (stream->tokenReadPointer >= stream->tokenCount)
    {
        return NULL;
    }

    // Retrieve the next token from the stream.
    TM32ASM_Token* token = stream->tokens[stream->tokenReadPointer++];
    return token;
}

TM32ASM_Token* TM32ASM_PeekNextToken (
    TM32ASM_TokenStream*    stream,
    size_t                  offset
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);

    // Check if the read pointer plus the offset is out of bounds.
    if (stream->tokenReadPointer + offset >= stream->tokenCount)
    {
        return NULL;
    }

    // Retrieve the correct token from the stream without advancing the read pointer.
    TM32ASM_Token* token = stream->tokens[stream->tokenReadPointer + offset];
    return token;
}

TM32ASM_Token* TM32ASM_GetTokenAt (
    TM32ASM_TokenStream* stream,
    size_t               index
)
{
    TM32ASM_ReturnValueIfNull(stream, NULL);

    // Check if the index is out of bounds.
    if (index >= stream->tokenCount)
    {
        return NULL;
    }

    // Retrieve the token from the stream at the specified index.
    TM32ASM_Token* token = stream->tokens[index];
    return token;
}
