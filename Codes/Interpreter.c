#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VARIABLES 100
#define MAX_STRING_LENGTH 256
#define MAX_LINE_LENGTH 512
#define INITIAL_ARRAY_SIZE 10
#define LINE_BUFFER_SIZE 1024
#define MAX_INT_VALUE 99999999
#define IDENTIFIER_CHARACTER_LIMIT 10
#define INTEGER_LENGTH_LIMIT 8
#define STRING_LENGTH_LIMIT 256

typedef enum {
    IDENTIFIER,
    INT_CONST,
    OPERATOR,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    STRING_CONST,
    KEYWORD,
    END_OF_LINE,
    COMMA,
    COMMENT
} TokenType;

typedef struct {
    TokenType type;
    char value[999];
} Token;

typedef enum {
    MY_INT,
    MY_STR
} DataType;

typedef struct {
    char name[20];
    DataType type;
    union {
        int intValue;
        char text[MAX_STRING_LENGTH];
    };
} Variable;

typedef struct {
    char **data;
    size_t size;
    size_t capacity;
} DynamicArray;

TokenType isKeyword(char *word);
void executionPart(const char *inputFilename, const char *outputFilename);
void printToken(FILE *outputFile, TokenType type, const char *token);
void error(const char *message);
void interpreter(DynamicArray *arr, Variable listVar[], int varIndex);
int containsKeyword(char *keyword);
void extractString(char *str, char *result);
void trimQuotes(char *str, char *result);
void initArray(DynamicArray *arr);
void resizeArray(DynamicArray *arr);
void addElement(DynamicArray *arr, const char *element);
void freeArray(DynamicArray *arr);
void loadFromFile(DynamicArray *arr, const char *filename);
int doIntOperation(int int1, int int2, char *operationType);
void doTextOperation(char *result, const char *str1, const char *str2, const char *operationType);
char *delete_substring(char *str1, const char *str2);

int main() {
    FILE *inputFile, *outputFile, *lexicalAnalysis;
    char inputFileName[] = "test.sta";
    char outputFileName[] = "output.lex";
    char lexicalAnalysisFileName[] = "output.lex";
    executionPart(inputFileName, outputFileName);
    Variable listVar[MAX_VARIABLES];
    DynamicArray arr;
    int varIndex = 0;
    initArray(&arr);
    loadFromFile(&arr, "output.lex");
    interpreter(&arr, listVar, varIndex);
    freeArray(&arr);
    return 0;
}

TokenType isKeyword(char *word) {
    char *keywords[] = {"int", "text", "is", "loop", "times", "read", "write", "newLine"};
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return KEYWORD;
        }
    }
    return IDENTIFIER;
}

