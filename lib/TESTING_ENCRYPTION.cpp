#include <iostream>
#include "encryption.h"

int main() {
    std::string test = "username:freegoblinpics~!password:evilpass~";

    checkStringForEncryptables(test, 0);

    std::cout << test;

    return 0;
}