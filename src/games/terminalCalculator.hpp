#pragma once

#define ZBIT_SET_SIZE 1000000

#define ZMATH_ZINT
#define ZMATH_ZBIT
#include "zmath.hpp"

#include <string>
#include <iostream>

inline void Terminal() {
    std::cout << "\x1b[2J\x1b[H";
    std::cout << "\033[2J\033[H";
    std::cout.flush();

    unsigned char selectedOperation = 0;
    std::string operationType;
    std::string num1 = "0";
    std::string operation = "+";
    std::string operationStr;
    std::string num2 = "0";
    std::string num3 = "0";
    std::string answer = "0";

    while (true) {

        std::cout << "Select operation type: (type help for all commands): ";
        std::cin >> operationType;

        if (operationType == "help") {
            std::cout << "basic (+-/*%^)\n" << "advanced (sqrt, bitset, prime, factorial)\n" << "binary (&)\n";
            selectedOperation = 0;
        }
        else if (operationType == "basic") { selectedOperation = 1; }
        else if (operationType == "advanced") { selectedOperation = 2; }
        else if (operationType == "binary") { selectedOperation = 3; }
        else if (operationType == "clear") {
            std::cout << "\x1b[2J\x1b[H";
            std::cout << "\033[2J\033[H";
            std::cout.flush();
            break;
        }

        switch (selectedOperation) {
        case 1:
            std::cout << "Num1: ";
            std::cin >> num1;
            if (num1 == "answer" || num1 == "Answer" || num1 == "ans" || num1 == "Ans") { num1 = answer; }
            std::cout << std::endl << "Operation: ";
            std::cin >> operation;
            std::cout << std::endl << "Num2: ";
            std::cin >> num2;

            if (operation == "+") { answer = std::to_string(std::stof(num1) + std::stof(num2)); }
            else if (operation == "-") { answer = std::to_string(std::stof(num1) - std::stof(num2)); }
            else if (operation == "/") { answer = std::to_string(std::stof(num1) / std::stof(num2)); }
            else if (operation == "*") { answer = std::to_string(std::stof(num1) * std::stof(num2)); }
            else if (operation == "&") { answer = std::to_string(std::stoi(num1) % std::stoi(num2)); }
            else if (operation == "^") { answer = std::to_string(pow(std::stof(num1), std::stof(num2))); }
            else if (operation == "+b") {
                ZInt<128> bnum1 = std::stoi(num1);
                answer = (bnum1 + std::stoi(num2)).toString();
            }
            else if (operation == "-b") {
                ZInt<128> bnum1 = std::stoi(num1);
                answer = (bnum1 - std::stoi(num2)).toString();
            }
            else if (operation == "/b") {
                ZInt<128> bnum1 = std::stoi(num1);
                answer = (bnum1 / std::stoi(num2)).toString();
            }
            else if (operation == "*b") {
                ZInt<128> bnum1 = std::stoi(num1);
                answer = (bnum1 * std::stoi(num2)).toString();
            }
            else if (operation == "+bb") {
                ZInt<128> bnum1 = std::stoi(num1);
                ZInt<128> bnum2 = std::stoi(num2);
                answer = (bnum1 + bnum2).toString();
            }
            else if (operation == "-bb") {
                ZInt<128> bnum1 = std::stoi(num1);
                ZInt<128> bnum2 = std::stoi(num2);
                answer = (bnum1 - bnum2).toString();
            }
            else if (operation == "/bb") {
                ZInt<128> bnum1 = std::stoi(num1);
                ZInt<128> bnum2 = std::stoi(num2);
                answer = (bnum1 / bnum2).toString();
            }
            else if (operation == "*bb") {
                ZInt<128> bnum1 = std::stoi(num1);
                ZInt<128> bnum2 = std::stoi(num2);
                answer = (bnum1 * bnum2).toString();
            }

            std::cout << std::endl << "Answer: " << answer << std::endl;
            break;
        case 2:
            std::cout << "Num1: ";
            std::cin >> num1;
            if (num1 == "answer" || num1 == "Answer" || num1 == "ans" || num1 == "Ans") { num1 = answer; }
            std::cout << std::endl << "Operation: ";
            std::cin >> operationStr;

            if (operationStr == "factorial") { answer = factorial<32768>(std::stoi(num1)).toString(); }
            else if (operationStr == "superFactorial") { answer = superFactorial<16384>(std::stoi(num1)).toString(); }
            else if (operationStr == "expoFactorial") { answer = exponentFactorial<32768>(std::stoi(num1)).toString(); }
            else if (operationStr == "square root" || operationStr == "sqrt" || operationStr == "Square root" || operationStr == "squareroot") { answer = std::to_string(sqrt(std::stof(num1))); }
            else if (operationStr == "bitset") { answer = createRandomBitset<8000000>().string(); }
            else if (operationStr == "fahrenheit") { answer = std::to_string(fahrenheit(std::stoi(num1))); }
            else if (operationStr == "celsius") { answer = std::to_string(celsius(std::stoi(num1))); }
            else if (operationStr == "prime") {
                answer = prime(std::stoi(num1)) ? "Prime" : "Not prime";
            }
            else if (operationStr == "factor") {
                std::cout << "num2: ";
                std::cin >> num2;
                answer = findFactor(std::stoi(num1), std::stoi(num2));
            }
            std::cout << "Answer: " << answer << std::endl;
            break;
        case 3:
            if (operation == "&") { answer = std::to_string(std::stoi(num1) & std::stoi(num2)); }
            break;
        }
    }
}