void executionPart(const char *inputFilename, const char *outputFilename) {
    FILE *inputFile = fopen(inputFilename, "r");
    FILE *outputFile = fopen(outputFilename, "w");
    if (!inputFile) {
        printf("Error: Input file could not be opened\n");
        exit(1);
    }
    if (!outputFile) {
        printf("Error: Output file could not be opened\n");
        exit(1);
    }
    int isComment = 0;
    int ch;
    char token[8191];
    int tokenIndex = 0;
    while ((ch = fgetc(inputFile)) != EOF) { 
        if (isComment) {
            if (ch == '*') {
                // End of comment line check
                int nextCh = fgetc(inputFile);
                if (nextCh == '/') {
                    isComment = 0;
                } else {
                    ungetc(nextCh, inputFile);
                }
            }
            continue;
        }
        if (isspace(ch)) { // Skip whitespaces
            continue;
        }
        if (ch == '/') { // Commentline check
            int nextCh = fgetc(inputFile);
            if (nextCh == '*') {
                isComment = 1;
                continue;
            } else {
                ungetc(nextCh, inputFile);
            }
        }
        if (ch == '-'){
            int nextCh = fgetc(inputFile);
            if (isdigit(nextCh)){  // Negative Value
                fprintf(outputFile, "IntConst(0)\n");
                while ((nextCh != '\n') && (!isspace(nextCh)) && (nextCh != '.') && nextCh != EOF && nextCh != '/' && nextCh != '+' && nextCh != '-' && nextCh != '*' && nextCh != ',' && nextCh != '{' && nextCh != '}'){ 
                    nextCh = fgetc(inputFile);
                    continue;
                }ch = nextCh;
                ungetc(nextCh,inputFile);
                continue;
            }else{
                ungetc(nextCh,inputFile);
            }
        }
        if (isdigit(ch)) { // Integer
            tokenIndex = 0;
            token[tokenIndex++] = ch;
            while ((ch = fgetc(inputFile)) != EOF && isdigit(ch)) {
                token[tokenIndex++] = ch;
            }
            if(!isdigit(ch) && ch != '\n' && ch != '.' && !isspace(ch) && ch != EOF && ch != '/' && ch != '+' && ch != '-' && ch != '*' && ch != ',' && ch != '{' && ch != '}'){
                error("Identifiers can not start with a number!");
                while ((ch != '\n') && (!isspace(ch)) && (ch != '.') && ch != EOF){
                    ch = fgetc(inputFile);
                    continue;
                }
            } else {
                token[tokenIndex] = '\0';
                if (tokenIndex <= MAX_INT_VALUE) {
                    printToken(outputFile, INT_CONST, token);
                } else {
                    error("You have reach integer character limit!");
                }
            }
            ungetc(ch, inputFile);
        } else if (isalpha(ch) || ch == '_') { // Identifier
            // Identifier or Keyword ??
            tokenIndex = 0;
            token[tokenIndex++] = ch;
            while ((ch = fgetc(inputFile)) != EOF && (isalnum(ch) || ch == '_')) {
                token[tokenIndex++] = ch;
            }
            token[tokenIndex] = '\0';
            ungetc(ch, inputFile); 
            TokenType type = isKeyword(token);
            if (type != KEYWORD) {
                type = IDENTIFIER;
            } if (tokenIndex <= IDENTIFIER_CHARACTER_LIMIT) {
                printToken(outputFile, type, token);
            } else{
                error("You have reached identifier character limit!");
            } 
        } else if (ch == '"') { // String
            tokenIndex = 0;
            token[tokenIndex++] = ch;
            while ((ch = fgetc(inputFile)) != EOF && ch != '"') {
                token[tokenIndex++] = ch; // Skip until second "
            }
            token[tokenIndex++] = '"';
            token[tokenIndex] = '\0';
            if (ch == '"' &&(tokenIndex<=STRING_LENGTH_LIMIT+2)) {
                printToken(outputFile, STRING_CONST, token);
            }  else if (ch != '"'){
                error("String formatting error!");
            } else if (ch == '"' && (tokenIndex > STRING_LENGTH_LIMIT+2)){
                error("You have reached string length limit!");
            }
        } else { // Operators, comma, EndOfLine
            token[0] = ch;
            token[1] = '\0';
            switch (ch) {
                case '{':
                    printToken(outputFile, LEFT_CURLY_BRACKET, token);
                    break;
                case '}':
                    printToken(outputFile, RIGHT_CURLY_BRACKET, token);
                    break;
                case '*': case '/': case '-': case '+':
                    printToken(outputFile, OPERATOR, token);
                    break;
                case '.':
                    printToken(outputFile, END_OF_LINE, token);
                    break;
                case ',':
                    printToken(outputFile, COMMA, token);
                    break;
                default:
                    error("Unkown token.");
                    break;
            }
        }
    }
    if(isComment){
        error("Unclosed comment line.");
    }
    fclose(inputFile);
    fclose(outputFile);
}

void printToken(FILE *outputFile, TokenType type, const char *token) { // output dosyasına yazdırma işlemleri
    switch (type) {
        case IDENTIFIER:
            fprintf(outputFile, "Identifier(%s)\n", token);
            break;
        case INT_CONST:
            fprintf(outputFile, "IntConst(%s)\n", token);
            break;
        case OPERATOR:
            fprintf(outputFile, "Operator(%s)\n", token);
            break;
        case LEFT_CURLY_BRACKET:
            fprintf(outputFile, "LeftCurlyBracket\n");
            break;
        case RIGHT_CURLY_BRACKET:
            fprintf(outputFile, "RightCurlyBracket\n");
            break;
        case STRING_CONST:
            fprintf(outputFile, "String(%s)\n", token);
            break;
        case KEYWORD:
            fprintf(outputFile, "Keyword(%s)\n", token);
            break;
        case END_OF_LINE:
            fprintf(outputFile, "EndOfLine\n");
            break;
        case COMMA:
            fprintf(outputFile, "Seperator\n");
            break;
        case COMMENT:
            break;
    }
}

void error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

int containsKeyword(char *line) {
    int i;
    char *keywords[] = {"int", "text", "loop", "read", "write", "newLine", "Identifier", "IntConst", "String", "Operator"};
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
    for (i = 0; i < numKeywords; i++) {
        if (strstr(line, keywords[i]) != NULL) {
            return i; // Returns the index of the keyword found
        }
        if (strcmp(line, "EndOfLine") == 0){
            break;
        }
    }
    return -1; // No keyword found
}

