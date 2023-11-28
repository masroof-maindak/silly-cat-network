#include <iostream>

int main() {

    std::string inputLine;
    while (inputLine != "exit") {

        std::getline(std::cin, inputLine);
    }

    return 0;
}