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
    printf("  -l, --lex-only                    Perform only lexical analysis\n");
    printf("  -r, --preprocess-only             Perform lexical analysis and preprocessing only\n");
    printf("  -p, --parse-only                  Perform lexical analysis, preprocessing and parsing only\n");
    printf("      --variables                   List all preprocessor variables (use with --preprocess-only)\n");
    printf("      --output-preprocessed <file>  Write preprocessed source to file (use with --preprocess-only)\n");
    printf("  -o, --output-file                 Specify output file name\n");
    printf("  -v, --version                     Display version information\n");
    printf("  -h, --help                        Display this help message\n");
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
            if (token->lexeme != NULL)
            {
                printf("[%zu] Type: %d, Param: 0x%04X, Lexeme: \"%s\", Line: %u\n", 
                       i, token->type, token->param, token->lexeme, token->line);
            }
            else
            {
                printf("[%zu] Type: %d, Param: 0x%04X, Line: %u\n", 
                       i, token->type, token->param, token->line);
            }
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
    bool showVariables = TM32ASM_HasArgumentKey("variables", '\0') != 0;
    const char* outputPreprocessedFile = TM32ASM_GetArgumentValue("output-preprocessed", '\0', 0);
    
    // Validate --output-preprocessed option usage
    if (outputPreprocessedFile != NULL && !preprocessOnly)
    {
        TM32ASM_LogError("--output-preprocessed can only be used with --preprocess-only");
        return 1;
    }
    
    if (outputPreprocessedFile != NULL && showVariables)
    {
        TM32ASM_LogError("--output-preprocessed cannot be used with --variables");
        return 1;
    }
    
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
    
    // Set preprocessor options
    bool verbose = TM32ASM_HasArgumentKey("verbose", 'V') != 0;
    bool warningsAsErrors = TM32ASM_HasArgumentKey("warnings-as-errors", 'W') != 0;
    TM32ASM_SetPreprocessorOptions(preprocessor, warningsAsErrors, verbose);
    
    // Add the source file's directory as an include path
    const char* lastSlash = strrchr(sourceFile, '/');
    const char* lastBackslash = strrchr(sourceFile, '\\');
    const char* dirEnd = (lastSlash > lastBackslash) ? lastSlash : lastBackslash;
    
    if (dirEnd != NULL)
    {
        // Source file is in a directory, add that directory as include path
        size_t dirLen = dirEnd - sourceFile;
        char* sourceDir = malloc(dirLen + 1);
        if (sourceDir != NULL)
        {
            strncpy(sourceDir, sourceFile, dirLen);
            sourceDir[dirLen] = '\0';
            
        if (!TM32ASM_AddIncludePath(preprocessor, sourceDir))
        {
            TM32ASM_LogWarn("Could not add source directory '%s' as include path", sourceDir);
        }            free(sourceDir);
        }
    }
    else
    {
        // Source file is in current directory, add current directory as include path
        if (!TM32ASM_AddIncludePath(preprocessor, "."))
        {
            TM32ASM_LogWarn("Could not add current directory as include path");
        }
    }
    
    // Set input token stream for preprocessing
    if (!TM32ASM_SetInputTokenStream(preprocessor, stream))
    {
        TM32ASM_LogError("Failed to set input token stream for preprocessor");
        TM32ASM_DestroyPreprocessor(preprocessor);
        TM32ASM_DestroyLexer(lexer);
        return 1;
    }
    
    // Transfer ownership of token stream to preprocessor
    // The lexer should no longer try to destroy it
    lexer->tokenStream = NULL;
    
    // Process token stream through preprocessor
    if (!TM32ASM_ProcessTokenStream(preprocessor))
    {
        TM32ASM_LogError("Preprocessing failed");
        TM32ASM_DestroyPreprocessor(preprocessor);
        TM32ASM_DestroyLexer(lexer);
        return 1;
    }
    
    // Get preprocessed token stream
    TM32ASM_TokenStream* preprocessedStream = TM32ASM_GetOutputTokenStream(preprocessor);
    if (preprocessedStream == NULL)
    {
        TM32ASM_LogError("Failed to get preprocessed token stream");
        TM32ASM_DestroyPreprocessor(preprocessor);
        TM32ASM_DestroyLexer(lexer);
        return 1;
    }
    
    // If `--preprocess-only`, output preprocessed tokens and exit
    if (preprocessOnly)
    {
        printf("=== PREPROCESSED TOKEN STREAM ===\n");
        TM32ASM_PrintTokens(preprocessedStream);
        printf("=== END PREPROCESSED TOKENS ===\n");
        
        // TODO: Handle --output-preprocessed and --variables options
        
        TM32ASM_DestroyLexer(lexer);
        TM32ASM_DestroyPreprocessor(preprocessor);
        return 0;
    }
    
    // Clean up
    TM32ASM_DestroyLexer(lexer);
    TM32ASM_DestroyPreprocessor(preprocessor);
    return 0;
}