void extractString(char *str, char *result) {
    char *start = strchr(str, '('); // Find the opening parenthesis
    char *end = strchr(start, ')'); // Find the closing parenthesis
    // Copy the substring between the parentheses to the result
    strncpy(result, start + 1, end - start - 1);
    result[end - start - 1] = '\0'; // Null-terminate the result string
}

void trimQuotes(char *str, char *result) {
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        strncpy(result, str + 1, len - 2);
        result[len - 2] = '\0';
    } else {
        // If the string doesn't start and end with quotes, just copy it as is
        strcpy(result, str);
    }
}

int findVar(Variable listVar[], int varIndex, char textstr[]){
    int i;
    int isFound = 0;
    for (i = 0; i <= varIndex; i++) {
        if (strcmp(listVar[i].name, textstr) == 0) {
            isFound = 1;
            break; // Found the variable
        }
    }
    if (isFound == 1){
        return i;
    }else{
        error("Variable not found.");
    }
}

void interpreter(DynamicArray *arr, Variable listVar[], int varIndex){
    char textstr[999];
    int count = 0;
    int int1 = 0;
    int int2 = 0;
    char chr1[999];
    char chr2[999];
    char operationType[10];
    int i, j, z;
    char result[999];
    int loopCount;
    while (count < arr->size){
        int keywordIndex = containsKeyword(arr->data[count]);
        switch (keywordIndex) {
            case 0: // "int" keyword
                while (1) {
                    varIndex++;
                    extractString(arr->data[++count], textstr);
                    strcpy(listVar[varIndex].name, textstr);
                    listVar[varIndex].type = MY_INT;
                    listVar[varIndex].intValue = 0; // Default initialization
                    if (strcmp(arr->data[++count], "Keyword(is)") == 0) {
                        extractString(arr->data[++count], textstr); // IntConst(smth)
                        if (containsKeyword(arr->data[++count]) == 9){
                            int1 = atoi(textstr);
                            extractString(arr->data[count], operationType);
                            extractString(arr->data[++count], textstr);
                            int2 = atoi(textstr);
                            listVar[varIndex].intValue = doIntOperation(int1, int2, operationType);
                        }else{
                            count--;
                            listVar[varIndex].intValue = atoi(textstr);
                        }

                    } else {
                        count--;
                    }
                    if (count + 1 < arr->size && strcmp(arr->data[count + 1], "Seperator") == 0) {
                        count++;
                    } else {
                        break;
                    }
                }
                break;
            case 1: // "text" keyword
                while (1) {
                    varIndex++;
                    extractString(arr->data[++count], textstr);
                    strcpy(listVar[varIndex].name, textstr);
                    listVar[varIndex].type = MY_STR;
                    listVar[varIndex].text[0] = '\0'; // Default initialization

                    if (strcmp(arr->data[++count], "Keyword(is)") == 0) {
                        extractString(arr->data[++count], textstr);
                        trimQuotes(textstr, textstr);
                        strcpy(chr1, textstr);
                        if (containsKeyword(arr->data[++count]) == 9){
                            extractString(arr->data[count], operationType);
                            if (containsKeyword(arr->data[++count]) == 6){
                                extractString(arr->data[count], textstr);
                                i = findVar(listVar, varIndex, textstr);
                                strcpy(chr2, listVar[i].text);
                                doTextOperation(result, chr1, chr2, operationType);
                                strcpy(listVar[varIndex].text, result);
                            }else if (containsKeyword(arr->data[++count]) == 8){
                                extractString(arr->data[count], textstr);
                                trimQuotes(textstr, textstr);
                                strcpy(chr2, textstr);
                                doTextOperation(result, chr1, chr2, operationType);
                                strcpy(listVar[varIndex].text, result);
                            }
                        }else{
                            count--;
                            strcpy(listVar[varIndex].text, chr1);
                        }
                        
                    } else {
                        count--;
                    }
                    if (count + 1 < arr->size && strcmp(arr->data[count + 1], "Seperator") == 0) {
                        count++;
                    } else {
                        break;
                    }
                }
                break;
            case 2: // "loop" keyword
                if (containsKeyword(arr->data[++count]) == 6){ // Identifier(smth) times
                    extractString(arr->data[count], textstr);
                    i = findVar(listVar, varIndex, textstr);
                    loopCount = listVar[i].intValue;
                }else{ // IntConst(smth)
                    extractString(arr->data[count], textstr);
                    loopCount = atoi(textstr);
                }
                
                count += 2; // Skip "times"
                if (strcmp(arr->data[count], "LeftCurlyBracket") == 0) {
                    // Multi-line loop
                    DynamicArray loopBody;
                    initArray(&loopBody);
                    count++; // Skip LeftCurlyBracket
                    while (strcmp(arr->data[count], "RightCurlyBracket") != 0){
                        addElement(&loopBody, arr->data[count]);
                        count++;
                    }
                    for (int loopIndex = 0; loopIndex < loopCount; loopIndex++){
                        interpreter(&loopBody, listVar, varIndex);
                    }
                    freeArray(&loopBody);
                } else {
                    // One-liner loop
                    DynamicArray loopBody;
                    initArray(&loopBody);

                    while (count < arr->size && strcmp(arr->data[count], "EndOfLine") != 0) {
                        addElement(&loopBody, arr->data[count]);
                        count++;
                    }

                    for (int loopIndex = 0; loopIndex < loopCount; loopIndex++) {
                        interpreter(&loopBody, listVar, varIndex);
                    }
                    freeArray(&loopBody);
                }
                break;
            case 3: // "read" keyword 
                if (containsKeyword(arr->data[++count]) == 8){
                    extractString(arr->data[count], textstr);
                    trimQuotes(textstr, textstr);
                    count++; // Skip Seperator.
                    printf("%s", textstr);
                    extractString(arr->data[++count], textstr);
                    i = findVar(listVar, varIndex, textstr);
                    if (listVar[i].type == MY_INT){
                        scanf("%d", &listVar[i].intValue);
                    }
                    if (listVar[i].type == MY_STR){
                        scanf("%s", textstr);
                        strcpy(listVar[i].text, textstr);
                    }
                }
                break;
            case 4: // "write" keyword
                count++; // Move to the next element after "write"
                while (count < arr->size && strcmp(arr->data[count], "EndOfLine") != 0) {
                    if (containsKeyword(arr->data[count]) == 8) { // String constant
                        extractString(arr->data[count], textstr);
                        trimQuotes(textstr, textstr);
                        printf("%s", textstr);
                    } else if (containsKeyword(arr->data[count]) == 6) { // Identifier
                        extractString(arr->data[count], textstr);
                        i = findVar(listVar, varIndex, textstr);
                        if (listVar[i].type == MY_INT) {
                            printf("%d", listVar[i].intValue);
                        } else if (listVar[i].type == MY_STR) {
                            printf("%s", listVar[i].text);
                        }
                    }
                    count++;
                    if (count < arr->size && strcmp(arr->data[count], "Seperator") == 0) {
                        printf(" ");
                        count++;
                    }
                }
                break;
            case 5: // "newLine" keyword
                printf("\n");
                break;
            case 6: // "Identifier" keyword
                extractString(arr->data[count], textstr);
                i = findVar(listVar, varIndex, textstr);
                if (i <= varIndex && strcmp(arr->data[++count], "Keyword(is)") == 0){
                    int testNext = containsKeyword(arr->data[++count]);
                    if (testNext == 7){ // IntConst
                        extractString(arr->data[count], textstr);
                        int1 = atoi(textstr);
                        if(strcmp(arr->data[++count], "EndOfLine") != 0){
                            if (containsKeyword(arr->data[count]) == 9){
                                extractString(arr->data[count], operationType);
                                if (containsKeyword(arr->data[++count]) == 6){
                                    extractString(arr->data[count], textstr);
                                    j = findVar(listVar, varIndex, textstr);
                                    int2 = listVar[j].intValue;
                                    listVar[i].intValue = doIntOperation(int1, int2, operationType);
                                }else{
                                    extractString(arr->data[count], textstr);
                                    int2 = atoi(textstr);
                                    listVar[i].intValue = doIntOperation(int1, int2, operationType);
                                }
                            }
                        }else{
                            listVar[i].intValue = int1;
                        }
                    } else if (testNext == 8){ // String
                        extractString(arr->data[count], textstr);
                        trimQuotes(textstr, chr1);
                        if (strcmp(arr->data[++count], "EndOfLine") != 0){
                            extractString(arr->data[count], operationType);
                            if (containsKeyword(arr->data[++count]) == 6){ // String Operator Identifier
                                extractString(arr->data[count], textstr);
                                j = findVar(listVar, varIndex, textstr);
                                strcpy(chr2, listVar[j].text);
                                doTextOperation(result, chr1, chr2, operationType);
                                strcpy(listVar[i].text, result);
                            }else // String Operator String
                            {
                                extractString(arr->data[count], chr2);
                                doTextOperation(result, chr1, chr2, operationType);
                                strcpy(listVar[i].text, result);
                            }
                        }else{
                            strcpy(listVar[i].text, chr1);
                        }
                    } else if (testNext == 6){ //Identifier
                        extractString(arr->data[count], textstr);
                        j = findVar(listVar, varIndex, textstr);
                        if (listVar[j].type == MY_STR){
                            if (containsKeyword(arr->data[++count]) == 9){
                                strcpy(chr1, listVar[j].text);
                                extractString(arr->data[count], operationType);
                                if (containsKeyword(arr->data[++count]) == 6){
                                    extractString(arr->data[count], textstr);
                                    z = findVar(listVar, varIndex, textstr);
                                    strcpy(chr2, listVar[z].text);
                                    doTextOperation(result, chr1, chr2, operationType);
                                    strcpy(listVar[i].text, result);
                                }else{
                                    extractString(arr->data[count], textstr);
                                    trimQuotes(textstr, chr2);
                                    doTextOperation(result, chr1, chr2, operationType);
                                    strcpy(listVar[i].text, result);
                                }
                            }else{
                                strcpy(listVar[i].text, listVar[j].text);
                            }
                        }else{
                            if (containsKeyword(arr->data[++count]) == 9){
                                int1 = listVar[j].intValue;
                                extractString(arr->data[count], operationType);
                                if (containsKeyword(arr->data[++count]) == 6){
                                    extractString(arr->data[count], textstr);
                                    z = findVar(listVar, varIndex, textstr);
                                    int2 = listVar[z].intValue;
                                    listVar[i].intValue = doIntOperation(int1, int2, operationType);
                                }else{
                                    extractString(arr->data[count], textstr);
                                    int2 = atoi(textstr);
                                    listVar[i].intValue = doIntOperation(int1, int2, operationType);
                                }
                            }else{
                                listVar[i].intValue = listVar[j].intValue;
                            }
                        }
                    }
                }
                break;
            default:
                break;
        }
        count++;
    }
}


