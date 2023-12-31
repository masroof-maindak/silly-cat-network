#include <iostream>
#include "bTree.h"

int main() {
    bTree bruh("bruh");

    // for(char i = 'a'; i <= 'z'; i++) {
    //     std::string xyz = "";
    //     xyz += i;
    //     bruh.insert(xyz);
    // }

    bruh.insert("bingus");
    bruh.insert("freegoblinpics");
    bruh.insert("fatfatpankocat");
    bruh.insert("fatfatmillycat");
    bruh.insert("ganymedeshouse");
    bruh.insert("monkeycatluna");
    bruh.insert("oldboypeanut");
    bruh.insert("meevinofficial");
    bruh.insert("unico_uni");    
    bruh.insert("toletole");    
    bruh.insert("guangdang005");    
    bruh.insert("zazubabyman");    

    bruh.erase("bingus");
    bruh.print(); std::cout << "\n\n";

    return 0;
}