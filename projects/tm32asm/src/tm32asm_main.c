/**
 * @file    tm32asm_main.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Main entry point for the TM32 Assembler Tool.
 */

/* Include Files **************************************************************/

#include <tm32asm_common.h>
#include <tm32asm_arguments.h>
#include <tm32asm_lexer.h>
#include <tm32asm_token.h>

/* Private Functions **********************************************************/

/**
 * @brief   Displays usage information for the assembler.
 */
static void ShowUsage (const char* programName)
{
    printf("TM32 Assembler Tool\n");
    printf("Usage: %s [options] <input_file>\n\n", programName);
    printf("Options:\n");
    printf("  -h, --help          Show this help message\n");
    printf("  -o, --output FILE   Specify output file (default: input.tmo)\n");
    printf("  -v, --verbose       Enable verbose output\n");
    printf("  -t, --tokens        Show tokenization output\n");
    printf("\n");
}

/**
 * @brief   Gets a human-readable name for a token type.
 * 
 * @param   tokenType   The token type to get the name for.
 * @return  A string representation of the token type.
 */
static const char* GetTokenTypeName (TM32ASM_TokenType tokenType)
{
    switch (tokenType)
    {
        case TM32ASM_TT_DIRECTIVE: return "DIRECTIVE";
        case TM32ASM_TT_FUNCTION: return "FUNCTION";
        case TM32ASM_TT_REGISTER: return "REGISTER";
        case TM32ASM_TT_CONDITION: return "CONDITION";
        case TM32ASM_TT_INSTRUCTION: return "INSTRUCTION";
        case TM32ASM_TT_IDENTIFIER: return "IDENTIFIER";
        case TM32ASM_TT_BINARY: return "BINARY";
        case TM32ASM_TT_OCTAL: return "OCTAL";
        case TM32ASM_TT_DECIMAL: return "DECIMAL";
        case TM32ASM_TT_HEXADECIMAL: return "HEXADECIMAL";
        case TM32ASM_TT_FIXED_POINT: return "FIXED_POINT";
        case TM32ASM_TT_CHARACTER: return "CHARACTER";
        case TM32ASM_TT_PIXEL: return "PIXEL";
        case TM32ASM_TT_STRING: return "STRING";
        case TM32ASM_TT_PARAM_POSITIONAL: return "PARAM_POSITIONAL";
        case TM32ASM_TT_PARAM_MACRO_NAME: return "PARAM_MACRO_NAME";
        case TM32ASM_TT_PARAM_COUNT: return "PARAM_COUNT";
        case TM32ASM_TT_PARAM_LIST: return "PARAM_LIST";
        case TM32ASM_TT_PARAM_LIST_NOT_FIRST: return "PARAM_LIST_NOT_FIRST";
        case TM32ASM_TT_PARAM_MANGLE_UNIQUE: return "PARAM_MANGLE_UNIQUE";
        case TM32ASM_TT_PLUS: return "PLUS";
        case TM32ASM_TT_MINUS: return "MINUS";
        case TM32ASM_TT_TIMES: return "TIMES";
        case TM32ASM_TT_EXPONENT: return "EXPONENT";
        case TM32ASM_TT_DIVIDE: return "DIVIDE";
        case TM32ASM_TT_MODULO: return "MODULO";
        case TM32ASM_TT_BITWISE_AND: return "BITWISE_AND";
        case TM32ASM_TT_BITWISE_OR: return "BITWISE_OR";
        case TM32ASM_TT_BITWISE_XOR: return "BITWISE_XOR";
        case TM32ASM_TT_BITWISE_NOT: return "BITWISE_NOT";
        case TM32ASM_TT_LEFT_SHIFT: return "LEFT_SHIFT";
        case TM32ASM_TT_RIGHT_SHIFT: return "RIGHT_SHIFT";
        case TM32ASM_TT_ASSIGN_EQUAL: return "ASSIGN_EQUAL";
        case TM32ASM_TT_ASSIGN_PLUS: return "ASSIGN_PLUS";
        case TM32ASM_TT_ASSIGN_MINUS: return "ASSIGN_MINUS";
        case TM32ASM_TT_ASSIGN_TIMES: return "ASSIGN_TIMES";
        case TM32ASM_TT_ASSIGN_EXPONENT: return "ASSIGN_EXPONENT";
        case TM32ASM_TT_ASSIGN_DIVIDE: return "ASSIGN_DIVIDE";
        case TM32ASM_TT_ASSIGN_MODULO: return "ASSIGN_MODULO";
        case TM32ASM_TT_ASSIGN_BITWISE_AND: return "ASSIGN_BITWISE_AND";
        case TM32ASM_TT_ASSIGN_BITWISE_OR: return "ASSIGN_BITWISE_OR";
        case TM32ASM_TT_ASSIGN_BITWISE_XOR: return "ASSIGN_BITWISE_XOR";
        case TM32ASM_TT_ASSIGN_LEFT_SHIFT: return "ASSIGN_LEFT_SHIFT";
        case TM32ASM_TT_ASSIGN_RIGHT_SHIFT: return "ASSIGN_RIGHT_SHIFT";
        case TM32ASM_TT_COMPARE_EQUAL: return "COMPARE_EQUAL";
        case TM32ASM_TT_COMPARE_NOT_EQUAL: return "COMPARE_NOT_EQUAL";
        case TM32ASM_TT_COMPARE_LESS: return "COMPARE_LESS";
        case TM32ASM_TT_COMPARE_LESS_EQUAL: return "COMPARE_LESS_EQUAL";
        case TM32ASM_TT_COMPARE_GREATER: return "COMPARE_GREATER";
        case TM32ASM_TT_COMPARE_GREATER_EQUAL: return "COMPARE_GREATER_EQUAL";
        case TM32ASM_TT_LOGICAL_AND: return "LOGICAL_AND";
        case TM32ASM_TT_LOGICAL_OR: return "LOGICAL_OR";
        case TM32ASM_TT_LOGICAL_NOT: return "LOGICAL_NOT";
        case TM32ASM_TT_OPEN_PARENTHESIS: return "OPEN_PARENTHESIS";
        case TM32ASM_TT_CLOSE_PARENTHESIS: return "CLOSE_PARENTHESIS";
        case TM32ASM_TT_OPEN_BRACKET: return "OPEN_BRACKET";
        case TM32ASM_TT_CLOSE_BRACKET: return "CLOSE_BRACKET";
        case TM32ASM_TT_OPEN_BRACE: return "OPEN_BRACE";
        case TM32ASM_TT_CLOSE_BRACE: return "CLOSE_BRACE";
        case TM32ASM_TT_COMMA: return "COMMA";
        case TM32ASM_TT_COLON: return "COLON";
        case TM32ASM_TT_AT: return "AT";
        default: return "UNKNOWN";
    }
}

