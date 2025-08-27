/**
 * @file    tm32asm_lexer.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the lexer structure and related definitions and prototypes.
 */

#ifndef TM32ASM_LEXER_H
#define TM32ASM_LEXER_H

/* Include Files **************************************************************/

#include <tm32asm_token_stream.h>

/* Public Structures **********************************************************/

/**
 * @brief   Represents the state and context of the TM32 Assembler Tool's
 *          lexer as it extracts tokens from source files.
 * 
 * The lexical analyzer (henceforth, "the lexer") is the first step of the
 * TM32 assembly process. It is responsible for loading source code from files
 * and extracting tokens from that code.
 */
typedef struct
{
    char*               sourceCode;         /** @brief The loaded source code. */
    size_t              sourceLength;      /** @brief The length of the source code. */
    size_t              currentPosition;   /** @brief Current position in the source code. */
    uint32_t            currentLine;       /** @brief Current line number (1-based). */
    uint32_t            currentColumn;     /** @brief Current column number (1-based). */
    const char*         filename;          /** @brief Name of the current source file. */
    TM32ASM_TokenStream* tokenStream;      /** @brief Output token stream. */
} TM32ASM_Lexer;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Assembly Tool's
 *          lexer (henceforth, "the TM32ASM lexer").
 * 
 * @return  A pointer to the newly-created lexer instance if successful;
 *          `NULL` otherwise.
 */
TM32ASM_Lexer* TM32ASM_CreateLexer ();

/**
 * @brief   Destroys and deallocates the given TM32ASM lexer.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer to destroy.
 */
void TM32ASM_DestroyLexer (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Loads source code from a file into the lexer.
 * 
 * @param   lexer    A pointer to the TM32ASM lexer.
 * @param   filename The name of the source file to load.
 * 
 * @return  `true` if the file was loaded successfully; `false` otherwise.
 */
bool TM32ASM_LoadSourceFile (
    TM32ASM_Lexer*  lexer,
    const char*     filename
);

/**
 * @brief   Tokenizes the loaded source code in the lexer.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer containing loaded source code.
 * 
 * @return  `true` if tokenization was successful; `false` otherwise.
 */
bool TM32ASM_TokenizeSource (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Gets the token stream produced by the lexer.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the token stream, or `NULL` if no tokenization
 *          has been performed.
 */
TM32ASM_TokenStream* TM32ASM_GetTokenStream (
    const TM32ASM_Lexer* lexer
);

#endif // TM32ASM_LEXER_H
