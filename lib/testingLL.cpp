#include <iostream>
#include "LinkedList.h"

int main() {

    LinkedList<std::string> l ("testingLL.txt");

    std::cout << l[0];
    std::cout << l[1];
    std::cout << l[2];
    std::cout << l[3];

    std::cout << l;

    //testing deserialization

    return 0;
}
