/**
 * @file    tm32asm_token.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 *
 * @brief   Contains the token structure and related definitions and prototypes.
 */

#ifndef TM32ASM_TOKEN_H
#define TM32ASM_TOKEN_H

/* Include Files **************************************************************/

#include <tm32asm_common.h>

/* Public Enumerations ********************************************************/

/**
 * @brief   Enumerates all possible types of tokens in the TM32 assembly
 *          language's syntax.
 */
typedef enum
{
    TM32ASM_TT_UNKNOWN = 0,             /** @brief Unknown token... */

    // Keywords and Identifiers
    TM32ASM_TT_DIRECTIVE,               /** @brief Directive (`.data`, `.if`, etc.) */
    TM32ASM_TT_FUNCTION,                /** @brief Built-In Assembler Function (`high()`, `int()`, `sin()`) */
    TM32ASM_TT_REGISTER,                /** @brief General-Purpose CPU Register (`a`, `bw`, `ch`, `dl`, etc.) */
    TM32ASM_TT_CONDITION,               /** @brief CPU Execution Condition (`nc`, `zs`, `cc`, etc.) */
    TM32ASM_TT_INSTRUCTION,             /** @brief CPU Instruction (`ld`, `mv`, `jmp`, etc.) */
    TM32ASM_TT_IDENTIFIER,              /** @brief User-Defined Identifier (`myVar`, `addressLabel`, etc.) */

    // Literals
    TM32ASM_TT_BINARY,                  /** @brief Binary Literal (`0b1010`, `0b1111`, etc.) */
    TM32ASM_TT_OCTAL,                   /** @brief Octal Literal (`0o12`, `0o17`, etc.) */
    TM32ASM_TT_DECIMAL,                 /** @brief Decimal Literal (`10`, `255`, etc.) */
    TM32ASM_TT_HEXADECIMAL,             /** @brief Hexadecimal Literal (`0xA`, `0xFF`, etc.) */
    TM32ASM_TT_FIXED_POINT,             /** @brief Fixed-Point Literal (`1.0`, `-0.5`, etc.) */
    TM32ASM_TT_CHARACTER,               /** @brief Character Literal (`'a'`, `'b'`, `'\n'`, `'\0'`, etc.) */
    TM32ASM_TT_PIXEL,                   /** @brief Pixel Literal */
    TM32ASM_TT_STRING,                  /** @brief String Literal (`"Hello, World!"`, `"Sample Text"`, etc.) */

    // Arithmetic and Bitwise Operators
    TM32ASM_TT_PLUS,                    /** @brief Addition Operator (`+`) */
    TM32ASM_TT_MINUS,                   /** @brief Subtraction Operator (`-`) */
    TM32ASM_TT_TIMES,                   /** @brief Multiplication Operator (`*`) */
    TM32ASM_TT_EXPONENT,                /** @brief Exponentiation Operator (`**`) */
    TM32ASM_TT_DIVIDE,                  /** @brief Division Operator (`/`) */
    TM32ASM_TT_MODULO,                  /** @brief Modulo Operator (`%`) */
    TM32ASM_TT_BITWISE_AND,             /** @brief Bitwise AND Operator (`&`) */
    TM32ASM_TT_BITWISE_OR,              /** @brief Bitwise OR Operator (`|`) */
    TM32ASM_TT_BITWISE_XOR,             /** @brief Bitwise XOR Operator (`^`) */
    TM32ASM_TT_BITWISE_NOT,             /** @brief Bitwise NOT Operator (`~`) */
    TM32ASM_TT_LEFT_SHIFT,              /** @brief Left Shift Operator (`<<`) */
    TM32ASM_TT_RIGHT_SHIFT,             /** @brief Right Shift Operator (`>>`) */

    // Assignment Operators
    TM32ASM_TT_ASSIGN_EQUAL,            /** @brief Assignment Operator (`=`) */
    TM32ASM_TT_ASSIGN_PLUS,             /** @brief Assignment Operator (`+=`) */
    TM32ASM_TT_ASSIGN_MINUS,            /** @brief Assignment Operator (`-=`) */
    TM32ASM_TT_ASSIGN_TIMES,            /** @brief Assignment Operator (`*=`) */
    TM32ASM_TT_ASSIGN_EXPONENT,         /** @brief Assignment Operator (`**=`) */
    TM32ASM_TT_ASSIGN_DIVIDE,           /** @brief Assignment Operator (`/=`) */
    TM32ASM_TT_ASSIGN_MODULO,           /** @brief Assignment Operator (`%=`) */
    TM32ASM_TT_ASSIGN_BITWISE_AND,      /** @brief Assignment Operator (`&=`) */
    TM32ASM_TT_ASSIGN_BITWISE_OR,       /** @brief Assignment Operator (`|=`) */
    TM32ASM_TT_ASSIGN_BITWISE_XOR,      /** @brief Assignment Operator (`^=`) */
    TM32ASM_TT_ASSIGN_LEFT_SHIFT,       /** @brief Assignment Operator (`<<=`) */
    TM32ASM_TT_ASSIGN_RIGHT_SHIFT,      /** @brief Assignment Operator (`>>=`) */

    // Comparison Operators
    TM32ASM_TT_COMPARE_EQUAL,           /** @brief Comparison Operator (`==`) */
    TM32ASM_TT_COMPARE_NOT_EQUAL,       /** @brief Comparison Operator (`!=`) */
    TM32ASM_TT_COMPARE_LESS,            /** @brief Comparison Operator (`<`) */
    TM32ASM_TT_COMPARE_LESS_EQUAL,      /** @brief Comparison Operator (`<=`) */
    TM32ASM_TT_COMPARE_GREATER,         /** @brief Comparison Operator (`>`) */
    TM32ASM_TT_COMPARE_GREATER_EQUAL,   /** @brief Comparison Operator (`>=`) */

    // Logical Operators
    TM32ASM_TT_LOGICAL_AND,             /** @brief Logical AND Operator (`&&`) */
    TM32ASM_TT_LOGICAL_OR,              /** @brief Logical OR Operator (`||`) */
    TM32ASM_TT_LOGICAL_NOT,             /** @brief Logical NOT Operator (`!`) */

    // Grouping Operators
    TM32ASM_TT_OPEN_PARENTHESIS,        /** @brief Grouping Operator (`(`) */
    TM32ASM_TT_CLOSE_PARENTHESIS,       /** @brief Grouping Operator (`)`) */
    TM32ASM_TT_OPEN_BRACKET,            /** @brief Grouping Operator (`[`) */
    TM32ASM_TT_CLOSE_BRACKET,           /** @brief Grouping Operator (`]`) */
    TM32ASM_TT_OPEN_BRACE,              /** @brief Grouping Operator (`{`) */
    TM32ASM_TT_CLOSE_BRACE,             /** @brief Grouping Operator (`}`) */

    // Punctuation Tokens
    TM32ASM_TT_COMMA,                   /** @brief Punctuation Token (`,`) */
    TM32ASM_TT_COLON,                   /** @brief Punctuation Token (`:`) */
    
} TM32ASM_TokenType;

/* Public Structures **********************************************************/

/**
 * @brief   Represents a token in the TM32 assembly language's syntax.
 */
typedef struct
{
    char*               lexeme;     /** @brief The actual text of the token. */
    TM32ASM_TokenType   type;       /** @brief The type of the token. */
    int32_t             param;      /** @brief An additional parameter further describing the token. */

    const char*         filename;   /** @brief The name of the source file from which the token was extracted. */
    uint32_t            line;       /** @brief The line number in the source file where the token was found. */
} TM32ASM_Token;

#endif // TM32ASM_TOKEN_H