void initArray(DynamicArray *arr) {
    arr->data = (char **)malloc(1 * sizeof(char *));
    arr->size = 0;
    arr->capacity = 1;
    if (arr->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

void resizeArray(DynamicArray *arr) {
    arr->capacity *= 2;
    arr->data = (char **)realloc(arr->data, arr->capacity * sizeof(char *));
    if (arr->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}

void addElement(DynamicArray *arr, const char *element) {
    if (arr->size == arr->capacity) {
        resizeArray(arr);
    }
    arr->data[arr->size] = strdup(element);
    if (arr->data[arr->size] == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    arr->size++;
}

void freeArray(DynamicArray *arr) {
    for (size_t i = 0; i < arr->size; i++) {
        free(arr->data[i]);
    }
    free(arr->data);
}

void loadFromFile(DynamicArray *arr, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char buffer[LINE_BUFFER_SIZE];
    while (fgets(buffer, LINE_BUFFER_SIZE, file) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; // Remove newline character
        }
        addElement(arr, buffer);
    }

    fclose(file);
}

int doIntOperation(int int1, int int2, char *operationType){
    if (strcmp(operationType, "+") == 0){
        if ((int1 + int2) >  MAX_INT_VALUE){
            error("Max Int Value exceeded.");
        }else{
            return int1 + int2;
        }
    }else if (strcmp(operationType, "-") == 0)
    {
        int result = int1 - int2;
        return result < 0 ? 0 : result;
    }else if (strcmp(operationType, "/") == 0){
        if(int2 != 0){
            return int1 / int2;
        }else{
            error("Division by 0");
        }
    }else{
        if ((int1 * int2) > MAX_INT_VALUE){
            error("Max Int Value exceeded.");
        }else{
            return int1 * int2;
        }
    }
    
}

void doTextOperation(char *result, const char *str1, const char *str2, const char *operationType) {
    if (strcmp(operationType, "+") == 0) {
        strcat(result, str1);
        strcat(result, str2);
    } else if (strcmp(operationType, "-") == 0) {
        strcpy(result, str1);
        result = delete_substring(result, str2);
    }
}

char *delete_substring(char *str1, const char *str2) {
    char *found = strstr(str1, str2);
    if (found) {
        char *tail = found + strlen(str2);
        memmove(found, tail, strlen(tail) + 1);
    }
    return str1;
}