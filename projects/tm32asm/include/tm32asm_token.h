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

#include <tm32cpu_processor.h>
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

/**
 * @brief   Enumerates all types of preprocessor and assembler directives.
 */
typedef enum
{
    
    // Preprocessor Directives
    TM32ASM_DT_INCLUDE,             /** @brief Include Directive (`.include`) */
    TM32ASM_DT_LET,                 /** @brief Mutable Variable Declaration (`.let`) */
    TM32ASM_DT_CONST,               /** @brief Immutable Constant Declaration (`.const`) */
    TM32ASM_DT_DEFINE,              /** @brief Simple Text Substitution Macro (`.define`, `.def`) */
    TM32ASM_DT_MACRO,               /** @brief Macro Definition (`.macro`) */
    TM32ASM_DT_SHIFT,               /** @brief Shift Directive (`.shift`) */
    TM32ASM_DT_ENDMACRO,            /** @brief End Macro Directive (`.endmacro`, `.endm`) */
    TM32ASM_DT_IF,                  /** @brief Conditional Directive (`.if`) */
    TM32ASM_DT_ELSE,                /** @brief Else Directive (`.else`) */
    TM32ASM_DT_ELSEIF,              /** @brief Else If Directive (`.elseif`, `.elif`) */
    TM32ASM_DT_ENDIF,               /** @brief End If Directive (`.endif`, `.endc`) */
    TM32ASM_DT_REPEAT,              /** @brief Repeat Directive (`.repeat`, `.rept`) */
    TM32ASM_DT_ENDREPEAT,           /** @brief End Repeat Directive (`.endrepeat`, `.endr`) */
    TM32ASM_DT_WHILE,               /** @brief While Directive (`.while`) */
    TM32ASM_DT_ENDWHILE,            /** @brief End While Directive (`.endwhile`, `.endw`) */
    TM32ASM_DT_FOR,                 /** @brief For Directive (`.for`) */
    TM32ASM_DT_ENDFOR,              /** @brief End For Directive (`.endfor`, `.endf`) */
    TM32ASM_DT_CONTINUE,            /** @brief Continue Directive (`.continue`) */
    TM32ASM_DT_BREAK,               /** @brief Break Directive (`.break`) */
    TM32ASM_DT_ERROR,               /** @brief Error Directive (`.error`) */
    TM32ASM_DT_ASSERT,              /** @brief Assert Directive (`.assert`) */
    TM32ASM_DT_FILE,                /** @brief File Directive (`.file`) */
    TM32ASM_DT_LINE,                /** @brief Line Directive (`.line`) */

    // Assembler Directives
    TM32ASM_DT_METADATA,            /** @brief Metadata Section Directive (`.metadata`) */
    TM32ASM_DT_INTERRUPT,           /** @brief Interrupt Section Directive (`.interrupt XX`, `.int XX`) */
    TM32ASM_DT_TEXT,                /** @brief Code Section Directive (`.text`, `.code`) */
    TM32ASM_DT_RODATA,              /** @brief Read-Only Data Section Directive (`.rodata`) */
    TM32ASM_DT_DATA,                /** @brief Data Section Directive (`.data`) */
    TM32ASM_DT_BSS,                 /** @brief BSS Section Directive (`.bss`) */
    TM32ASM_DT_ORG,                 /** @brief Origin Directive (`.org <address>`) */
    TM32ASM_DT_DB,                  /** @brief Define Byte Directive (`.db <value1>, <value2>, ...`, `.byte <value1>, <value2>, ...`) */
    TM32ASM_DT_DW,                  /** @brief Define Word Directive (`.dw <value1>, <value2>, ...`, `.word <value1>, <value2>, ...`) */
    TM32ASM_DT_DD,                  /** @brief Define Double Word Directive (`.dd <value1>, <value2>, ...`, `.dword <value1>, <value2>, ...`) */
    TM32ASM_DT_ASCIZ,               /** @brief Define ASCII Zero-Terminated String Directive (`.asciz "string"`) */
    TM32ASM_DT_ALIGN,               /** @brief Alignment Directive (`.align <boundary>`) */
    TM32ASM_DT_DS,                  /** @brief Reserve Space Directive (`.ds <size>`, `.space <size>`) */
    TM32ASM_DT_INCBIN,              /** @brief Include Binary File Directive (`.incbin "filename", <offset>, <length>`) */

} TM32ASM_DirectiveType;

/**
 * @brief   Enumerates all built-in functions.
 */
