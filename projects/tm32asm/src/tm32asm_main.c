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
            printf("[%zu] Type: %d, Lexeme: \"%s\", Line: %u\n", 
                   i, token->type, token->lexeme ? token->lexeme : "(null)", token->line);
        }
    }
    
    printf("\n=== END LEXICAL ANALYSIS ===\n");
    return true;
}

static void TM32ASM_PrintPreprocessorVariables (TM32ASM_Preprocessor* preprocessor)
{
    if (preprocessor == NULL)
    {
        return;
    }
    
    printf("=== PREPROCESSOR VARIABLES ===\n");
    
    // Get access to the preprocessor's symbol table
    size_t symbolCount = 0;
    const TM32ASM_Symbol* symbols = TM32ASM_GetSymbolTable(preprocessor, &symbolCount);
    
    if (symbols == NULL || symbolCount == 0)
    {
        printf("  No variables found.\n");
        printf("=== END PREPROCESSOR VARIABLES ===\n");
        return;
    }
    
    int foundCount = 0;
    
    // Iterate through all symbols in the preprocessor's symbol table
    for (size_t i = 0; i < symbolCount; i++)
    {
        const TM32ASM_Symbol* symbol = &symbols[i];
        
        // Only display variables and constants that are defined and resolved
        if (symbol->name != NULL && symbol->isDefined && symbol->isResolved &&
            (symbol->type == TM32ASM_ST_VARIABLE || symbol->type == TM32ASM_ST_CONSTANT))
        {
            TM32ASM_Value result;
            if (TM32ASM_ViewVariable(preprocessor, symbol->name, &result))
            {
                char* valueStr = TM32ASM_ValueToString(&result);
                const char* typeStr = "unknown";
                
                switch (result.type)
                {
                    case TM32ASM_VT_INTEGER:
                        typeStr = "integer";
                        break;
                    case TM32ASM_VT_FIXED_POINT:
                        typeStr = "fixed-point";
                        break;
                    case TM32ASM_VT_STRING:
                        typeStr = "string";
                        break;
                    case TM32ASM_VT_CHARACTER:
                        typeStr = "character";
                        break;
                }
                
                printf("  %-15s = %-15s (%s)\n", symbol->name, 
                       valueStr ? valueStr : "NULL", typeStr);
                
                if (valueStr) free(valueStr);
                TM32ASM_DestroyValue(&result);
                foundCount++;
            }
        }
    }
    
    if (foundCount == 0)
    {
        printf("  No variables found.\n");
    }
    else
    {
        printf("\nTotal variables found: %d\n", foundCount);
    }
    
    printf("=== END PREPROCESSOR VARIABLES ===\n");
}

