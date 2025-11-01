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
    ASSIGN_OP = 20,
    ADD_OP = 21,
    SUB_OP = 22,
    MULT_OP = 23,
    DIV_OP = 24,
    LEFT_PAREN = 25,
    RIGHT_PAREN = 26,
    SEMI_COLON = 27
    // EOF는 <cstdio>에 이미 정의된 매크로이므로, 
    // 충돌을 피하기 위해 다른 이름을 쓰거나 (예: EOF_TOKEN)
    // 파서에서만 사용하는 특별한 값(예: -1)을 사용할 수 있습니다.
    // 여기서는 기존 코드를 존중하여 lexical() 함수가 반환하는 값을 
    // parser.cpp에서 EOF(-1)와 비교한다고 가정하고 목록에서 제외할 수 있습니다.
};


// 함수 선언
void addChar();
void getChar();
void getNonBlank();
int lexical();
int lookup(char ch);

#endif // LEXICAL_ANALYZER_H