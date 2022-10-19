#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void fControl(FILE *, FILE *);

void outputOperator(char *, FILE *);

int isOperator(char ch);

int isIdentifier(char *);

int isKeyword(char *);

int bracket(char);

char fpeek(FILE *);

int isEndofLine(char);

void identiferOutput(char *, FILE *);

int main() {
    char ch;

    int i = 0;

    FILE *fp, *fp2;
    fp = fopen("code.psi", "r");
    fp2 = fopen("code.lex", "w");

    // check if file is actually opened
    fControl(fp, fp2);

    // main loop to read the file
    while ((ch = fgetc(fp)) != EOF) {
        char *buffer = (char*) malloc(500);
        // INT CONST
        if (isdigit(ch) && (isEndofLine(fpeek(fp)) || isspace(fpeek(fp)) || isdigit(fpeek(fp)) || isOperator(fpeek(fp)))) {
            int flag = 0;
            i = 0;
            buffer[i] = ch;
            while ((ch = fgetc(fp)) != EOF) {
                char *buffer2[3] = {0};
                if (!isspace(ch)) {

                    if (!isOperator(ch)){
                        buffer[++i] = ch;

                    }else{
                        buffer2[0] = ch;
                        buffer[++i] = '\0';
                        fprintf(fp2, "IntConst(%s)\n", buffer);
                        buffer[0] = '\0';
                        outputOperator(buffer2, fp2);
                        break;
                    }
                }
                if (!isdigit(fpeek(fp))) {
                    if (isalpha(fpeek(fp))) {
                        flag = 1;
                        continue;
                    } else {
                        buffer[++i] = '\0';
                        break;
                    }
                }
            }

            if (i > 10) {
                fprintf(fp2, "ERROR: INTEGER CONSTANT is longer than 10 characters.\n");
            } else if (ch == EOF) {
                fprintf(fp2, "ERROR: END OF FILE.\n");
            }
            if (flag){
                identiferOutput(buffer, fp2);
            } else{
                if(buffer[0]!= '\0'){
                    fprintf(fp2, "IntConst(%s)\n", buffer);
                }
            }


        } else if (isalnum(ch) || ch == '_') {
            // IDENTIFIER OR KEYWORD
            for (int j = 0; j < sizeof(buffer); ++j) {
                buffer [j] = 0;
            }
            i = 0;
            buffer[i++] = ch;
            while ((ch = fgetc(fp)) != EOF) {
                if (!isalnum(ch) && ch != '_') {
                    ungetc(ch, fp);
                    buffer[i] = '\0';
                    break;
                }
                buffer[i++] = ch;
            }
            if (isKeyword(buffer)) {
                fprintf(fp2, "Keyword(%s)\n", buffer);

            } else {
                if (isdigit(buffer[0])&&buffer[1] =='\0'){
                    fprintf(fp2, "IntConst(%s)\n", buffer);
                }else{

                    identiferOutput(buffer, fp2);
                }

            }


        }
            // IF CH IS NOT ALPHANUMERIC
        else {
            // COMMENT
            if (ch == '(' && fpeek(fp) == '*') {


                fgetc(fp); // waste the star
                while ((ch = fgetc(fp)) != EOF) {
                    if (ch == '*' && fpeek(fp) == ')') {
                        fgetc(fp); // waste the right par
                        break;
                    }
                }
                if (ch == EOF) {
                    //RAISE AN ERROR comment not closed

                    perror("error: unterminated comment\n");
                    exit(1);
                }

            }

                // STRING CONST
            else if (ch == '"') {


                i = 0;
                while ((ch = fgetc(fp)) != EOF) {
                    buffer[i++] = ch;
                    if (fpeek(fp) == '"') {
                        fgetc(fp);
                        buffer[i] = '\0';
                        break;
                    }
                }

                if (ch == EOF) {
                    perror("warning: missing terminating \" character\n");
                    exit(1);
                }
                fprintf(fp2, "StringConst(%s)\n", buffer);


            }
                // OPERATOR
            else if (isOperator(ch)) {


                i = 0;
                buffer[i++] = ch;
                while (((ch = fgetc(fp)) != EOF)) {
                    if (!(isOperator(ch))) {
                        ungetc(ch, fp);
                        buffer[i] = '\0';
                        break;
                    }
                    buffer[i++] = ch;
                }


                outputOperator(buffer, fp2);


            }
                // END OF LINE
            else if (isEndofLine(ch)) {
                fprintf(fp2, "EndOfLine\n");
            }
                //Bracket
            else {
                int flag = bracket(ch);
                switch (flag) {
                    case 1:
                        fprintf(fp2, "LeftCurlyBracket\n");
                        break;
                    case 2:
                        fprintf(fp2, "RightCurlyBracket\n");
                        break;
                    case 3:
                        fprintf(fp2, "LeftSquareBracket\n");
                        break;
                    case 4:
                        fprintf(fp2, "RightSquareBracket\n");
                        break;
                    case 5:
                        fprintf(fp2, "LeftPar\n");
                        break;
                    case 6:
                        fprintf(fp2, "RightPar\n");
                    default:;
                }

            }
        }
        free(buffer);
    }
    fclose(fp);
    fclose(fp2);

    return 0;
}