static bool TM32ASM_WritePreprocessedTokensToFile (TM32ASM_TokenStream* tokenStream, const char* filename)
{
    TM32ASM_ReturnValueIfNull(tokenStream, false);
    TM32ASM_ReturnValueIfNull(filename, false);
    
    FILE* outputFile = fopen(filename, "w");
    if (outputFile == NULL)
    {
        TM32ASM_LogError("Failed to open output file: %s", filename);
        return false;
    }
    
    uint32_t currentLine = 0;
    const char* currentFilename = NULL;
    bool needsSpace = false;
    bool isStartOfLine = true;
    
    for (size_t i = 0; i < tokenStream->tokenCount; i++)
    {
        TM32ASM_Token* token = tokenStream->tokens[i];
        if (token != NULL && token->lexeme != NULL)
        {
            // Check if we need a newline based on line number changes
            bool needsNewline = false;
            if (token->line != currentLine || 
                (currentFilename != NULL && token->filename != NULL && strcmp(currentFilename, token->filename) != 0))
            {
                needsNewline = !isStartOfLine; // Don't add newline if already at start of line
                currentLine = token->line;
                currentFilename = token->filename;
            }
            
            // Add newline if needed (line changed)
            if (needsNewline)
            {
                fprintf(outputFile, "\n");
                isStartOfLine = true;
                needsSpace = false;
            }
            
            // Handle different token types with appropriate formatting
            switch (token->type)
            {
                case TM32ASM_TT_DIRECTIVE:
                    // Check if this directive should start on a new line. This
                    // will be the case for all assembler directives.
                    bool directiveNeedsNewline = false;
                    if (token->lexeme != NULL)
                    {
                        // These directives should typically start on their own line
                        if (strcmp(token->lexeme, ".db") == 0 || 
                            strcmp(token->lexeme, ".dw") == 0 || 
                            strcmp(token->lexeme, ".dd") == 0 ||
                            strcmp(token->lexeme, ".text") == 0 ||
                            strcmp(token->lexeme, ".data") == 0 ||
                            strcmp(token->lexeme, ".rodata") == 0 ||
                            strcmp(token->lexeme, ".bss") == 0)
                        {
                            directiveNeedsNewline = !isStartOfLine;
                        }
                    }
                    
                    // Add newline if this directive needs one
                    if (directiveNeedsNewline)
                    {
                        fprintf(outputFile, "\n");
                        isStartOfLine = true;
                        needsSpace = false;
                    }
                    
                    if (!isStartOfLine && needsSpace)
                    {
                        fprintf(outputFile, " ");
                    }
                    fprintf(outputFile, "%s", token->lexeme);
                    needsSpace = true;
                    isStartOfLine = false;
                    break;
                    
                case TM32ASM_TT_COLON:
                    fprintf(outputFile, "%s", token->lexeme);
                    needsSpace = false;
                    isStartOfLine = false;
                    break;
                    
                case TM32ASM_TT_COMMA:
                    fprintf(outputFile, "%s ", token->lexeme);
                    needsSpace = false;
                    isStartOfLine = false;
                    break;
                    
                case TM32ASM_TT_IDENTIFIER:
                    // Check if this is a label (followed by colon)
                    if (i + 1 < tokenStream->tokenCount && 
                        tokenStream->tokens[i + 1] != NULL && 
                        tokenStream->tokens[i + 1]->type == TM32ASM_TT_COLON)
                    {
                        // This is a label - start on new line if not already
                        if (!isStartOfLine)
                        {
                            fprintf(outputFile, "\n");
                            isStartOfLine = true;
                        }
                        fprintf(outputFile, "%s", token->lexeme);
                        needsSpace = false;
                        isStartOfLine = false;
                    }
                    else
                    {
                        // Regular identifier
                        if (needsSpace && !isStartOfLine)
                        {
                            fprintf(outputFile, " ");
                        }
                        fprintf(outputFile, "%s", token->lexeme);
                        needsSpace = true;
                        isStartOfLine = false;
                    }
                    break;
                    
                case TM32ASM_TT_INSTRUCTION:
                    // Instructions should be properly indented and start on new lines
                    if (!isStartOfLine)
                    {
                        fprintf(outputFile, "\n");
                        isStartOfLine = true;
                        needsSpace = false;
                    }
                    fprintf(outputFile, "    %s", token->lexeme); // 4-space indent
                    needsSpace = true;
                    isStartOfLine = false;
                    break;
                case TM32ASM_TT_REGISTER:
                case TM32ASM_TT_CONDITION:
                case TM32ASM_TT_BINARY:
                case TM32ASM_TT_OCTAL:
                case TM32ASM_TT_DECIMAL:
                case TM32ASM_TT_HEXADECIMAL:
                case TM32ASM_TT_FIXED_POINT:
                case TM32ASM_TT_CHARACTER:
                case TM32ASM_TT_STRING:
                    if (needsSpace && !isStartOfLine)
                    {
                        fprintf(outputFile, " ");
                    }
                    fprintf(outputFile, "%s", token->lexeme);
                    needsSpace = true;
                    isStartOfLine = false;
                    break;
                    
                default:
                    if (needsSpace && !isStartOfLine)
                    {
                        fprintf(outputFile, " ");
                    }
                    fprintf(outputFile, "%s", token->lexeme);
                    needsSpace = true;
                    isStartOfLine = false;
                    break;
            }
        }
    }
    
    // Add final newline
    if (!isStartOfLine)
    {
        fprintf(outputFile, "\n");
    }
    
    fclose(outputFile);
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
        // If --variables flag is also provided, list all preprocessor variables
        if (showVariables)
        {
            TM32ASM_PrintPreprocessorVariables(preprocessor);
        }
        // If --output-preprocessed is provided, write to file
        else if (outputPreprocessedFile != NULL)
        {
            if (!TM32ASM_WritePreprocessedTokensToFile(processedStream, outputPreprocessedFile))
            {
                TM32ASM_LogError("Failed to write preprocessed output to file: %s", outputPreprocessedFile);
                TM32ASM_DestroyTokenStream(processedStream);
                TM32ASM_DestroyPreprocessor(preprocessor);
                TM32ASM_DestroyLexer(lexer);
                return 1;
            }
            printf("Preprocessed source written to: %s\n", outputPreprocessedFile);
        }
        else
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
        }
        
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
