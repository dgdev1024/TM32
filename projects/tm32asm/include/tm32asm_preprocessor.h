/**
 * @file    tm32asm_preprocessor.h
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/30
 *
 * @brief   Contains the definitions and prototypes for the TM32 assembler
 *          tool's preprocessor component.
 */

#ifndef TM32ASM_PREPROCESSOR_H
#define TM32ASM_PREPROCESSOR_H

/* Include Files **************************************************************/

#include <tm32asm_common.h>
#include <tm32asm_lexer.h>

/* Public Constants ***********************************************************/

#define TM32ASM_MAX_INCLUDE_DEPTH       32      /** @brief Maximum include file nesting depth */
#define TM32ASM_MAX_MACRO_NESTING       64      /** @brief Maximum macro expansion nesting depth */
#define TM32ASM_MAX_LOOP_NESTING        16      /** @brief Maximum loop construct nesting depth */

/* Public Enumerations ********************************************************/

/**
 * @brief   Enumerates the types of preprocessor symbols.
 */
typedef enum
{
    TM32ASM_ST_MACRO_SIMPLE,            /** @brief Simple text substitution macro */
    TM32ASM_ST_MACRO_PARAMETRIC,        /** @brief Parametric macro with arguments */
    TM32ASM_ST_VARIABLE,                /** @brief Mutable variable */
    TM32ASM_ST_CONSTANT,                /** @brief Immutable constant */
} TM32ASM_SymbolType;

/**
 * @brief   Enumerates the types of preprocessor control flow contexts.
 */
typedef enum
{
    TM32ASM_CFT_CONDITIONAL,            /** @brief `.if` conditional block */
    TM32ASM_CFT_REPEAT,                 /** @brief `.repeat` loop block */
    TM32ASM_CFT_WHILE,                  /** @brief `.while` loop block */
    TM32ASM_CFT_FOR,                    /** @brief `.for` loop block */
} TM32ASM_ControlFlowType;

/**
 * @brief   Enumerates the different passes of the preprocessor.
 */
typedef enum
{
    TM32ASM_PP_FILE_INCLUSION,          /** @brief File inclusion pass */
    TM32ASM_PP_SYMBOL_DECLARATION,      /** @brief Symbol declaration pass */
    TM32ASM_PP_MACRO_EXPANSION,         /** @brief Macro expansion pass */
    TM32ASM_PP_VARIABLE_EVALUATION,     /** @brief Variable and constant evaluation pass */
    TM32ASM_PP_CONTROL_FLOW,            /** @brief Control flow and loop processing pass */
    TM32ASM_PP_FINALIZATION,            /** @brief Final token stream generation pass */
    TM32ASM_PP_NEWLINE_OPTIMIZATION,    /** @brief Newline optimization pass */
} TM32ASM_PreprocessorPass;

/* Public Structures **********************************************************/

/**
 * @brief   Represents a preprocessor symbol (macro, variable, or constant).
 */
typedef struct
{
    char*                   name;           /** @brief Symbol name */
    TM32ASM_SymbolType      type;           /** @brief Symbol type */
    char*                   value;          /** @brief Symbol value or replacement text */
    char**                  parameters;     /** @brief Parameter names for parametric macros */
    size_t                  parameterCount; /** @brief Number of parameters */
    TM32ASM_TokenStream*    macroBody;      /** @brief Token stream for parametric macro body */
    bool                    isDefined;      /** @brief Whether the symbol is defined */
    uint32_t                line;           /** @brief Line where symbol is defined */
    const char*             filename;       /** @brief File where symbol is defined */
} TM32ASM_Symbol;

/**
 * @brief   Represents a file inclusion context.
 */
typedef struct
{
    const char*             filename;       /** @brief Name of the included file */
    TM32ASM_TokenStream*    tokenStream;    /** @brief Token stream from the file */
    size_t                  currentPosition;/** @brief Current position in token stream */
    uint32_t                line;           /** @brief Current line number in file */
} TM32ASM_FileContext;

/**
 * @brief   Represents a control flow context for conditional assembly and loops.
 */
typedef struct
{
    TM32ASM_ControlFlowType type;           /** @brief Type of control flow construct */
    bool                    isActive;       /** @brief Whether this context is currently active */
    uint32_t                startLine;      /** @brief Line where construct starts */
    const char*             filename;       /** @brief File where construct is defined */
    
    // Context-specific data
    union
    {
        struct
        {
            bool            conditionMet;   /** @brief Whether any condition was met */
            bool            inElseBlock;    /** @brief Whether we're in else block */
        } conditional;
        
        struct
        {
            uint32_t        count;          /** @brief Repeat count */
            uint32_t        current;        /** @brief Current iteration */
            char*           iterationVar;   /** @brief Optional iteration variable name */
        } repeat;
        
        struct
        {
            char*           condition;      /** @brief While condition expression */
        } whileLoop;
        
        struct
        {
            char*           variable;       /** @brief Loop variable name */
            char*           start;          /** @brief Start value expression */
            char*           end;            /** @brief End value expression */
            char*           step;           /** @brief Step value expression */
            uint32_t        current;        /** @brief Current iteration value */
        } forLoop;
    } data;
} TM32ASM_ControlFlowContext;

/**
 * @brief   Represents a macro expansion context for tracking nested macro calls.
 */
typedef struct
{
    const TM32ASM_Symbol*   macroSymbol;    /** @brief The macro symbol being expanded */
    TM32ASM_TokenStream**   parameters;     /** @brief Array of parameter token streams */
    size_t                  parameterCount; /** @brief Number of parameters */
    size_t                  currentPosition;/** @brief Current position in macro body */
} TM32ASM_MacroContext;

