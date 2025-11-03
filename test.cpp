#include <iostream>
#include <string>
#include <windows.h> // 활성화를 위해 포함

// (위에서 정의한 EnableVirtualTerminalProcessing() 함수가 있다고 가정)
void EnableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        return;
    }
}

// 사용하기 편하게 색상 코드를 상수로 정의
const char* const RESET = "\x1B[0m";
const char* const RED = "\x1B[31m";
const char* const BLUE = "\x1B[34m";
const char* const MAGENTA = "\x1B[35m"; // 보라색(마젠타)
const char* const GREEN = "\x1B[32m";

int main() {
    // 1. ANSI 코드 활성화
    EnableVirtualTerminalProcessing();

    // 2. 원하는 출력에 색상 적용
    // (사용자 예시 적용)

    std::cout << "operand1 := 3;" << std::endl;
    std::cout << BLUE << "ID: 1; CONST: 1; OP: 0;" << RESET << std::endl;
    std::cout << RED << "(OK)" << RESET << std::endl;

    std::cout << "operand2 := operand1 + 2;" << std::endl;
    std::cout << BLUE << "ID: 2; CONST: 1; OP: 1;" << RESET << std::endl;
    std::cout << RED << "(OK)" << RESET << std::endl;
    
    std::cout << "target := operand1 + operand2 * 3" << std::endl;
    std::cout << BLUE << "ID: 3; CONST: 1; OP: 2;" << RESET << std::endl;
    std::cout << RED << "(OK)" << RESET << std::endl;

    std::cout << MAGENTA << "Result ==> operand1: 3; operand2: 5; target: 18;" << RESET << std::endl;

    return 0;
}