#include "LexicalAnalyzer.h"
#include <iostream>
#include <unordered_set>
#include <string>
#include <cctype>

using namespace std;

int charClass = 0;
char lexeme[100];
char nextChar = ' ';
int lexLen = 0;
int token = 0;
int nextToken = 0;
FILE* in_fp = NULL;
bool hasDot = false;

static const unordered_set<string> C_KEYWORDS = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short",
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", "_Bool", "_Complex", "_Imaginary"
};

// lookup - 연산자와 괄호를 조사하여 그 토큰을 반환하는 함수
int lookup(char ch)
{
    switch (ch)
    {
    case '(':
        addChar();
        nextToken = LEFT_PAREN;
        break;
    case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        break;
    case '+':
        addChar();
        nextToken = ADD_OP;
        break;
    case '-':
        addChar();
        nextToken = SUB_OP;
        break;
    case '*':
        addChar();
        nextToken = MULT_OP;
        break;
    case '/':
        addChar();
        nextToken = DIV_OP;
        break;
    case ':': // ':' 문자를 먼저 처리
        addChar();
        getChar(); // 다음 문자가 '='인지 확인
        if (nextChar == '=')
        {
            addChar();
            nextToken = ASSIGN_OP; // := 대입 연산자 처리
        }
        break;
    case ';':
        addChar();
        nextToken = SEMI_COLON;
        break;

    // 알 수 없는 문자가 들어왔을 때 EOF가 아닌 UNKNOWN을 반환
    default:
        addChar();
        nextToken = UNKNOWN;
        break;
    }
    return nextToken;
}

// addchar - nextChar를 lexeme에 추가하는 함수
void addChar()
{
    if (lexLen <= 98)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
    {
        cout << "Error - lexeme is too long" << endl;
    }
}

// getChar - 입력으로부터 다음 문자를 가져와서 그 문자 유형을 결정하는 함수
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar) || nextChar == '_') {
            charClass = LETTER;
        }
        else if (isdigit(nextChar)) {
            charClass = DIGIT;
        }
        else {
            charClass = UNKNOWN;
        }

        if (nextChar != 10) {
            cout << nextChar;
        }

    }
    else
    {
        charClass = EOF;
    }
}

// getNonBlank - 비 공백 문자를 반환할 때까지 getChar를 호출하는 함수
void getNonBlank()
{
    while ((isspace(nextChar) || nextChar <= 32) && nextChar != EOF)
    { // EOF를 체크하여 무한 루프 방지
        getChar(); // 공백을 넘어갈 때 getChar 호출
    }
}

// lexical - 어휘 분석기
int lexical()
{
    lexLen = 0;
    getNonBlank(); // 공백 처리
    switch (charClass)
    {
        case LETTER: // 식별자 파싱 (원본과 동일)
        {
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT)
            {
                addChar();
                getChar();
            }
            string identifier = lexeme;
            if (C_KEYWORDS.find(identifier) != C_KEYWORDS.end()) {
                nextToken = KEYWORD;
            }
            else {
                nextToken = IDENT;
            }
            break;
        }

        case DIGIT: // 숫자 리터럴 파싱(정수/실수)
        {
            hasDot = false;
            addChar();
            getChar();
            while (true)
            {
               if (charClass == DIGIT)
               {
                addChar();
                getChar();
               }
               else if (!hasDot && nextChar == '.')
               {
                hasDot = true;
                addChar();
                getChar();

                if (charClass != DIGIT)
                {
               break;
                }
               }
               else
               {
                break;
               }
            }

            // 숫자로 시작했으나 뒤에 문자가 붙어 잘못된 식별자(예: 9id)가 된 경우
            if (charClass == LETTER) 
            {
                // "9" 까지 lexeme에 들어간 상태, nextChar는 "i"
                addChar(); // "i" 추가
                getChar(); // "d" 읽기

                // 나머지 문자/숫자를 모두 읽어서 하나의 UNKNOWN 토큰으로 만듦
                while (charClass == LETTER || charClass == DIGIT) {
                    addChar();
                    getChar();
                }
                nextToken = UNKNOWN; // 최종 토큰을 UNKNOWN으로 설정
            }
            else
            {
                // "9" 또는 "9.123" 처럼 올바른 숫자 리터럴인 경우
                nextToken = INT_LIT;
            }
            break;
        }

        case UNKNOWN: // 연산자와 괄호 파싱
            lookup(nextChar);
            getChar();
            break;

        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = '\0';
            break;
    }

    return nextToken;
}