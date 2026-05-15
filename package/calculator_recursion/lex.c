#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

static TokenSet getToken(void);
static TokenSet curToken = UNKNOWN;
static char lexeme[MAXLEN];

TokenSet getToken(void)
{
    int i = 0;
    char c = '\0';

    while ((c = fgetc(stdin)) == ' ' || c == '\t');

    if (isdigit(c)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while (isdigit(c) && i < MAXLEN) {
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        ungetc(c, stdin);
        lexeme[i] = '\0';
        return INT;
    } else if (c == '+') {
        char next = fgetc(stdin);
        if (next == '+') { // 辨識 ++
            strcpy(lexeme, "++");
            return INCDEC;
        } else if (next == '=') { // 辨識 +=
            strcpy(lexeme, "+=");
            return ADDSUB_ASSIGN;
        } else {
            ungetc(next, stdin);
            strcpy(lexeme, "+");
            return ADDSUB;
        }
    } else if (c == '-') {
        char next = fgetc(stdin);
        if (next == '-') { // 辨識 --
            strcpy(lexeme, "--");
            return INCDEC;
        } else if (next == '=') { // 辨識 -=
            strcpy(lexeme, "-=");
            return ADDSUB_ASSIGN;
        } else {
            ungetc(next, stdin);
            strcpy(lexeme, "-");
            return ADDSUB;
        }
    } else if (c == '*' || c == '/') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return MULDIV;
    } else if (c == '\n') {
        lexeme[0] = '\0';
        return END;
    } else if (c == '=') {
        strcpy(lexeme, "=");
        return ASSIGN;
    } else if (c == '(') {
        strcpy(lexeme, "(");
        return LPAREN;
    } else if (c == ')') {
        strcpy(lexeme, ")");
        return RPAREN;
    }else if (isalpha(c) || c == '_') {
        lexeme[0] = c;
        i = 1;
        c = fgetc(stdin);
        while ((isalnum(c) || c == '_') && i < MAXLEN) {
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        ungetc(c, stdin);
        lexeme[i] = '\0';
        return ID;
    } else if (c == '&') {
        strcpy(lexeme, "&");
        return AND;
    } else if (c == '|') {
        strcpy(lexeme, "|");
        return OR;
    } else if (c == '^') {
        strcpy(lexeme, "^");
        return XOR;
    }else if (c == EOF) {
        return ENDFILE;
    } else {
        return UNKNOWN;
    }
}

void advance(void) {
    curToken = getToken();
}

int match(TokenSet token) {
    if (curToken == UNKNOWN)
        advance();
    return token == curToken;
}

char *getLexeme(void) {
    return lexeme;
}