/**
 * @brief   Processes a source file with the lexer.
 * 
 * @param   filename        The name of the file to process.
 * @param   showTokens      Whether to display token information.
 * @param   verbose         Whether to show verbose output.
 * @return  TRUE on success, FALSE on failure.
 */
static bool ProcessSourceFile (
    const char* filename,
    bool   showTokens,
    bool   verbose
)
{
    if (verbose)
    {
        printf("Processing file: %s\n", filename);
    }

    // Read the source file
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file '%s'\n", filename);
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer and read content
    char* sourceCode = TM32ASM_Create(fileSize + 1, char);
    if (sourceCode == NULL)
    {
        printf("Error: Could not allocate memory for file content\n");
        fclose(file);
        return false;
    }

    size_t bytesRead = fread(sourceCode, 1, fileSize, file);
    sourceCode[bytesRead] = '\0';
    fclose(file);

    if (verbose)
    {
        printf("Read %zu bytes from file\n", bytesRead);
    }

    // Create and initialize lexer
    TM32ASM_Lexer* lexer = TM32ASM_CreateLexer();
    if (lexer == NULL)
    {
        printf("Error: Could not create lexer\n");
        TM32ASM_Destroy(sourceCode);
        return false;
    }

    if (!TM32ASM_LoadSourceString(lexer, sourceCode, filename))
    {
        printf("Error: Could not load source into lexer\n");
        TM32ASM_DestroyLexer(lexer);
        TM32ASM_Destroy(sourceCode);
        return false;
    }

    // Tokenize the source
    int tokenCount = 0;
    TM32ASM_Token* token;

    if (showTokens)
    {
        printf("\nTokens:\n");
        printf("=======\n");
    }

    while ((token = TM32ASM_NextToken(lexer)) != NULL)
    {
        tokenCount++;

        if (showTokens)
        {
            printf("%3d: %-15s \"%s\" (line %d)\n",
                   tokenCount,
                   GetTokenTypeName(token->type),
                   token->lexeme,
                   token->line);
        }

        TM32ASM_DestroyToken(token);
    }

    if (verbose || showTokens)
    {
        printf("\nTotal tokens: %d\n", tokenCount);
    }

    // Cleanup
    TM32ASM_DestroyLexer(lexer);
    TM32ASM_Destroy(sourceCode);

    if (verbose)
    {
        printf("Lexical analysis completed successfully\n");
    }

    return true;
}

/* Public Functions ***********************************************************/

int main (int argc, char** argv)
{
    // Capture command-line arguments
    TM32ASM_CaptureArguments(argc, (const char**)argv);

    // Check for help option
    if (TM32ASM_HasArgumentKey("help", 'h'))
    {
        ShowUsage(argv[0]);
        TM32ASM_ReleaseArguments();
        return 0;
    }

    // Get input file (first positional argument)
    const char* inputFile = TM32ASM_GetPositionalArgument(0);

    if (inputFile == NULL)
    {
        printf("Error: No input file specified\n\n");
        ShowUsage(argv[0]);
        TM32ASM_ReleaseArguments();
        return 1;
    }

    // Parse options
    bool verbose = TM32ASM_HasArgumentKey("verbose", 'v') != 0;
    bool showTokens = TM32ASM_HasArgumentKey("tokens", 't') != 0;
    const char* outputFile = TM32ASM_GetArgumentValue("output", 'o', 0);

    if (verbose)
    {
        printf("TM32 Assembler Tool - Lexical Analysis Phase\n");
        printf("============================================\n");
        printf("Input file: %s\n", inputFile);
        if (outputFile)
        {
            printf("Output file: %s\n", outputFile);
        }
        printf("\n");
    }

    // Process the source file
    bool success = ProcessSourceFile(inputFile, showTokens, verbose);

    // Cleanup and return
    TM32ASM_ReleaseArguments();
    return success ? 0 : 1;
}
