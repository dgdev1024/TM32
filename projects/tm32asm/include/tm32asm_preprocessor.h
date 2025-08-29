/**
 * @file    tm32asm_preprocessor.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Contains the preprocessor structure and related definitions and prototypes.
 */

#ifndef TM32ASM_PREPROCESSOR_H
#define TM32ASM_PREPROCESSOR_H

/* Include Files **************************************************************/

#include <tm32asm_token.h>
#include <tm32asm_token_stream.h>

/* Public Constants ***********************************************************/

#define TM32ASM_PREPROCESSOR_INITIAL_SYMBOL_CAPACITY        32
#define TM32ASM_PREPROCESSOR_INITIAL_CONDITIONAL_CAPACITY   16
#define TM32ASM_PREPROCESSOR_INITIAL_LOOP_CAPACITY          8
#define TM32ASM_PREPROCESSOR_INITIAL_INCLUDE_CAPACITY       8
#define TM32ASM_PREPROCESSOR_MAX_MACRO_EXPANSION_DEPTH      64
#define TM32ASM_PREPROCESSOR_MAX_INCLUDE_DEPTH              32
#define TM32ASM_PREPROCESSOR_MAX_LOOP_ITERATIONS            1000000
#define TM32ASM_PREPROCESSOR_MAX_EXPRESSION_DEPTH           64

/* Public Enumerations ********************************************************/

/**
 * @brief   Enumerates the different types of expression values.
 */
typedef enum
{
    TM32ASM_VT_INTEGER = 0,             /** @brief Integer value */
    TM32ASM_VT_FIXED_POINT,             /** @brief Fixed-point value */
    TM32ASM_VT_STRING                   /** @brief String value */
} TM32ASM_ValueType;

/**
 * @brief   Enumerates the different types of symbols that can be stored
 *          in the preprocessor's symbol table.
 */
typedef enum
{
    TM32ASM_ST_UNKNOWN = 0,             /** @brief Unknown symbol type */
    TM32ASM_ST_VARIABLE,                /** @brief Mutable variable (`.let`) */
    TM32ASM_ST_CONSTANT,                /** @brief Immutable constant (`.const`) */
    TM32ASM_ST_SIMPLE_MACRO,            /** @brief Simple text substitution macro (`.define`, `.def`) */
    TM32ASM_ST_PARAMETRIC_MACRO         /** @brief Parametric macro (`.macro`) */
} TM32ASM_SymbolType;

/**
 * @brief   Enumerates the different states of conditional compilation blocks.
 */
typedef enum
{
    TM32ASM_CS_INACTIVE = 0,            /** @brief Block is not being processed */
    TM32ASM_CS_ACTIVE,                  /** @brief Block is being processed */
    TM32ASM_CS_SKIPPING                 /** @brief Block is being skipped */
} TM32ASM_ConditionalState;

/**
 * @brief   Enumerates the different types of loop constructs supported
 *          by the preprocessor.
 */
typedef enum
{
    TM32ASM_LT_REPEAT = 0,              /** @brief `.repeat` / `.rept` loop */
    TM32ASM_LT_WHILE,                   /** @brief `.while` loop */
    TM32ASM_LT_FOR                      /** @brief `.for` loop */
} TM32ASM_LoopType;

/* Public Structures **********************************************************/

/**
 * @brief   Represents a value that can be produced by expression evaluation.
 */
typedef struct
{
    TM32ASM_ValueType   type;           /** @brief Type of the value */
    union
    {
        int64_t         integerValue;   /** @brief Integer value */
        double          fixedPointValue;/** @brief Fixed-point value */
        char*           stringValue;    /** @brief String value */
    };
} TM32ASM_Value;

/**
 * @brief   Represents a symbol stored in the preprocessor's symbol table.
 */
typedef struct
{
    char*                   name;           /** @brief Symbol name */
    TM32ASM_SymbolType      type;           /** @brief Symbol type */
    char*                   value;          /** @brief Symbol value (for variables/constants/simple macros) */
    char**                  parameters;     /** @brief Parameter names (for parametric macros) */
    size_t                  parameterCount; /** @brief Number of parameters (for parametric macros) */
    TM32ASM_TokenStream*    body;           /** @brief Macro body tokens (for parametric macros) */
    bool                    isDefined;      /** @brief Whether the symbol is currently defined */
} TM32ASM_Symbol;

/**
 * @brief   Represents a conditional compilation context.
 */
