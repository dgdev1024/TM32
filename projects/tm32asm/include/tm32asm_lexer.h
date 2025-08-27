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

#include <tm32asm_token.h>

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
    char*       source;         /** @brief The source code being tokenized. */
    size_t      sourceLength;   /** @brief The length of the source code. */
    size_t      currentPos;     /** @brief Current position in the source code. */
    uint32_t    currentLine;    /** @brief Current line number in the source code. */
    const char* filename;       /** @brief Name of the source file being processed. */
    
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
 * @brief   Loads source code from a file into the lexer for tokenization.
 * 
 * @param   lexer       A pointer to the TM32ASM lexer.
 * @param   filename    The path to the source file to load.
 * 
 * @return  `true` if the file was loaded successfully; `false` otherwise.
 */
bool TM32ASM_LoadSourceFile (
    TM32ASM_Lexer*  lexer,
    const char*     filename
);

/**
 * @brief   Loads source code from a string into the lexer for tokenization.
 * 
 * @param   lexer       A pointer to the TM32ASM lexer.
 * @param   source      The source code string to load.
 * @param   filename    The filename to associate with the source (for error messages).
 * 
 * @return  `true` if the source was loaded successfully; `false` otherwise.
 */
bool TM32ASM_LoadSourceString (
    TM32ASM_Lexer*  lexer,
    const char*     source,
    const char*     filename
);

/**
 * @brief   Extracts the next token from the loaded source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  A pointer to the next token if successful; `NULL` if there are no
 *          more tokens or if an error occurred.
 */
TM32ASM_Token* TM32ASM_NextToken (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Resets the lexer to the beginning of the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 */
void TM32ASM_ResetLexer (
    TM32ASM_Lexer* lexer
);

/**
 * @brief   Checks if there are more tokens to be extracted from the source code.
 * 
 * @param   lexer   A pointer to the TM32ASM lexer.
 * 
 * @return  `true` if there are more tokens; `false` otherwise.
 */
bool TM32ASM_HasMoreTokens (
    TM32ASM_Lexer* lexer
);

#endif // TM32ASM_LEXER_H
