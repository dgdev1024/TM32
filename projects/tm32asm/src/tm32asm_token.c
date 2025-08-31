/**
 * @file    tm32asm_token.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/27
 *
 * @brief   Contains the implementation of the token structure's functions.
 */

/* Include Files **************************************************************/

#include <tm32asm_token.h>

/* Private Constants **********************************************************/

static const TM32ASM_Token KEYWORD_LOOKUP[] = {

    // Preprocessor Directives
    { ".include",       TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INCLUDE,     NULL, 0 },
    { ".let",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_LET,         NULL, 0 },
    { ".const",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_CONST,       NULL, 0 },
    { ".define",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DEFINE,      NULL, 0 },
    { ".def",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DEFINE,      NULL, 0 },
    { ".macro",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_MACRO,       NULL, 0 },
    { ".shift",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_SHIFT,       NULL, 0 },
    { ".endmacro",      TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDMACRO,    NULL, 0 },
    { ".endm",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDMACRO,    NULL, 0 },
    { ".if",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_IF,          NULL, 0 },
    { ".else",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ELSE,        NULL, 0 },
    { ".elseif",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ELSEIF,      NULL, 0 },
    { ".elif",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ELSEIF,      NULL, 0 },
    { ".endif",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDIF,       NULL, 0 },
    { ".endc",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDIF,       NULL, 0 },
    { ".repeat",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_REPEAT,      NULL, 0 },
    { ".rept",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_REPEAT,      NULL, 0 },
    { ".endrepeat",     TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDREPEAT,   NULL, 0 },
    { ".endr",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDREPEAT,   NULL, 0 },
    { ".while",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_WHILE,       NULL, 0 },
    { ".endwhile",      TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDWHILE,    NULL, 0 },
    { ".endw",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDWHILE,    NULL, 0 },
    { ".for",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_FOR,         NULL, 0 },
    { ".end",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_END,         NULL, 0 },
    { ".endfor",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDFOR,      NULL, 0 },
    { ".endf",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDFOR,      NULL, 0 },
    { ".continue",      TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_CONTINUE,    NULL, 0 },
    { ".break",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_BREAK,       NULL, 0 },
    { ".warn",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_WARN,        NULL, 0 },
    { ".error",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ERROR,       NULL, 0 },
    { ".assert",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ASSERT,      NULL, 0 },
    { ".file",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_FILE,        NULL, 0 },
    { ".line",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_LINE,        NULL, 0 },
    
    // Assembler Directives
    { ".metadata",      TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_METADATA,    NULL, 0 },
    { ".interrupt",     TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INTERRUPT,   NULL, 0 },
    { ".int",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INTERRUPT,   NULL, 0 },
    { ".text",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_TEXT,        NULL, 0 },
    { ".code",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_TEXT,        NULL, 0 },
    { ".rodata",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_RODATA,      NULL, 0 },
    { ".data",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DATA,        NULL, 0 },
    { ".bss",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_BSS,         NULL, 0 },
    { ".org",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ORG,         NULL, 0 },
    { ".db",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DB,          NULL, 0 },
    { ".byte",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DB,          NULL, 0 },
    { ".dw",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DW,          NULL, 0 },
    { ".word",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DW,          NULL, 0 },
    { ".dd",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DD,          NULL, 0 },
    { ".dword",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DD,          NULL, 0 },
    { ".asciz",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ASCIZ,       NULL, 0 },
    { ".stringz",       TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ASCIZ,       NULL, 0 },
    { ".align",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ALIGN,       NULL, 0 },
    { ".ds",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DS,          NULL, 0 },
    { ".space",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DS,          NULL, 0 },
    { ".incbin",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INCBIN,      NULL, 0 },

    // Built-In Functions
    { "high",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_HIGH,        NULL, 0 },
    { "low",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_LOW,         NULL, 0 },
    { "bitwidth",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_BITWIDTH,    NULL, 0 },
    { "integer",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_INTEGER,     NULL, 0 },
    { "fraction",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_FRACTION,    NULL, 0 },
    { "round",          TM32ASM_TT_FUNCTION,        TM32ASM_FT_ROUND,       NULL, 0 },
    { "ceil",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_CEIL,        NULL, 0 },
    { "floor",          TM32ASM_TT_FUNCTION,        TM32ASM_FT_FLOOR,       NULL, 0 },
    { "sin",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_SIN,         NULL, 0 },
    { "cos",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_COS,         NULL, 0 },
    { "tan",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_TAN,         NULL, 0 },
    { "asin",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_ASIN,        NULL, 0 },
    { "acos",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_ACOS,        NULL, 0 },
    { "atan",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_ATAN,        NULL, 0 },
    { "strlen",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRLEN,      NULL, 0 },
    { "strupper",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRUPPER,    NULL, 0 },
    { "strlower",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRLOWER,    NULL, 0 },
    { "strcmp",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRCMP,      NULL, 0 },
    { "strfind",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRFIND,     NULL, 0 },
    { "strrfind",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRRFIND,    NULL, 0 },
    { "strbyte",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRBYTE,     NULL, 0 },
    { "defined",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_DEFINED,     NULL, 0 },
    { "isconst",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_ISCONST,     NULL, 0 },
    { "isvariable",     TM32ASM_TT_FUNCTION,        TM32ASM_FT_ISVARIABLE,  NULL, 0 },
    { "section",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_SECTION,     NULL, 0 },
    { "origin",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_ORIGIN,      NULL, 0 },
    { "sizeof",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_SIZEOF,      NULL, 0 },
    { "startof",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_STARTOF,     NULL, 0 },

    // General-Purpose CPU Registers
    { "a",              TM32ASM_TT_REGISTER,        TM32CPU_RT_A,           NULL, 0 },
    { "aw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_AW,          NULL, 0 },
    { "ah",             TM32ASM_TT_REGISTER,        TM32CPU_RT_AH,          NULL, 0 },
    { "al",             TM32ASM_TT_REGISTER,        TM32CPU_RT_AL,          NULL, 0 },
    { "b",              TM32ASM_TT_REGISTER,        TM32CPU_RT_B,           NULL, 0 },
    { "bw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_BW,          NULL, 0 },
    { "bh",             TM32ASM_TT_REGISTER,        TM32CPU_RT_BH,          NULL, 0 },
    { "bl",             TM32ASM_TT_REGISTER,        TM32CPU_RT_BL,          NULL, 0 },
    { "c",              TM32ASM_TT_REGISTER,        TM32CPU_RT_C,           NULL, 0 },
    { "cw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_CW,          NULL, 0 },
    { "ch",             TM32ASM_TT_REGISTER,        TM32CPU_RT_CH,          NULL, 0 },
    { "cl",             TM32ASM_TT_REGISTER,        TM32CPU_RT_CL,          NULL, 0 },
    { "d",              TM32ASM_TT_REGISTER,        TM32CPU_RT_D,           NULL, 0 },
    { "dw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_DW,          NULL, 0 },
    { "dh",             TM32ASM_TT_REGISTER,        TM32CPU_RT_DH,          NULL, 0 },
    { "dl",             TM32ASM_TT_REGISTER,        TM32CPU_RT_DL,          NULL, 0 },

    // Execution Conditions
    { "nc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_NC,          NULL, 0 },
    { "zc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_ZC,          NULL, 0 },
    { "zs",             TM32ASM_TT_CONDITION,       TM32CPU_CT_ZS,          NULL, 0 },
    { "cc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_CC,          NULL, 0 },
    { "cs",             TM32ASM_TT_CONDITION,       TM32CPU_CT_CS,          NULL, 0 },
    { "pc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_PC,          NULL, 0 },
    { "ps",             TM32ASM_TT_CONDITION,       TM32CPU_CT_PS,          NULL, 0 },
    { "sc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_SC,          NULL, 0 },
    { "ss",             TM32ASM_TT_CONDITION,       TM32CPU_CT_SS,          NULL, 0 },

    // CPU Instructions
    { "nop",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_NOP,         NULL, 0 },
    { "stop",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STOP,        NULL, 0 },
    { "halt",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_HALT,        NULL, 0 },
    { "sec",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SEC,         NULL, 0 },
    { "cec",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CEC,         NULL, 0 },
    { "di",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_DI,          NULL, 0 },
    { "ei",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_EI,          NULL, 0 },
    { "daa",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_DAA,         NULL, 0 },
    { "scf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SCF,         NULL, 0 },
    { "ccf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CCF,         NULL, 0 },
    { "flg",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_FLG,         NULL, 0 },
    { "stf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STF,         NULL, 0 },
    { "clf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CLF,         NULL, 0 },
    { "ld",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_LD,          NULL, 0 },
    { "ldh",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_LDH,         NULL, 0 },
    { "ldp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_LDP,         NULL, 0 },
    { "st",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_ST,          NULL, 0 },
    { "sth",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STH,         NULL, 0 },
    { "stp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STP,         NULL, 0 },
    { "mv",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_MV,          NULL, 0 },
    { "pop",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_POP,         NULL, 0 },
    { "push",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_PUSH,        NULL, 0 },
    { "jmp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_JMP,         NULL, 0 },
    { "jpb",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_JPB,         NULL, 0 },
    { "call",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CALL,        NULL, 0 },
    { "int",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_INT,         NULL, 0 },
    { "ret",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RET,         NULL, 0 },
    { "reti",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RETI,        NULL, 0 },
    { "inc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_INC,         NULL, 0 },
    { "dec",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_DEC,         NULL, 0 },
    { "add",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_ADD,         NULL, 0 },
    { "adc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_ADC,         NULL, 0 },
    { "sub",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SUB,         NULL, 0 },
    { "sbc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SBC,         NULL, 0 },
    { "and",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_AND,         NULL, 0 },
    { "or",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_OR,          NULL, 0 },
    { "xor",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_XOR,         NULL, 0 },
    { "not",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_NOT,         NULL, 0 },
    { "cmp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CMP,         NULL, 0 },
    { "sla",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SLA,         NULL, 0 },
    { "sra",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SRA,         NULL, 0 },
    { "srl",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SRL,         NULL, 0 },
    { "rl",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RL,          NULL, 0 },
    { "rlc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RLC,         NULL, 0 },
    { "rr",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RR,          NULL, 0 },
    { "rrc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RRC,         NULL, 0 },
    { "bit",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_BIT,         NULL, 0 },
    { "tog",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_TOG,         NULL, 0 },
    { "set",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SET,         NULL, 0 },
    { "res",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RES,         NULL, 0 },

    // Other Miscellaneous Keywords
    { "to",             TM32ASM_TT_KEYWORD,         TM32ASM_MKT_TO,         NULL, 0 },
    { "step",           TM32ASM_TT_KEYWORD,         TM32ASM_MKT_STEP,       NULL, 0 },

    { NULL,             TM32ASM_TT_UNKNOWN,         0,                      NULL, 0 }
};

/* Private Functions **********************************************************/

/**
 * @brief   Checks whether a token's lexeme can be predicted from its type and param.
 * 
 * @param   token   Pointer to the token to check.
 * @return  bool    Returns `true` if the lexeme can be predicted, otherwise `false`.
 */
static bool TM32ASM_IsTokenLexemePredictable(const TM32ASM_Token* token)
{
    if (!token) {
        return false;
    }

    // Check for arithmetic operators
    if (token->type == TM32ASM_TT_PLUS ||
        token->type == TM32ASM_TT_MINUS ||
        token->type == TM32ASM_TT_TIMES ||
        token->type == TM32ASM_TT_EXPONENT ||
        token->type == TM32ASM_TT_DIVIDE ||
        token->type == TM32ASM_TT_MODULO) {
        return true;
    }
    
    // Check for bitwise operators
    if (token->type == TM32ASM_TT_BITWISE_AND ||
        token->type == TM32ASM_TT_BITWISE_OR ||
        token->type == TM32ASM_TT_BITWISE_XOR ||
        token->type == TM32ASM_TT_BITWISE_NOT ||
        token->type == TM32ASM_TT_LEFT_SHIFT ||
        token->type == TM32ASM_TT_RIGHT_SHIFT) {
        return true;
    }
    
    // Check for assignment operators
    if (token->type == TM32ASM_TT_ASSIGN_EQUAL ||
        token->type == TM32ASM_TT_ASSIGN_PLUS ||
        token->type == TM32ASM_TT_ASSIGN_MINUS ||
        token->type == TM32ASM_TT_ASSIGN_TIMES ||
        token->type == TM32ASM_TT_ASSIGN_EXPONENT ||
        token->type == TM32ASM_TT_ASSIGN_DIVIDE ||
        token->type == TM32ASM_TT_ASSIGN_MODULO ||
        token->type == TM32ASM_TT_ASSIGN_BITWISE_AND ||
        token->type == TM32ASM_TT_ASSIGN_BITWISE_OR ||
        token->type == TM32ASM_TT_ASSIGN_BITWISE_XOR ||
        token->type == TM32ASM_TT_ASSIGN_LEFT_SHIFT ||
        token->type == TM32ASM_TT_ASSIGN_RIGHT_SHIFT) {
        return true;
    }
    
    // Check for comparison operators
    if (token->type == TM32ASM_TT_COMPARE_EQUAL ||
        token->type == TM32ASM_TT_COMPARE_NOT_EQUAL ||
        token->type == TM32ASM_TT_COMPARE_LESS ||
        token->type == TM32ASM_TT_COMPARE_LESS_EQUAL ||
        token->type == TM32ASM_TT_COMPARE_GREATER ||
        token->type == TM32ASM_TT_COMPARE_GREATER_EQUAL) {
        return true;
    }
    
    // Check for logical operators
    if (token->type == TM32ASM_TT_LOGICAL_AND ||
        token->type == TM32ASM_TT_LOGICAL_OR ||
        token->type == TM32ASM_TT_LOGICAL_NOT) {
        return true;
    }
    
    // Check for grouping operators
    if (token->type == TM32ASM_TT_OPEN_PARENTHESIS ||
        token->type == TM32ASM_TT_CLOSE_PARENTHESIS ||
        token->type == TM32ASM_TT_OPEN_BRACKET ||
        token->type == TM32ASM_TT_CLOSE_BRACKET ||
        token->type == TM32ASM_TT_OPEN_BRACE ||
        token->type == TM32ASM_TT_CLOSE_BRACE) {
        return true;
    }
    
    // Check for punctuation tokens
    if (token->type == TM32ASM_TT_COMMA ||
        token->type == TM32ASM_TT_COLON ||
        token->type == TM32ASM_TT_AT ||
        token->type == TM32ASM_TT_QUESTION) {
        return true;
    }
    
    // Check for end-of-file token
    if (token->type == TM32ASM_TT_END_OF_FILE) {
        return true; // EOF always has predictable lexeme
    }
    
    return false;
}

/**
 * @brief Reconstructs the lexeme for a token with predictable lexemes.
 * 
 * @param[in] token The token to get the lexeme for.
 * @return A pointer to the reconstructed lexeme (caller should NOT free this).
 *         Returns NULL if the token doesn't have a predictable lexeme.
 */
static const char* TM32ASM_GetPredictableLexeme(const TM32ASM_Token* token)
{
    if (token == NULL) return NULL;

    // For keyword tokens, search the lookup table
    if (token->type >= TM32ASM_TT_DIRECTIVE && token->type <= TM32ASM_TT_KEYWORD)
    {
        for (size_t i = 0; KEYWORD_LOOKUP[i].lexeme != NULL; i++)
        {
            if (KEYWORD_LOOKUP[i].type == token->type && 
                KEYWORD_LOOKUP[i].param == token->param)
            {
                return KEYWORD_LOOKUP[i].lexeme;
            }
        }
        return NULL;
    }

    // Map token types directly to their lexemes
    switch (token->type)
    {
        // Arithmetic operators
        case TM32ASM_TT_PLUS:
            return "+";
        case TM32ASM_TT_MINUS:
            return "-";
        case TM32ASM_TT_TIMES:
            return "*";
        case TM32ASM_TT_EXPONENT:
            return "**";
        case TM32ASM_TT_DIVIDE:
            return "/";
        case TM32ASM_TT_MODULO:
            return "%";

        // Bitwise operators
        case TM32ASM_TT_BITWISE_AND:
            return "&";
        case TM32ASM_TT_BITWISE_OR:
            return "|";
        case TM32ASM_TT_BITWISE_XOR:
            return "^";
        case TM32ASM_TT_BITWISE_NOT:
            return "~";
        case TM32ASM_TT_LEFT_SHIFT:
            return "<<";
        case TM32ASM_TT_RIGHT_SHIFT:
            return ">>";

        // Assignment operators
        case TM32ASM_TT_ASSIGN_EQUAL:
            return "=";
        case TM32ASM_TT_ASSIGN_PLUS:
            return "+=";
        case TM32ASM_TT_ASSIGN_MINUS:
            return "-=";
        case TM32ASM_TT_ASSIGN_TIMES:
            return "*=";
        case TM32ASM_TT_ASSIGN_EXPONENT:
            return "**=";
        case TM32ASM_TT_ASSIGN_DIVIDE:
            return "/=";
        case TM32ASM_TT_ASSIGN_MODULO:
            return "%=";
        case TM32ASM_TT_ASSIGN_BITWISE_AND:
            return "&=";
        case TM32ASM_TT_ASSIGN_BITWISE_OR:
            return "|=";
        case TM32ASM_TT_ASSIGN_BITWISE_XOR:
            return "^=";
        case TM32ASM_TT_ASSIGN_LEFT_SHIFT:
            return "<<=";
        case TM32ASM_TT_ASSIGN_RIGHT_SHIFT:
            return ">>=";

        // Comparison operators
        case TM32ASM_TT_COMPARE_EQUAL:
            return "==";
        case TM32ASM_TT_COMPARE_NOT_EQUAL:
            return "!=";
        case TM32ASM_TT_COMPARE_LESS:
            return "<";
        case TM32ASM_TT_COMPARE_LESS_EQUAL:
            return "<=";
        case TM32ASM_TT_COMPARE_GREATER:
            return ">";
        case TM32ASM_TT_COMPARE_GREATER_EQUAL:
            return ">=";

        // Logical operators
        case TM32ASM_TT_LOGICAL_AND:
            return "&&";
        case TM32ASM_TT_LOGICAL_OR:
            return "||";
        case TM32ASM_TT_LOGICAL_NOT:
            return "!";

        // Grouping operators
        case TM32ASM_TT_OPEN_PARENTHESIS:
            return "(";
        case TM32ASM_TT_CLOSE_PARENTHESIS:
            return ")";
        case TM32ASM_TT_OPEN_BRACKET:
            return "[";
        case TM32ASM_TT_CLOSE_BRACKET:
            return "]";
        case TM32ASM_TT_OPEN_BRACE:
            return "{";
        case TM32ASM_TT_CLOSE_BRACE:
            return "}";

        // Punctuation tokens
        case TM32ASM_TT_COMMA:
            return ",";
        case TM32ASM_TT_COLON:
            return ":";
        case TM32ASM_TT_AT:
            return "@@";
        case TM32ASM_TT_QUESTION:
            return "?";

        // Control tokens
        case TM32ASM_TT_NEWLINE:
            return "\n";
        case TM32ASM_TT_END_OF_FILE:
            return "";

        default:
            break;
    }

    return NULL;
}

/* Public Functions ***********************************************************/

/**
 * @brief Gets the lexeme for any token, handling both stored and predictable lexemes.
 * 
 * @param[in] token The token to get the lexeme for.
 * @return A pointer to the token's lexeme. Caller should NOT free this pointer.
 *         For predictable tokens, returns a static string. For others, returns 
 *         the stored lexeme. Returns NULL if token is NULL or has no lexeme.
 */
const char* TM32ASM_GetTokenLexeme(const TM32ASM_Token* token)
{
    if (token == NULL) return NULL;

    // If the token has a stored lexeme, use it
    if (token->lexeme != NULL)
    {
        return token->lexeme;
    }

    // Otherwise, try to reconstruct it if it's predictable
    return TM32ASM_GetPredictableLexeme(token);
}

TM32ASM_Token* TM32ASM_CreateToken (
    const char*         lexeme,
    TM32ASM_TokenType   type
)
{
    // A lexeme is required. If a token does not have a lexeme, then `\0` should
    // be passed in for the lexeme.
    TM32ASM_ReturnValueIfNull(lexeme, NULL);

    TM32ASM_Token* token = TM32ASM_CreateZero(1, TM32ASM_Token);
    if (token == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the token");
        return NULL;
    }

    token->type = type;
    
    // Optimization: Skip heap allocation for tokens with predictable lexemes
    // Create temporary token for predictability check
    TM32ASM_Token temp_token = { .type = type, .param = 0 };
    if (TM32ASM_IsTokenLexemePredictable(&temp_token))
    {
        token->lexeme = NULL;  // Will be reconstructed on-demand by TM32ASM_GetTokenLexeme()
    }
    else
    {
        token->lexeme = strdup(lexeme);
        if (token->lexeme == NULL)
        {
            TM32ASM_LogErrno("Could not allocate memory for the token lexeme");
            TM32ASM_DestroyToken(token);
            return NULL;
        }
    }

    // The struct's other fields are to be set later, by the lexer and
    // preprocessor if necessary.

    return token;
}

void TM32ASM_DestroyToken (
    TM32ASM_Token* token
)
{
    if (token != NULL)
    {
        if (token->lexeme != NULL)
        {
            TM32ASM_Destroy(token->lexeme);
            token->lexeme = NULL;
        }
        TM32ASM_Destroy(token);
    }
}

TM32ASM_Token* TM32ASM_CopyToken (
    const TM32ASM_Token* token
)
{
    TM32ASM_ReturnValueIfNull(token, NULL);

    // Get the actual lexeme (either stored or reconstructed)
    const char* lexeme = TM32ASM_GetTokenLexeme(token);
    TM32ASM_Token* copy = TM32ASM_CreateToken(lexeme, token->type);
    if (copy == NULL)
    {
        return NULL;
    }

    copy->param = token->param;
    copy->filename = token->filename; // Note: filename is not owned by the token, so we just copy the pointer
    copy->line = token->line;

    return copy;
}

bool TM32ASM_GetKeyword (
    const TM32ASM_Token*     token,
    TM32ASM_Token**          out
)
{
    TM32ASM_ReturnValueIfNull(token, false);
    TM32ASM_ReturnValueIfNull(out, false);

    // Get the actual lexeme (either stored or reconstructed)
    const char* lexeme = TM32ASM_GetTokenLexeme(token);
    
    for (size_t i = 0; KEYWORD_LOOKUP[i].lexeme != NULL; i++)
    {
        if (strcasecmp(lexeme, KEYWORD_LOOKUP[i].lexeme) == 0)
        {
            *out = TM32ASM_CreateToken(
                KEYWORD_LOOKUP[i].lexeme,
                KEYWORD_LOOKUP[i].type
            );
            if (*out == NULL)
            {
                return false;
            }
            (*out)->param = KEYWORD_LOOKUP[i].param;
            (*out)->filename = token->filename;
            (*out)->line = token->line;
            return true;
        }
    }

    return false;
}

uint32_t TM32ASM_GetIntegerLexeme (
    const TM32ASM_Token* token
)
{
    TM32ASM_ReturnValueIfNull(token, 0);

    // Get the actual lexeme (either stored or reconstructed)
    const char* lexeme = TM32ASM_GetTokenLexeme(token);

    switch (token->type)
    {
        case TM32ASM_TT_BINARY:
            return (uint32_t) strtoull(lexeme, NULL, 2);
        case TM32ASM_TT_OCTAL:
            return (uint32_t) strtoull(lexeme, NULL, 8);
        case TM32ASM_TT_DECIMAL:
            return (uint32_t) strtoull(lexeme, NULL, 10);
        case TM32ASM_TT_HEXADECIMAL:
            return (uint32_t) strtoull(lexeme, NULL, 16);
        case TM32ASM_TT_FIXED_POINT:
        {
            uint64_t fixedPoint = TM32ASM_GetFixedPointLexeme(token);
            return (uint32_t) (fixedPoint >> 32);
        }
        default: return 0;
    }
}

double TM32ASM_GetNumberLexeme (
    const TM32ASM_Token* token
)
{
    TM32ASM_ReturnValueIfNull(token, 0.0);

    // Get the actual lexeme (either stored or reconstructed)
    const char* lexeme = TM32ASM_GetTokenLexeme(token);

    switch (token->type)
    {
        case TM32ASM_TT_BINARY:
            return (double) strtoull(lexeme, NULL, 2);
        case TM32ASM_TT_OCTAL:
            return (double) strtoull(lexeme, NULL, 8);
        case TM32ASM_TT_DECIMAL:
            return (double) strtoull(lexeme, NULL, 10);
        case TM32ASM_TT_HEXADECIMAL:
            return (double) strtoull(lexeme, NULL, 16);
        case TM32ASM_TT_FIXED_POINT:
            return strtod(lexeme, NULL);
        default: return 0.0;
    }
}

uint64_t TM32ASM_GetFixedPointLexeme (
    const TM32ASM_Token* token
)
{
    TM32ASM_ReturnValueIfNull(token, 0);

    if (token->type != TM32ASM_TT_FIXED_POINT)
    {
        return 0;
    }

    // Get the actual lexeme (either stored or reconstructed)
    const char* lexeme = TM32ASM_GetTokenLexeme(token);

    // Convert the string to a floating-point number first. Extract the integer
    // and fractional parts from that number.
    double floatNumber = strtod(lexeme, NULL);
    double floatInteger = 0.0;
    double floatFraction = modf(floatNumber, &floatInteger);

    // Combine the integer and fractional parts into a fixed-point representation.
    uint64_t fixedPoint = (uint64_t) floatInteger << 32;
    fixedPoint |= (uint32_t) (floatFraction * (1LL << 32));
    return fixedPoint;
}
