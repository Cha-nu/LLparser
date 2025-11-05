#ifndef PARSER_H
#define SYNTAX_ANALYZER_H

// 어휘 분석기에서 사용되는 함수 및 변수에 대한 참조를 포함
#include "LexicalAnalyzer.h"
#include <vector>
#include <iostream>
#include <windows.h>

using namespace std;

#define OK 1
#define WARNING -1

// 전역 변수로 ID, CONST, OP 개수 추적
extern int idCount;
extern int constCount;
extern int opCount;

// 색상 코드
const char* const RESET = "\x1B[0m";
const char* const RED = "\x1B[31m";
const char* const BLUE = "\x1B[34m";
const char* const MAGENTA = "\x1B[35m"; // 보라색(마젠타)
const char* const GREEN = "\x1B[32m";


// 심볼 구조체
struct Symbol
{
    string name;
    double value;
    bool isDefined;
};

// 심볼 테이블 구조체
struct SymbolTable
{
    Symbol symbols[100];
    int size;

    // 생성자 (크기를 0으로 초기화)
    SymbolTable() : size(0)
    {
        for (int i = 0; i < 100; i++) {
            symbols[i] = Symbol();  // 각 Symbol을 기본값으로 초기화
        }
    }

    // 심볼 생성 함수
    Symbol* createSymbol(const string& name)
    {
        symbols[size].name = name;
        symbols[size].value = 0.0;        // 기본값
        symbols[size].isDefined = false;
        return &symbols[size++];          // 새 심볼 반환 후 크기 증가
    }

    // 심볼 검색 함수
    Symbol* findSymbol(const string& name)
    {
        for (int i = 0; i < size; i++)
        {
            if (symbols[i].name == name)
            {
                return &symbols[i];
            }
        }
        return nullptr;
    }

    // 심볼 업데이트 함수
    void updateSymbol(const string& variable, double value)
    {
        Symbol* symbol = findSymbol(variable);
        if (symbol)
        {
            symbol->value = value;
            symbol->isDefined = true;
        }
    }

    // 심볼 테이블 출력 함수
    void print() const
    {
        cout << MAGENTA << "Result ==> ";
        for (int i = 0; i < size; i++)
        {
            cout << symbols[i].name << ": ";
            if (symbols[i].isDefined)
            {
                cout << symbols[i].value;
            }
            else
            {
                cout << "Unknown";
            }
            if (i < size - 1) cout << "; ";
        }
        cout << ";" << RESET << endl;
    }
};

struct ParsingTreeNode
{
    int token;                   // 토큰 유형
    double value;                // 노드의 값
    bool isDefined;              // 노드의 값이 초기화 되었는지 확인
    int status;                  // 노드의 상태 (OK, WARNING, ERROR 등)
    string message;              // 경고 및 오류 메시지 배열
    vector<ParsingTreeNode*> children;  // 모든 자식 노드를 관리하는 벡터

    // 생성자
    ParsingTreeNode(int token) : token(token), value(0.0), isDefined(false), status(OK), message("(OK)") {}

    // 자식 노드 추가 함수
    void addChild(ParsingTreeNode* childNode)
    {
        children.push_back(childNode);
        // 상태 및 메시지 업데이트
        setStatus(childNode->status, childNode->message);
    }

    // 값 설정 함수
    void setValue(double newValue)
    {
        value = newValue;
        isDefined = true;
    }

    // 상태 및 메시지 설정 함수
    void setStatus(int newStatus, const string& newMessage)
    {
        if (status > newStatus)
        {
            status = newStatus;
            message = newMessage;
        }
    }

    // 메시지 출력
    void printMessage() const
    {
        if (!message.empty()) {
            cout << RED << message << RESET << endl;
        }
    }
};

// 각 카운터 초기화
void resetCounts();
void EnableVirtualTerminalProcessing();

// 파싱 트리 관련 함수 선언
void freeTree(ParsingTreeNode* root); // 메모리 해제를 위한 함수

// 구문 분석 함수 선언
ParsingTreeNode* program();
ParsingTreeNode* statements();
ParsingTreeNode* statement();
ParsingTreeNode* expression();
ParsingTreeNode* term();
ParsingTreeNode* factor();
ParsingTreeNode* term_tail(double leftValue);
ParsingTreeNode* factor_tail(double leftValue);

#endif // SYNTAX_ANALYZER_H
