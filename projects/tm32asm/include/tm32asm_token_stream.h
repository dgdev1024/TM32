/**
 * @file    tm32asm_token_stream.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 *
 * @brief   Contains the token stream structure and related definitions and 
 *          prototypes.
 */

#ifndef TM32ASM_TOKEN_STREAM_H
#define TM32ASM_TOKEN_STREAM_H

/* Include Files **************************************************************/

#include <tm32asm_token.h>

/* Public Constants ***********************************************************/

#define TM32ASM_TOKEN_STREAM_INITIAL_CAPACITY   16
#define TM32ASM_TOKEN_STREAM_GROWTH_FACTOR       1.5f
#define TM32ASM_TOKEN_STREAM_MAX_CAPACITY        (SIZE_MAX / sizeof(TM32ASM_Token*))

/* Public Structures **********************************************************/

/**
 * @brief   A structure which contains an array of TM32 ASM syntax tokens, which
 *          can be extracted by the lexer and mutated by the preprocessor.
 */
typedef struct
{
    TM32ASM_Token**     tokens;             /** @brief The array of heap-allocated tokens. */
    size_t              tokenCount;         /** @brief The number of tokens in the array. */
    size_t              tokenCapacity;      /** @brief The current capacity of the token array. */
    size_t              tokenReadPointer;   /** @brief The current read position in the token array. */
    size_t              tokenWritePointer;  /** @brief The current write position in the token array. */
} TM32ASM_TokenStream;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new token stream, and its underlying tokens
 *          array.
 * 
 * @return  A pointer to the newly created token stream if successful, or
 *          `NULL` if an error occurred.
 */
TM32ASM_TokenStream* TM32ASM_CreateTokenStream ();

/**
 * @brief   Destroys a token stream and frees all associated memory.
 * 
 * @param   stream  A pointer to the token stream to destroy.
 */
void TM32ASM_DestroyTokenStream (
    TM32ASM_TokenStream* stream
);

/**
 * @brief   Creates a copy of an existing token stream.
 * 
 * @param   stream  A pointer to the token stream to copy.
 * 
 * @return  A pointer to the newly created copy if successful, or `NULL` if
 *          an error occurred.
 */
TM32ASM_TokenStream* TM32ASM_CopyTokenStream (
    const TM32ASM_TokenStream* stream
);

/**
 * @brief   Inserts a token into the token stream at the current write pointer.
 * 
 * @param   stream  A pointer to the token stream in which to insert the token.
 * @param   token   A pointer to the token to insert.
 * 
 * @return  A pointer to the inserted token if successful, or `NULL` if an error
 *          occurs.
 * 
 * @warning The token, once inserted, will be owned by the token stream and
 *          should not be freed by the caller.
 */
TM32ASM_Token* TM32ASM_InsertToken (
    TM32ASM_TokenStream*    stream,
    TM32ASM_Token*          token
);

/**
 * @brief   Inserts a token into the token stream at a specified index.
 * 
 * @param   stream  A pointer to the token stream in which to insert the token.
 * @param   token   A pointer to the token to insert.
 * @param   index   The index at which to insert the token.
 * 
 * @return  A pointer to the inserted token if successful, or `NULL` if an error
 *          occurs.
 * 
 * @warning The token, once inserted, will be owned by the token stream and
 *          should not be freed by the caller.
 */
TM32ASM_Token* TM32ASM_InsertTokenAt (
    TM32ASM_TokenStream*    stream,
    TM32ASM_Token*          token,
    size_t                  index
);

/**
 * @brief   Pushes a token onto the back of the token stream.
 * 
 * @param   stream  A pointer to the token stream in which to push the token.
 * @param   token   A pointer to the token to push.
 * 
 * @return  A pointer to the pushed token if successful, or `NULL` if an error
 *          occurs.
 * 
 * @warning The token, once inserted, will be owned by the token stream and
 *          should not be freed by the caller.
 */
TM32ASM_Token* TM32ASM_PushTokenBack (
    TM32ASM_TokenStream*    stream,
    TM32ASM_Token*          token
);

/**
 * @brief   Creates a new TM32 ASM syntax token with the given type and lexeme,
 *          then emplaces it into the token stream at the current write pointer.
 * 
 * @param   stream  A pointer to the token stream in which to emplace the new
 *                  token.
 * @param   type    The type of the new token.
 * @param   lexeme  The lexeme (text) of the new token.
 * 
 * @return  A pointer to the newly created token if successful, or `NULL` if an
 *          error occurs.
 */
TM32ASM_Token* TM32ASM_EmplaceToken (
    TM32ASM_TokenStream*    stream,
    TM32ASM_TokenType       type,
    const char*             lexeme
);

/**
 * @brief   Creates a new TM32 ASM syntax token with the given type and lexeme,
 *          then emplaces it at the back of the token stream.
 * 
 * @param   stream  A pointer to the token stream in which to emplace the new
 *                  token.
 * @param   type    The type of the new token.
 * @param   lexeme  The lexeme (text) of the new token.
 * 
 * @return  A pointer to the newly created token if successful, or `NULL` if an
 *          error occurs.
 */
TM32ASM_Token* TM32ASM_EmplaceTokenBack (
    TM32ASM_TokenStream*    stream,
    TM32ASM_TokenType       type,
    const char*             lexeme
);

/**
 * @brief   Reserves capacity for at least the specified number of tokens to
 *          optimize bulk operations.
 * 
 * @param   stream      A pointer to the token stream to reserve capacity for.
 * @param   capacity    The minimum capacity to reserve.
 * 
 * @return  `true` if the capacity was reserved successfully; `false` otherwise.
 */
bool TM32ASM_ReserveTokens (
    TM32ASM_TokenStream*    stream,
    size_t                  capacity
);

/**
 * @brief   Retrieves a TM32 ASM syntax token from the token stream at the
 *          current read pointer, then advances the read pointer if successful.
 * 
 * @param   stream  A pointer to the token stream from which to retrieve the
 *                  next token.
 * 
 * @return  A pointer to the next token if successful, or `NULL` if an error
 *          occurs, or if the read pointer is out of bounds.
 */
TM32ASM_Token* TM32ASM_ConsumeNextToken (
    TM32ASM_TokenStream* stream
);

/**
 * @brief   Retrieves a TM32 ASM syntax token from the token stream at the
 *          current read pointer plus a given offset, without advancing the
 *          read pointer.
 * 
 * @param   stream  A pointer to the token stream from which to retrieve the
 *                  next token.
 * @param   offset  The offset from the current read pointer.
 * 
 * @return  A pointer to the next token if successful, or `NULL` if an error
 *          occurs, or if the read pointer plus the offset is out of bounds.
 */
TM32ASM_Token* TM32ASM_PeekNextToken (
    TM32ASM_TokenStream*    stream,
    size_t                  offset
);

/**
 * @brief   Retrieves a TM32 ASM syntax token from the token stream at the
 *          specified index.
 * 
 * @param   stream  A pointer to the token stream from which to retrieve the
 *                  token.
 * @param   index   The index of the token to retrieve.
 * 
 * @return  A pointer to the token at the specified index if successful, or
 *          `NULL` if an error occurs, or if the index is out of bounds.
 */
TM32ASM_Token* TM32ASM_GetTokenAt (
    TM32ASM_TokenStream* stream,
    size_t               index
);

#endif // TM32ASM_TOKEN_STREAM_H
