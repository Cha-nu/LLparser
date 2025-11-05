#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <cstdio>

extern char lexeme[100];
extern char nextChar;
extern int lexLen;
extern int token;
extern int nextToken;
extern FILE* in_fp;

// 문자 유형 상수 (enum 사용)
enum CharClass {
    LETTER = 0,
    DIGIT = 1,
    UNKNOWN = 99
};

// 토큰 코드 상수 (enum 사용)
enum TokenCode {
    INT_LIT = 10,
    IDENT = 11,
    KEYWORD = 12,
    ASSIGN_OP = 20,
    ADD_OP = 21,
    SUB_OP = 22,
    MULT_OP = 23,
    DIV_OP = 24,
    LEFT_PAREN = 25,
    RIGHT_PAREN = 26,
    SEMI_COLON = 27
    // EOF는 <cstdio>에서 사용
};


// 함수 선언
void addChar();
void getChar();
void getNonBlank();
int lexical();
int lookup(char ch);

#endif // LEXICAL_ANALYZER_H