typedef enum
{

    // Integer Functions
    TM32ASM_FT_HIGH,
    TM32ASM_FT_LOW,
    TM32ASM_FT_BITWIDTH,

    // Fixed-Point Functions
    TM32ASM_FT_INTEGER,
    TM32ASM_FT_FRACTION,
    TM32ASM_FT_ROUND,
    TM32ASM_FT_CEIL,
    TM32ASM_FT_FLOOR,

    // Trigonometric Functions
    TM32ASM_FT_SIN,
    TM32ASM_FT_COS,
    TM32ASM_FT_TAN,
    TM32ASM_FT_ASIN,
    TM32ASM_FT_ACOS,
    TM32ASM_FT_ATAN,

    // String Functions
    TM32ASM_FT_STRLEN,
    TM32ASM_FT_STRUPPER,
    TM32ASM_FT_STRLOWER,
    TM32ASM_FT_STRCMP,
    TM32ASM_FT_STRFIND,
    TM32ASM_FT_STRRFIND,
    TM32ASM_FT_STRBYTE,
    TM32ASM_FT_DEFINED,

    // Meta Functions
    TM32ASM_FT_ISCONST,
    TM32ASM_FT_ISVARIABLE,
    TM32ASM_FT_SECTION,
    TM32ASM_FT_ORIGIN,
    TM32ASM_FT_SIZEOF,
    TM32ASM_FT_STARTOF,

} TM32ASM_FunctionType;

/* Public Structures **********************************************************/

/**
 * @brief   Represents a token in the TM32 assembly language's syntax.
 */
typedef struct
{
    char*               lexeme;     /** @brief The actual text of the token. */
    TM32ASM_TokenType   type;       /** @brief The type of the token. */
    int32_t             param;      /** @brief For keyword tokens, an additional parameter further describing the token. */

    const char*         filename;   /** @brief The name of the source file from which the token was extracted. */
    uint32_t            line;       /** @brief The line number in the source file where the token was found. */
} TM32ASM_Token;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Creates a new syntax token.
 * 
 * @param   lexeme The actual text of the token.
 * @param   type   The type of the token.
 * 
 * @return  A pointer to the newly created token if successful, or `NULL` if
 *          memory allocation fails.
 */
TM32ASM_Token* TM32ASM_CreateToken (
    const char*         lexeme,
    TM32ASM_TokenType   type
);

/**
 * @brief   Destroys a syntax token.
 * 
 * @param   token  A pointer to the token to destroy.
 */
void TM32ASM_DestroyToken (
    TM32ASM_Token* token
);

/**
 * @brief   Checks to see if the lexeme in the given token indicates a keyword,
 *          and if so, retrieves the corresponding keyword token.
 * 
 * Keywords in the TM32 assembly language are case-insensitive.
 * 
 * @param   token   A pointer to the token to check.
 * @param   out     If not `NULL`, will point to the keyword token if found.
 * 
 * @return  `true` if the token is a keyword; `false` otherwise.
 */
bool TM32ASM_GetKeyword (
    const TM32ASM_Token*     token,
    TM32ASM_Token**          out
);

/**
 * @brief   For tokens with a numeric literal type, converts the lexeme string
 *          into an integer value.
 * 
 * @param   token  A pointer to the token whose lexeme is to be converted.
 * 
 * @return  The integer value represented by the token's lexeme, or `0` if the
 *          token is not a valid numeric literal.
 */
uint32_t TM32ASM_GetIntegerLexeme (
    const TM32ASM_Token* token
);

/**
 * @brief   For tokens with a numeric literal type, converts the lexeme string
 *          into a floating-point value.
 * 
 * @param   token  A pointer to the token whose lexeme is to be converted.
 * 
 * @return  The floating-point value represented by the token's lexeme, or `0.0` if the
 *          token is not a valid numeric literal.
 */
double TM32ASM_GetNumberLexeme (
    const TM32ASM_Token* token
);

/**
 * @brief   For tokens with a numeric literal type, converts the lexeme string
 *          into a fixed-point value, then gets its fixed-point integer
 *          representation.
 * 
 * In the TM32 assembly language, fixed-point numbers are internally represented
 * as 64-bit integers, where the upper 32 bits represent the integer part and the
 * lower 32 bits represent the fractional part.
 * 
 * @param   token  A pointer to the token whose lexeme is to be converted.
 * 
 * @return  The fixed-point integer representation of the token's lexeme, or `0` if the
 *          token is not a valid numeric literal.
 */
uint64_t TM32ASM_GetFixedPointLexeme (
    const TM32ASM_Token* token
);

#endif // TM32ASM_TOKEN_H