/**
 * @brief   The TM32 assembler tool's preprocessor component structure.
 */
typedef struct
{
    // Input and output token streams
    TM32ASM_TokenStream*        inputTokenStream;       /** @brief Input token stream from lexer */
    TM32ASM_TokenStream*        outputTokenStream;      /** @brief Output token stream for parser */
    
    // Symbol management
    TM32ASM_Symbol*             symbols;                /** @brief Array of preprocessor symbols */
    size_t                      symbolCount;            /** @brief Number of defined symbols */
    size_t                      symbolCapacity;         /** @brief Capacity of symbols array */
    
    // File inclusion management
    TM32ASM_FileContext*        fileStack;              /** @brief Stack of included file contexts */
    size_t                      fileStackDepth;         /** @brief Current include depth */
    char**                      includePaths;           /** @brief Array of include search paths */
    size_t                      includePathCount;       /** @brief Number of include search paths */
    char**                      includedFiles;          /** @brief Array of already included files */
    size_t                      includedFileCount;      /** @brief Number of already included files */
    
    // Control flow management
    TM32ASM_ControlFlowContext* controlFlowStack;       /** @brief Stack of control flow contexts */
    size_t                      controlFlowDepth;       /** @brief Current control flow nesting depth */
    
    // Macro expansion management
    TM32ASM_MacroContext**      macroStack;             /** @brief Stack of macro expansion contexts */
    size_t                      macroStackDepth;        /** @brief Current macro expansion depth */
    uint32_t                    nextUniqueId;           /** @brief Next unique ID for @? mangling */
    
    // Processing state
    TM32ASM_PreprocessorPass    currentPass;            /** @brief Current processing pass */
    size_t                      currentTokenIndex;      /** @brief Current position in token stream */
    bool                        errorOccurred;          /** @brief Whether an error has occurred */
    
    // Options and flags
    bool                        warningsAsErrors;       /** @brief Treat warnings as errors */
    bool                        verbose;                /** @brief Enable verbose output */
} TM32ASM_Preprocessor;

/* Public Function Prototypes *************************************************/

/**
 * @brief   Allocates and creates an instance of the TM32 assembler tool's
 *          preprocessor component.
 *
 * @return  A pointer to the newly created TM32ASM preprocessor component, or 
 *          `NULL` if the allocation fails.
 */
TM32ASM_Preprocessor* TM32ASM_CreatePreprocessor ();

/**
 * @brief   Destroys and deallocates an instance of the TM32 assembler tool's
 *          preprocessor component.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance to
 *                          destroy. If `NULL`, no action is taken.
 */
void TM32ASM_DestroyPreprocessor (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Adds an include search path to the preprocessor.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   path            The include path to add.
 *
 * @return  `true` if the path was added successfully; `false` otherwise.
 */
bool TM32ASM_AddIncludePath (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             path
);

/**
 * @brief   Defines a preprocessor symbol (variable, constant, or macro).
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   name            The symbol name.
 * @param   value           The symbol value or replacement text.
 * @param   type            The symbol type.
 *
 * @return  `true` if the symbol was defined successfully; `false` otherwise.
 */
bool TM32ASM_DefineSymbol (
    TM32ASM_Preprocessor*   preprocessor,
    const char*             name,
    const char*             value,
    TM32ASM_SymbolType      type
);

/**
 * @brief   Sets the input token stream for preprocessing.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   tokenStream     The input token stream from the lexer.
 *
 * @return  `true` if the token stream was set successfully; `false` otherwise.
 */
bool TM32ASM_SetInputTokenStream (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream
);

/**
 * @brief   Processes the input token stream through all preprocessor passes.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  `true` if preprocessing was successful; `false` otherwise.
 */
bool TM32ASM_ProcessTokenStream (
    TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Gets the output token stream after preprocessing.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 *
 * @return  A pointer to the output token stream, or `NULL` if preprocessing
 *          has not been performed or failed.
 */
TM32ASM_TokenStream* TM32ASM_GetOutputTokenStream (
    const TM32ASM_Preprocessor* preprocessor
);

/**
 * @brief   Sets preprocessor options.
 *
 * @param   preprocessor        A pointer to the TM32ASM_Preprocessor instance.
 * @param   warningsAsErrors    Whether to treat warnings as errors.
 * @param   verbose             Whether to enable verbose output.
 */
void TM32ASM_SetPreprocessorOptions (
    TM32ASM_Preprocessor*   preprocessor,
    bool                    warningsAsErrors,
    bool                    verbose
);

/**
 * @brief   Evaluates an expression from a sequence of tokens.
 *
 * @param   preprocessor    A pointer to the TM32ASM_Preprocessor instance.
 * @param   tokenStream     The token stream containing the expression.
 * @param   startIndex      The starting index of the expression in the token stream.
 * @param   endIndex        The ending index of the expression in the token stream (exclusive).
 * @param   result          A pointer to store the evaluated result.
 *
 * @return  `true` if the expression was successfully evaluated; `false` otherwise.
 */
bool TM32ASM_EvaluateExpression (
    TM32ASM_Preprocessor*   preprocessor,
    TM32ASM_TokenStream*    tokenStream,
    size_t                  startIndex,
    size_t                  endIndex,
    int64_t*                result
);

#endif // TM32ASM_PREPROCESSOR_H
