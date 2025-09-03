#include "terminalCalculator.hpp"
#include "../../deps/ZDev/math.hpp"

#include <thread>
#include <string>
#include <iostream>

std::string operationType = "";
unsigned char selectedOperation = 0;
std::string num1 = "0";
char operation = '+';
std::string operationStr = "";
std::string num2 = "0";
std::string num3 = "0";
std::string answer = "0";

TerminalCalculator::TerminalCalculator() {
    std::thread run(&TerminalCalculator::terminalCalculatorRun, this);
    run.detach();
}

void TerminalCalculator::terminalCalculatorRun() {
    std::cout << "\x1b[2J\x1b[H";
    std::cout << "\033[2J\033[H";
    std::cout.flush();

    while (true) {
        std::cout << "Select operation type: (type help for all commands): ";
        std::cin >> operationType;

        if (operationType == "help") {
            std::cout << "basic (+-/*%^)\n" << "advanced (sqrt, lerp, factorial)\n" << "binary (&)\n";
            selectedOperation = 0;
        }
        else if (operationType == "basic") { selectedOperation = 1; }
        else if (operationType == "advanced") { selectedOperation = 2; }
        else if (operationType == "binary") { selectedOperation = 3; }

        switch (selectedOperation) {
        case 1:
            std::cout << "Num1: ";
            std::cin >> num1;
            if (num1 == "answer" || num1 == "Answer" || num1 == "ans" || num1 == "Ans") { num1 = answer; }
            std::cout << std::endl << "Operation: ";
            std::cin >> operation;
            std::cout << std::endl << "Num2: ";
            std::cin >> num2;

            switch (operation) {
            case '+': answer = std::to_string(std::stof(num1) + std::stof(num2)); break;
            case '-': answer = std::to_string(std::stof(num1) - std::stof(num2)); break;
            case '/': answer = std::to_string(std::stof(num1) / std::stof(num2)); break;
            case '*': answer = std::to_string(std::stof(num1) * std::stof(num2)); break;
            case '%': answer = std::to_string(std::stoi(num1) % std::stoi(num2)); break;
            case '^': answer = std::to_string(pow(std::stof(num1), std::stof(num2))); break;
            }

            std::cout << std::endl << "Answer: " << answer << std::endl;
            break;
        case 2:
            std::cout << "Num1: ";
            std::cin >> num1;
            if (num1 == "answer" || num1 == "Answer" || num1 == "ans" || num1 == "Ans") { num1 = answer; }
            std::cout << std::endl << "Operation: ";
            std::cin >> operationStr;

            if (operationStr == "factorial") { answer = bigInts::toString8192(bigInts::uint8192(std::stoi(num1)) ! std::stoi(num1)); }
            else if (operationStr == "square root" || operationStr == "sqrt" || operationStr == "Square root" || operationStr == "squareroot") { answer = std::to_string(std::sqrt(std::stof(num1))); }
            else if (operationStr == "lerp") {
                std::cout << "\nsmoothStep\nsmootherStep\nlerp\neaseInSine\neaseOutSine\neaseInOutSine\neaseInExpo\neaseOutExpo\neaseInOutExpo\neaseInCirc\neaseOutCirc\neaseInOutCirc\neaseOutBounce\n\nChoose one: ";
                std::cin >> operationStr;
                if (operationStr == "smoothStep") {
                    std::cout << "Num2: ";
                    std::cin >> num2;
                    std::cout << "Num3: ";
                    std::cin >> num3;
                    answer = smoothStep(std::stof(num1), std::stof(num2), std::stof(num3));
                }
                else if (operationStr == "smootherStep") {
                    std::cout << "Num2: ";
                    std::cin >> num2;
                    std::cout << "Num3: ";
                    std::cin >> num3;
                    answer = smootherStep(std::stof(num1), std::stof(num2), std::stof(num3));
                }
                else if (operationStr == "lerp") {
                    std::cout << "Num2: ";
                    std::cin >> num2;
                    std::cout << "Num3: ";
                    std::cin >> num3;
                    answer = linearInterpolate(std::stof(num1), std::stof(num2), std::stof(num3));
                }
                else if (operationStr == "easeInSine") { answer = std::to_string(easeInSine(std::stof(num1))); }
                else if (operationStr == "easeOutSine") { answer = std::to_string(easeOutSine(std::stof(num1))); }
                else if (operationStr == "easeInOutSine") { answer = std::to_string(easeInOutSine(std::stof(num1))); }
                else if (operationStr == "easeInExpo") { answer = std::to_string(easeInExpo(std::stof(num1))); }
                else if (operationStr == "easeOutExpo") { answer = std::to_string(easeOutExpo(std::stof(num1))); }
                else if (operationStr == "easeInOutExpo") { answer = std::to_string(easeInOutExpo(std::stof(num1))); }
                else if (operationStr == "easeInCirc") { answer = std::to_string(easeInCirc(std::stof(num1))); }
                else if (operationStr == "easeOutCirc") { answer = std::to_string(easeOutCirc(std::stof(num1))); }
                else if (operationStr == "easeInOutCirc") { answer = std::to_string(easeInOutCirc(std::stof(num1))); }
                else if (operationStr == "easeOutBounce") { answer = std::to_string(easeOutBounce(std::stof(num1))); }
            }
            else if (operationStr == "+128") { 
                std::cout << "Num2: ";
                std::cin >> num2;
                answer = bigInts::toString128(bigInts::uint128_t(std::stoi(num1)) + bigInts::uint128_t(std::stoi(num2)));
            }
            std::cout << "Answer: " << answer << std::endl;
            break;
        case 3:
            switch (operation) {
            case '&': answer = std::to_string(std::stoi(num1) & std::stoi(num2)); break;
            }
            break;
        }
    }
}