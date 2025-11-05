#include "Parser.h"
#include <iostream>

using namespace std;

// ID, CONST, OP 개수
int idCount = 0;
int constCount = 0;
int opCount = 0;

// 변수 초기화
void resetCounts() {
    idCount = 0;
    constCount = 0;
    opCount = 0;
}

// 트리를 순회하면서 노드 정보를 출력하는 함수
void printCounts() {
    cout << endl;
    cout << "ID: " << idCount << " CONST: " << constCount << " OP: " << opCount << endl;
}

// 심볼 테이블 정의
SymbolTable symTable;

// 파싱 트리 메모리 해제 함수
void freeTree(ParsingTreeNode* root) {
    if (!root) return;

    // 모든 자식 노드에 대해 재귀적으로 메모리 해제
    for (ParsingTreeNode* child : root->children) {
        freeTree(child);
    }

    delete root; // 현재 노드 메모리 해제
}

// <program> → <statements>
ParsingTreeNode* program() {
    ParsingTreeNode* node = new ParsingTreeNode(0);
    ParsingTreeNode* statementsNode = statements();
    node->addChild(statementsNode);
    symTable.print();
    return node;
}

// <statements> → <statement> { ; <statement> }
ParsingTreeNode* statements() {
    ParsingTreeNode* node = new ParsingTreeNode(0);

    // EOF (파일 끝)을 만날 때까지 파싱을 계속합니다.
    while (nextToken != EOF) { // (Lexer에 EOF_TOKEN이 정의되어 있어야 함)
        
        ParsingTreeNode* statementNode = statement();
        node->addChild(statementNode);

        // statement() 파싱 직후의 토큰을 확인합니다.
        
        if (nextToken == SEMI_COLON) {
            lexical(); // 세미콜론을 소모(consume)합니다.

            // 중복된 세미콜론 에러 처리
            while (nextToken == SEMI_COLON) {
                node->setStatus(ERROR, "(ERROR) \"중복 세미콜론(;) 발생\"");
                node->printMessage();
                lexical(); // 중복된 세미콜론 소모
            }
        }
        else if (nextToken == EOF) {
            // 파일의 끝이면 정상적으로 루프를 종료합니다.
            break;
        }
        else {
            // [핵심 오류 복구] ;도 EOF도 아닌 토큰 (ex: ')')을 만났을 때
            // 1. 오류를 보고
            string errorMsg = "(ERROR) \"세미콜론(;)이 예상된 위치에 예상치 못한 토큰 '" + (string)lexeme + "' 발견.\"";
            
            // statement() 자체에 에러가 없었다면(OK였다면),
            // 이 라인(statement)에 문법 오류 설정
            if (statementNode->status != ERROR) {
                statementNode->setStatus(ERROR, errorMsg);
            }
            
            // 2. 오류 복구: 다음 세미콜론(;)이나 EOF_TOKEN까지 토큰을 처리
            while (nextToken != SEMI_COLON && nextToken != EOF && nextToken != IDENT) {
                lexical(); // 토큰 소모
            }

            // 만약 세미콜론을 찾았다면, 다음 루프를 위해 소모
            if (nextToken == SEMI_COLON) {
                lexical(); 
            }
        }
    }

    return node;
}

