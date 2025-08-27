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
    { ".include",       TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INCLUDE      },
    { ".let",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_LET          },
    { ".const",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_CONST        },
    { ".define",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DEFINE       },
    { ".def",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DEFINE       },
    { ".macro",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_MACRO        },
    { ".shift",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_SHIFT        },
    { ".endmacro",      TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDMACRO     },
    { ".endm",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDMACRO     },
    { ".if",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_IF           },
    { ".else",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ELSE         },
    { ".elseif",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ELSEIF       },
    { ".elif",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ELSEIF       },
    { ".endif",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDIF        },
    { ".endc",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDIF        },
    { ".repeat",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_REPEAT       },
    { ".rept",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_REPEAT       },
    { ".endrepeat",     TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDREPEAT    },
    { ".endr",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDREPEAT    },
    { ".while",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_WHILE        },
    { ".endwhile",      TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDWHILE     },
    { ".endw",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDWHILE     },
    { ".for",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_FOR          },
    { ".endfor",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDFOR       },
    { ".endf",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ENDFOR       },
    
    // Assembler Directives
    { ".metadata",      TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_METADATA     },
    { ".interrupt",     TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INTERRUPT    },
    { ".int",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INTERRUPT    },
    { ".text",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_TEXT         },
    { ".code",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_TEXT         },
    { ".rodata",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_RODATA       },
    { ".data",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DATA         },
    { ".bss",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_BSS          },
    { ".org",           TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ORG          },
    { ".db",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DB           },
    { ".byte",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DB           },
    { ".dw",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DW           },
    { ".word",          TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DW           },
    { ".dd",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DD           },
    { ".dword",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DD           },
    { ".asciz",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ASCIZ        },
    { ".stringz",       TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ASCIZ        },
    { ".align",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_ALIGN        },
    { ".ds",            TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DS           },
    { ".space",         TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_DS           },
    { ".incbin",        TM32ASM_TT_DIRECTIVE,       TM32ASM_DT_INCBIN       },

    // Built-In Functions
    { "high",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_HIGH         },
    { "low",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_LOW          },
    { "bitwidth",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_BITWIDTH     },
    { "integer",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_INTEGER      },
    { "fraction",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_FRACTION     },
    { "round",          TM32ASM_TT_FUNCTION,        TM32ASM_FT_ROUND        },
    { "ceil",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_CEIL         },
    { "floor",          TM32ASM_TT_FUNCTION,        TM32ASM_FT_FLOOR        },
    { "sin",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_SIN          },
    { "cos",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_COS          },
    { "tan",            TM32ASM_TT_FUNCTION,        TM32ASM_FT_TAN          },
    { "asin",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_ASIN         },
    { "acos",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_ACOS         },
    { "atan",           TM32ASM_TT_FUNCTION,        TM32ASM_FT_ATAN         },
    { "strlen",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRLEN       },
    { "strupper",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRUPPER     },
    { "strlower",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRLOWER     },
    { "strcmp",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRCMP       },
    { "strfind",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRFIND      },
    { "strrfind",       TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRRFIND     },
    { "strbyte",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_STRBYTE      },
    { "defined",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_DEFINED      },
    { "isconst",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_ISCONST      },
    { "isvariable",     TM32ASM_TT_FUNCTION,        TM32ASM_FT_ISVARIABLE   },
    { "section",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_SECTION      },
    { "origin",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_ORIGIN       },
    { "sizeof",         TM32ASM_TT_FUNCTION,        TM32ASM_FT_SIZEOF       },
    { "startof",        TM32ASM_TT_FUNCTION,        TM32ASM_FT_STARTOF      },

    // General-Purpose CPU Registers
    { "a",              TM32ASM_TT_REGISTER,        TM32CPU_RT_A            },
    { "aw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_AW           },
    { "ah",             TM32ASM_TT_REGISTER,        TM32CPU_RT_AH           },
    { "al",             TM32ASM_TT_REGISTER,        TM32CPU_RT_AL           },
    { "b",              TM32ASM_TT_REGISTER,        TM32CPU_RT_B            },
    { "bw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_BW           },
    { "bh",             TM32ASM_TT_REGISTER,        TM32CPU_RT_BH           },
    { "bl",             TM32ASM_TT_REGISTER,        TM32CPU_RT_BL           },
    { "c",              TM32ASM_TT_REGISTER,        TM32CPU_RT_C            },
    { "cw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_CW           },
    { "ch",             TM32ASM_TT_REGISTER,        TM32CPU_RT_CH           },
    { "cl",             TM32ASM_TT_REGISTER,        TM32CPU_RT_CL           },
    { "d",              TM32ASM_TT_REGISTER,        TM32CPU_RT_D            },
    { "dw",             TM32ASM_TT_REGISTER,        TM32CPU_RT_DW           },
    { "dh",             TM32ASM_TT_REGISTER,        TM32CPU_RT_DH           },
    { "dl",             TM32ASM_TT_REGISTER,        TM32CPU_RT_DL           },

    // Execution Conditions
    { "nc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_NC           },
    { "zc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_ZC           },
    { "zs",             TM32ASM_TT_CONDITION,       TM32CPU_CT_ZS           },
    { "cc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_CC           },
    { "cs",             TM32ASM_TT_CONDITION,       TM32CPU_CT_CS           },
    { "pc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_PC           },
    { "ps",             TM32ASM_TT_CONDITION,       TM32CPU_CT_PS           },
    { "sc",             TM32ASM_TT_CONDITION,       TM32CPU_CT_SC           },
    { "ss",             TM32ASM_TT_CONDITION,       TM32CPU_CT_SS           },

    // CPU Instructions
    { "nop",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_NOP          },
    { "stop",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STOP         },
    { "halt",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_HALT         },
    { "sec",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SEC          },
    { "cec",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CEC          },
    { "di",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_DI           },
    { "ei",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_EI           },
    { "daa",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_DAA          },
    { "scf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SCF          },
    { "ccf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CCF          },
    { "flg",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_FLG          },
    { "stf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STF          },
    { "clf",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CLF          },
    { "ld",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_LD           },
    { "ldh",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_LDH          },
    { "ldp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_LDP          },
    { "st",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_ST           },
    { "sth",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STH          },
    { "stp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_STP          },
    { "mv",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_MV           },
    { "pop",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_POP          },
    { "push",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_PUSH         },
    { "jmp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_JMP          },
    { "jpb",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_JPB          },
    { "call",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CALL         },
    { "int",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_INT          },
    { "ret",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RET          },
    { "reti",           TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RETI         },
    { "inc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_INC          },
    { "dec",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_DEC          },
    { "add",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_ADD          },
    { "adc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_ADC          },
    { "sub",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SUB          },
    { "sbc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SBC          },
    { "and",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_AND          },
    { "or",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_OR           },
    { "xor",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_XOR          },
    { "not",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_NOT          },
    { "cmp",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_CMP          },
    { "sla",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SLA          },
    { "sra",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SRA          },
    { "srl",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SRL          },
    { "rl",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RL           },
    { "rlc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RLC          },
    { "rr",             TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RR           },
    { "rrc",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RRC          },
    { "bit",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_BIT          },
    { "tog",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_TOG          },
    { "set",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_SET          },
    { "res",            TM32ASM_TT_INSTRUCTION,     TM32CPU_IT_RES          },

    { NULL,             TM32ASM_TT_UNKNOWN,         0                       }
};

/* Public Functions ***********************************************************/

TM32ASM_Token* TM32ASM_CreateToken (
    const char*         lexeme,
    TM32ASM_TokenType   type
)
{
    TM32ASM_Token* token = TM32ASM_CreateZero(1, TM32ASM_Token);
    if (token == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the token");
        return NULL;
    }

    token->type = type;
    token->lexeme = strdup(lexeme);
    if (token->lexeme == NULL)
    {
        TM32ASM_LogErrno("Could not allocate memory for the token lexeme");
        TM32ASM_DestroyToken(token);
        return NULL;
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
        TM32ASM_Destroy(token->lexeme);
        TM32ASM_Destroy(token);
    }
}

bool TM32ASM_GetKeyword (
    const TM32ASM_Token*     token,
    TM32ASM_Token**          out
)
{
    TM32ASM_ReturnValueIfNull(token, false);
    TM32ASM_ReturnValueIfNull(out, false);

    for (size_t i = 0; KEYWORD_LOOKUP[i].lexeme != NULL; i++)
    {
        if (strcmp(token->lexeme, KEYWORD_LOOKUP[i].lexeme) == 0)
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

    switch (token->type)
    {
        case TM32ASM_TT_BINARY:
            return (uint32_t) strtoull(token->lexeme, NULL, 2);
        case TM32ASM_TT_OCTAL:
            return (uint32_t) strtoull(token->lexeme, NULL, 8);
        case TM32ASM_TT_DECIMAL:
            return (uint32_t) strtoull(token->lexeme, NULL, 10);
        case TM32ASM_TT_HEXADECIMAL:
            return (uint32_t) strtoull(token->lexeme, NULL, 16);
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

    switch (token->type)
    {
        case TM32ASM_TT_BINARY:
            return (double) strtoull(token->lexeme, NULL, 2);
        case TM32ASM_TT_OCTAL:
            return (double) strtoull(token->lexeme, NULL, 8);
        case TM32ASM_TT_DECIMAL:
            return (double) strtoull(token->lexeme, NULL, 10);
        case TM32ASM_TT_HEXADECIMAL:
            return (double) strtoull(token->lexeme, NULL, 16);
        case TM32ASM_TT_FIXED_POINT:
            return strtod(token->lexeme, NULL);
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

    // Convert the string to a floating-point number first. Extract the integer
    // and fractional parts from that number.
    double floatNumber = strtod(token->lexeme, NULL);
    double floatInteger = 0.0;
    double floatFraction = modf(floatNumber, &floatInteger);

    // Combine the integer and fractional parts into a fixed-point representation.
    uint64_t fixedPoint = (uint64_t) floatInteger << 32;
    fixedPoint |= (uint32_t) (floatFraction * (1LL << 32));
    return fixedPoint;
}