void outputOperator(char *buffer, FILE *fp2) {
    char operators[7][3] = {"+", "-", "*", "/", "++", "--", ":="};
    for (int i = 0; i < 7; ++i) {
        if (strcmp(operators[i], buffer) == 0) {
            fprintf(fp2, "Operator(%s)\n", buffer);
            return;
        }
    }
    fprintf(fp2, "<%s>Undefined operator\n", buffer);
}

int isOperator(char ch) {
    char operators[6] = {'+', '-', '*', '/', ':', '='};
    for (int i = 0; i < 6; ++i) {
        if (ch == operators[i])
            return 1;
    }
    return 0;
}


void fControl(FILE *fp, FILE *fp2) {
    if (fp == NULL || fp2 == NULL) {
        perror("Error while opening the file\n");
        exit(1);
    }
}

char fpeek(FILE *stream) {
    char c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}


int bracket(char ch) {
    switch (ch) {
        case '{':
            return 1;
            break;
        case '}':
            return 2;
            break;
        case '[':
            return 3;
            break;
        case ']':
            return 4;
            break;
        case '(':
            return 5;
            break;
        case ')':
            return 6;
            break;
        default:
            return 0;
    }

}


int isIdentifier(char *str) {
    // returns 1 if str is an identifier
    char *p = NULL;

    // make the str all lowercase
    str = strlwr(str);

    // must start with alphabetic chars
    if (str[0] > 'z' || str[0] < 'a') {
        return 0;
    }
    // must be shorter than 20 characters
    if (strlen(str) > 20) {
        return -1;
    }
    return 1;
}

int isKeyword(char *str) {
    char keywords[18][9] = {"break", "case", "char", "const", "continue", "do", "else", "enum",
                            "float", "for", "goto", "if", "int", "long", "record", "return", "static", "while"};
    int i;
    char* temp = strlwr(str);
    for (i = 0; i < 18; ++i) {
        if (strcmp(keywords[i], temp) == 0) {
            return 1;

        }
    }
    return 0;
}

int isEndofLine(char ch) {
    if (ch == ';') {
        return 1;
    }
    return 0;
}

void identiferOutput(char *buffer, FILE *fp2) {
    int a = isIdentifier(buffer);
    switch (a) {
        case -1:
            fprintf(fp2, "ERROR: <%s> identifer is too long\n", buffer);
            break;
        case 0:
            fprintf(fp2, "ERROR: <%s> identifer is not starting with alphabetic character\n", buffer);
            break;
        case 1:
            fprintf(fp2, "Identifier(%s)\n", strlwr(buffer));
            break;
        default:;

    }
}