// <statement> → <ident> <assignment_op> <expression>
ParsingTreeNode* statement() {
    ParsingTreeNode* node = new ParsingTreeNode(0);

    resetCounts();

    // IDENT 처리
    if (nextToken == IDENT) {
        idCount++;
        Symbol* symbol = symTable.findSymbol(lexeme);
        if (symbol == nullptr) {
            symbol = symTable.createSymbol(lexeme);
        }
        ParsingTreeNode* identNode = new ParsingTreeNode(IDENT);
        node->addChild(identNode);
        lexical();

        // ASSIGN_OP 처리
        if (nextToken == ASSIGN_OP) {
            node->addChild(new ParsingTreeNode(ASSIGN_OP));
            lexical();

            // 중복된 ASSIGN_OP 에러 처리
            while (nextToken == ASSIGN_OP) {
                node->setStatus(ERROR, "(ERROR) \"중복 배정 연산자(:=) 발생\"");
            }

            // 표현식 처리
            ParsingTreeNode* exprNode = expression();
            node->addChild(exprNode); // STATEMENT 노드에 expression 추가

            // [!!! 핵심 수정 사항 !!!]
            // expression()이 끝난 직후 토큰을 확인
            // 표현식 자체는 성공했더라도(isDefined), 뒤에 ;가 오지 않으면 문장(statement) 오류
            if (nextToken != SEMI_COLON && nextToken != EOF) {
                
                // (예: ')' 나 '+' 같은 예상치 못한 토큰)
                string errorMsg = "(ERROR) \"세미콜론(;)이 예상된 위치에 예상치 못한 토큰 '" + (string)lexeme + "' 발견.\"";
                
                // statement()가 이미 다른 에러를 가지고 있지 않다면, 이 에러를 설정
                if (node->status != ERROR) {
                    node->setStatus(ERROR, errorMsg);
                }

                // 오류가 발생했으므로, 이 표현식의 결과는 유효하지 않음(defined)으로 처리
                exprNode->isDefined = false;
            }

            if (exprNode->isDefined) {
                identNode->setValue(exprNode->value);
                symTable.updateSymbol(symbol->name, identNode->value);
            }
        }
        else {
            // ASSIGN_OP 누락 에러 처리
            node->setStatus(ERROR, "(ERROR) \"배정 연산자(:=)가 필요합니다.\"");
        }
    } 
    else {
        // 식별자 누락 에러 처리
        node->setStatus(ERROR, "(ERROR) \"식별자가 필요합니다.\"");
    }
    printCounts();
    node->printMessage();
    return node;
}

// <expression> → <term> <term_tail>
ParsingTreeNode* expression() {
    ParsingTreeNode* node = new ParsingTreeNode(0);

    ParsingTreeNode* termNode = term();
    node->addChild(termNode);
    ParsingTreeNode* termTailNode = term_tail(termNode->value);
    node->addChild(termTailNode);

    if (termNode->isDefined && termTailNode->isDefined) {
        node->setValue(termTailNode->value);
    }

    return node;
}

// <term> → <factor> <factor_tail>
ParsingTreeNode* term() {
    ParsingTreeNode* node = new ParsingTreeNode(0);

    ParsingTreeNode* factorNode = factor();
    node->addChild(factorNode);
    ParsingTreeNode* factorTailNode = factor_tail(factorNode->value);
    node->addChild(factorTailNode);

    if (factorNode->isDefined && factorTailNode->isDefined) {
        node->setValue(factorTailNode->value);
    }

    return node;
}

// <term_tail> → <add_op> <term> <term_tail> | ε
ParsingTreeNode* term_tail(int leftValue) {
    ParsingTreeNode* node = new ParsingTreeNode(0);
    if (nextToken == ADD_OP || nextToken == SUB_OP) {
        opCount++;
        int op = nextToken;
        string opName = lexeme;
        node->addChild(new ParsingTreeNode(nextToken));
        lexical();

        while (nextToken == ADD_OP || nextToken == SUB_OP || nextToken == MULT_OP || nextToken == DIV_OP) {
            if (op != nextToken) {
                string message = "(ERROR) \"서로 다른 연산자 " + opName + ", " + lexeme + " 사용\"";
                node->setStatus(ERROR, message);
                break;
            }
            else {
                string message = "(ERROR) \"중복 연산자(" + (string)lexeme + ") 발생\"";
                node->setStatus(ERROR, message);
            }
        lexical();
        }

        ParsingTreeNode* termNode = term();
        node->addChild(termNode);
        ParsingTreeNode* termTailNode = term_tail(termNode->value);
        node->addChild(termTailNode);

        if (termNode->isDefined && termTailNode->isDefined && node->status != -1) {
            if (op == ADD_OP) {
                node->setValue(leftValue + termTailNode->value);
            }
            else {
                node->setValue(leftValue - termTailNode->value);
            }
        }
    }
    else {
        node->setValue(leftValue);
    }
    return node;
}

