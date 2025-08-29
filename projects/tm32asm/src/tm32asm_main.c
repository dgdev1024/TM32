/**
 * @file    tm32asm_main.c
 * @author  Dennis W. Griffin <dgdev1024@gmail.com>
 * @date    2025/08/26
 * 
 * @brief   Main entry point for the TM32 Assembler Tool.
 */

/* Include Files **************************************************************/

#include <tm32asm_arguments.h>
#include <tm32asm_lexer.h>
#include <tm32asm_preprocessor.h>
#include <tm32asm_token_stream.h>

/* Private Function Prototypes ************************************************/

static void TM32ASM_AtExit ()
{
    TM32ASM_ReleaseArguments();
}

static void TM32ASM_PrintUsage (const char* programName)
{
    printf("Usage: %s [options] <source_file.asm> [-o <output_file.o>]\n", programName);
    printf("Options:\n");
    printf("  -l, --lex-only        Perform only lexical analysis\n");
    printf("  -r, --preprocess-only Perform lexical analysis and preprocessing only\n");
    printf("  -p, --parse-only      Perform lexical analysis, preprocessing and parsing only\n");
    printf("  -o, --output-file     Specify output file name\n");
    printf("  -v, --version         Display version information\n");
    printf("  -h, --help            Display this help message\n");
}

static void TM32ASM_PrintVersion ()
{
    printf("TM32 Assembler Tool v%d.%d.%d\n", 
           TM32ASM_VERSION_MAJOR, TM32ASM_VERSION_MINOR, TM32ASM_VERSION_PATCH);
}

static bool TM32ASM_PrintTokens (TM32ASM_TokenStream* tokenStream)
{
    TM32ASM_ReturnValueIfNull(tokenStream, false);
    
    printf("=== LEXICAL ANALYSIS RESULTS ===\n");
    printf("Total tokens: %zu\n\n", tokenStream->tokenCount);
    
    for (size_t i = 0; i < tokenStream->tokenCount; i++)
    {
        TM32ASM_Token* token = tokenStream->tokens[i];
        if (token != NULL)
        {
            printf("[%zu] Type: %d, Lexeme: \"%s\", Line: %u\n", 
                   i, token->type, token->lexeme ? token->lexeme : "(null)", token->line);
        }
    }
    
    printf("\n=== END LEXICAL ANALYSIS ===\n");
    return true;
}

/* Private Functions **********************************************************/

int main (int argc, const char** argv)
{
    atexit(TM32ASM_AtExit);
    TM32ASM_CaptureArguments(argc, argv);
    
    // Check for help or version
    if (TM32ASM_HasArgumentKey("help", 'h'))
    {
        TM32ASM_PrintUsage(argv[0]);
        return 0;
    }
    
    if (TM32ASM_HasArgumentKey("version", 'v'))
    {
        TM32ASM_PrintVersion();
        return 0;
    }
    
    // Check if we have at least one source file
    if (argc < 2)
    {
        TM32ASM_LogError("No source file provided");
        TM32ASM_PrintUsage(argv[0]);
        return 1;
    }
    
    const char* sourceFile = argv[argc - 1]; // Last argument is the source file
    bool lexOnly = TM32ASM_HasArgumentKey("lex-only", 'l') != 0;
    bool preprocessOnly = TM32ASM_HasArgumentKey("preprocess-only", 'r') != 0;
    bool parseOnly = TM32ASM_HasArgumentKey("parse-only", 'p') != 0;
    
    // Create lexer
    TM32ASM_Lexer* lexer = TM32ASM_CreateLexer();
    if (lexer == NULL)
    {
        TM32ASM_LogError("Failed to create lexer");
        return 1;
    }
    
    // Load source file
    if (!TM32ASM_LoadSourceFile(lexer, sourceFile))
    {
        TM32ASM_LogError("Failed to load source file: %s", sourceFile);
        TM32ASM_DestroyLexer(lexer);
        return 1;
    }
    
    // Tokenize source
    if (!TM32ASM_TokenizeSource(lexer))
    {
        TM32ASM_LogError("Failed to tokenize source file");
        TM32ASM_DestroyLexer(lexer);
        return 1;
    }

    // Get the token stream.
    TM32ASM_TokenStream* stream = TM32ASM_GetTokenStream(lexer);

    // If `--lex-only`, just print the lexed tokens and exit.
    if (lexOnly == true)
    {
        TM32ASM_PrintTokens(stream);
        TM32ASM_DestroyLexer(lexer);
        return 0;
    }
    
    // Create preprocessor
    TM32ASM_Preprocessor* preprocessor = TM32ASM_CreatePreprocessor();
    if (preprocessor == NULL)
    {
        TM32ASM_LogError("Failed to create preprocessor");
        TM32ASM_DestroyLexer(lexer);
        return 1;
    }
    
    // Process the token stream through the preprocessor
    TM32ASM_TokenStream* processedStream = TM32ASM_ProcessTokenStream(preprocessor, stream);
    if (processedStream == NULL)
    {
        TM32ASM_LogError("Failed to preprocess token stream");
        TM32ASM_DestroyPreprocessor(preprocessor);
        TM32ASM_DestroyLexer(lexer);
        return 1;
    }
    
    // If `--preprocess-only`, print the preprocessed tokens and exit.
    if (preprocessOnly == true)
    {
        printf("=== PREPROCESSING RESULTS ===\n");
        printf("Total tokens: %zu\n\n", processedStream->tokenCount);
        
        for (size_t i = 0; i < processedStream->tokenCount; i++)
        {
            TM32ASM_Token* token = processedStream->tokens[i];
            if (token != NULL)
            {
                printf("[%zu] Type: %d, Lexeme: \"%s\", Line: %u\n", 
                       i, token->type, token->lexeme, token->line);
            }
        }
        
        printf("\n=== END PREPROCESSING ===\n");
        TM32ASM_DestroyTokenStream(processedStream);
        TM32ASM_DestroyPreprocessor(preprocessor);
        TM32ASM_DestroyLexer(lexer);
        return 0;
    }
    
    TM32ASM_DestroyTokenStream(processedStream);
    TM32ASM_DestroyPreprocessor(preprocessor);
    TM32ASM_DestroyLexer(lexer);
    return 0;
}