typedef struct
{
    TM32ASM_ConditionalState    state;          /** @brief Current state of the conditional block */
    bool                        hasElse;        /** @brief Whether an `.else` has been encountered */
    bool                        conditionMet;   /** @brief Whether any condition in this block was true */
} TM32ASM_ConditionalContext;

/**
 * @brief   Represents a loop context for repetition constructs.
 */
typedef struct
{
    TM32ASM_LoopType        type;               /** @brief Type of loop */
    char*                   variableName;       /** @brief Loop variable name (for `.for` loops) */
    int64_t                 currentValue;       /** @brief Current loop variable value */
    int64_t                 endValue;           /** @brief End value for loop */
    int64_t                 stepValue;          /** @brief Step value for loop */
    size_t                  iterationCount;     /** @brief Current iteration count */
    size_t                  maxIterations;      /** @brief Maximum iterations */
    TM32ASM_TokenStream*    bodyTokens;         /** @brief Tokens to repeat */
    size_t                  bodyStartIndex;     /** @brief Start index of loop body in token stream */
    size_t                  bodyEndIndex;       /** @brief End index of loop body in token stream */
} TM32ASM_LoopContext;

/**
 * @brief   Represents a file inclusion context for tracking nested includes.
 */
typedef struct
{
    char*                   filename;       /** @brief Name of the included file */
    uint32_t                line;           /** @brief Current line in the included file */
    TM32ASM_TokenStream*    originalStream; /** @brief Original token stream before inclusion */
    size_t                  insertionPoint; /** @brief Where to insert processed tokens */
} TM32ASM_IncludeContext;

/**
 * @brief   Represents the state and context of the TM32 Assembler Tool's
 *          preprocessor as it processes preprocessor directives to mutate the
 *          token stream produced by the lexer.
 * 
 * The preprocessor is the second step of the TM32 assembly process, following
 * the lexer. It is responsible for handling directives such as macros, file
 * inclusions, and conditional compilation. It is also responsible for instructing
 * the lexer to extract tokens from source files included with the `.include`
 * preprocessor directive.
 */
typedef struct
{
    // Symbol Management
    TM32ASM_Symbol*             symbols;                /** @brief Array of symbols */
    size_t                      symbolCount;            /** @brief Number of symbols */
    size_t                      symbolCapacity;         /** @brief Capacity of symbols array */

    // Conditional Compilation Stack
    TM32ASM_ConditionalContext* conditionalStack;       /** @brief Stack of conditional contexts */
    size_t                      conditionalDepth;       /** @brief Current conditional nesting depth */
    size_t                      conditionalCapacity;    /** @brief Capacity of conditional stack */

    // Loop Context Stack
    TM32ASM_LoopContext*        loopStack;              /** @brief Stack of loop contexts */
    size_t                      loopDepth;              /** @brief Current loop nesting depth */
    size_t                      loopCapacity;           /** @brief Capacity of loop stack */

    // File Inclusion Stack
    TM32ASM_IncludeContext*     includeStack;           /** @brief Stack of include contexts */
    size_t                      includeDepth;           /** @brief Current include nesting depth */
    size_t                      includeCapacity;        /** @brief Capacity of include stack */
    char**                      includedFiles;          /** @brief List of already included files */
    size_t                      includedFileCount;      /** @brief Number of included files */
    char**                      includePaths;           /** @brief Search paths for include files */
    size_t                      includePathCount;       /** @brief Number of include paths */

    // Token Stream Management
    TM32ASM_TokenStream*        inputStream;            /** @brief Input token stream from lexer */
    TM32ASM_TokenStream*        outputStream;           /** @brief Output token stream for parser */
    size_t                      currentTokenIndex;      /** @brief Current position in input stream */

    // Error and Warning Handling
    bool                        treatWarningsAsErrors;  /** @brief Whether to treat warnings as errors */
    bool                        hasErrors;              /** @brief Whether any errors have occurred */
    uint32_t                    currentLine;            /** @brief Current line for error reporting */
    const char*                 currentFilename;        /** @brief Current filename for error reporting */

    // Macro Expansion State
    uint32_t                    macroExpansionDepth;    /** @brief Current macro expansion depth */
    uint32_t                    maxMacroExpansionDepth; /** @brief Maximum allowed macro expansion depth */
    uint64_t                    uniqueCounter;          /** @brief Counter for generating unique identifiers */
} TM32ASM_Preprocessor;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates a new instance of the TM32 Assembly Tool's
 *          preprocessor (henceforth, "the TM32ASM preprocessor").
 * 
 * @return  A pointer to the newly-created preprocessor instance if successful;
 *          `NULL` otherwise.
 */
TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ();

/**
 * @brief   Destroys and deallocates the given TM32ASM preprocessor.
 * 
 * @param   preprocessor   A pointer to the TM32ASM preprocessor to destroy.
 */
void TM32ASM_DestroyPreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Processes a token stream through the preprocessor, handling all
 *          preprocessor directives and producing a modified token stream.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   inputStream     A pointer to the input token stream from the lexer.
 * 
 * @return  A pointer to the processed token stream if successful; `NULL` otherwise.
 */
TM32ASM_TokenStream* TM32ASM_ProcessTokenStream (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    inputStream
);

/**
 * @brief   Adds an include search path to the preprocessor.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   path            The include path to add.
 * 
 * @return  `true` if the path was added successfully; `false` otherwise.
 */
bool TM32ASM_AddIncludePath (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             path
);

/**
 * @brief   Defines a variable in the preprocessor's symbol table.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   name            The name of the variable.
 * @param   value           The initial value of the variable (can be NULL).
 * 
 * @return  `true` if the variable was defined successfully; `false` otherwise.
 */
bool TM32ASM_DefineVariable (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value
);

/**
 * @brief   Defines a constant in the preprocessor's symbol table.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   name            The name of the constant.
 * @param   value           The value of the constant.
 * 
 * @return  `true` if the constant was defined successfully; `false` otherwise.
 */
bool TM32ASM_DefineConstant (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value
);

/**
 * @brief   Defines a simple text substitution macro in the preprocessor's symbol table.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   name            The name of the macro.
 * @param   value           The replacement text for the macro.
 * 
 * @return  `true` if the macro was defined successfully; `false` otherwise.
 */
bool TM32ASM_DefineSimpleMacro (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value
);

/**
 * @brief   Sets whether warnings should be treated as errors.
 * 
 * @param   preprocessor            A pointer to the TM32ASM preprocessor.
 * @param   treatWarningsAsErrors   Whether to treat warnings as errors.
 */
void TM32ASM_SetTreatWarningsAsErrors (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    treatWarningsAsErrors
);

/**
 * @brief   Checks if the preprocessor has encountered any errors during processing.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * 
 * @return  `true` if errors have been encountered; `false` otherwise.
 */
bool TM32ASM_HasErrors (
    const TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Evaluates an expression represented by a sequence of tokens.
 * 
 * @param   preprocessor    A pointer to the TM32ASM preprocessor.
 * @param   tokens          A pointer to the token stream containing the expression.
 * @param   startIndex      The starting index of the expression in the token stream.
 * @param   endIndex        The ending index of the expression in the token stream.
 * @param   result          A pointer to store the evaluation result.
 * 
 * @return  `true` if evaluation was successful; `false` otherwise.
 */
bool TM32ASM_EvaluateExpression (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokens,
    size_t                  startIndex,
    size_t                  endIndex,
    TM32ASM_Value*          result
);

/**
 * @brief   Creates a new value with an integer.
 * 
 * @param   value   The integer value.
 * 
 * @return  A new TM32ASM_Value containing the integer.
 */
TM32ASM_Value TM32ASM_CreateIntegerValue (
    int64_t value
);

/**
 * @brief   Creates a new value with a fixed-point number.
 * 
 * @param   value   The fixed-point value.
 * 
 * @return  A new TM32ASM_Value containing the fixed-point number.
 */
TM32ASM_Value TM32ASM_CreateFixedPointValue (
    double value
);

/**
 * @brief   Creates a new value with a string.
 * 
 * @param   value   The string value (will be copied).
 * 
 * @return  A new TM32ASM_Value containing the string.
 */
TM32ASM_Value TM32ASM_CreateStringValue (
    const char* value
);

/**
 * @brief   Destroys a value and frees any associated memory.
 * 
 * @param   value   A pointer to the value to destroy.
 */
void TM32ASM_DestroyValue (
    TM32ASM_Value* value
);

/**
 * @brief   Converts a value to a string representation.
 * 
 * @param   value   A pointer to the value to convert.
 * 
 * @return  A newly allocated string representation of the value, or NULL on failure.
 */
char* TM32ASM_ValueToString (
    const TM32ASM_Value* value
);

#endif // TM32ASM_PREPROCESSOR_H