// <factor> → <left_paren> <expression> <right_paren> | <ident> | <const>
ParsingTreeNode* factor() {
    ParsingTreeNode* node = new ParsingTreeNode(0);

    if (nextToken == LEFT_PAREN) {
        node->addChild(new ParsingTreeNode(LEFT_PAREN));
        lexical();  // '(' 처리
        
        ParsingTreeNode* expressionNode = expression();
        node->addChild(expressionNode);

        // [수정] 닫는 괄호가 일치하는지만 확인합니다.
        if (nextToken == RIGHT_PAREN) {
            node->addChild(new ParsingTreeNode(RIGHT_PAREN));
            lexical();  // ')' 처리

            // ( expr ) 까지 성공적으로 파싱됨.
            if (expressionNode->isDefined) {
                node->setValue(expressionNode->value);
            }
        }
        else {
            // ( expr 뒤에 ')'가 오지 않은 경우 (예: ( expr + 2)
            node->setStatus(ERROR, "(ERROR) \"닫는 괄호{)}가 필요합니다.\"");
            // 오류가 발생했으므로, 괄호 안의 expression 값은 사용하지 않습니다.
            // node->isDefined는 false로 유지됩니다.
            return node;
        }
    }
    else if (nextToken == IDENT) {
        idCount++;
        ParsingTreeNode* identNode = new ParsingTreeNode(IDENT);
        node->addChild(identNode);

        Symbol* symbol = symTable.findSymbol(lexeme);
        if (symbol == nullptr) {
            symTable.createSymbol(lexeme);
            string message = "(ERROR) \"정의되지 않은 변수(" + (string) lexeme + ")가 참조됨\"";
            node->setStatus(ERROR, message);
        }
        else if (symbol->isDefined) {
            identNode->setValue(symbol->value);
            node->setValue(identNode->value);
        }
        lexical();
    }
    else if (nextToken == INT_LIT) {
        constCount++;
        ParsingTreeNode* constNode = new ParsingTreeNode(INT_LIT);
        node->addChild(constNode);
        constNode->setValue(atoi(lexeme));
        node->setValue(constNode->value);

        lexical();
    }
    return node;
}

// <factor_tail> → <mult_op> <factor> <factor_tail> | ε
ParsingTreeNode* factor_tail(int leftValue) {
    ParsingTreeNode* node = new ParsingTreeNode(0);

    if (nextToken == MULT_OP || nextToken == DIV_OP) {
        opCount++;
        int op = nextToken;
        string opName = lexeme;
        node->addChild(new ParsingTreeNode(nextToken));
        lexical();

        while (nextToken == ADD_OP || nextToken == SUB_OP || nextToken == MULT_OP || nextToken == DIV_OP) {
            if (op != nextToken) {
                string message = "(ERROR) \"서로 다른 연산자 " + opName + ", " + lexeme + " 사용\"";
                node->setStatus(ERROR, message);
                break;
            }
            else {
                string message = "(ERROR) \"중복 연산자(" + (string)lexeme + ") 발생\"";
                node->setStatus(ERROR, message);
            }
            lexical();
        }

        ParsingTreeNode* factorNode = factor();
        node->addChild(factorNode);
        ParsingTreeNode* factorTailNode = factor_tail(factorNode->value);
        node->addChild(factorTailNode);

        if (factorNode->isDefined && factorTailNode->isDefined) {
            if (op == MULT_OP) {
                node->setValue(leftValue * factorTailNode->value);
            }
            else if (factorTailNode->value == 0)
            {
                node->setStatus(ERROR, "(ERROR) \"0으로는 나눌 수 없습니다.\"");
            }
        }
    }
    else {
        node->setValue(leftValue);
    }
    return node;
}