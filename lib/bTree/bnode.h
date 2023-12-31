#ifndef BNODE_H
#define BNODE_H

#include <iostream>
#include <algorithm>
#include <vector>

struct bnode {

    bool isLeaf;                   // is this bnode a leaf?
    int uniqueID;                  // to store a bnode's identifier
    std::vector<int> ptrs;         // array of uniqueIDs of children
    std::vector<std::string> keys; // array of values

    // Constructors
    bnode(int rootID, bool leaf);  // default
    bnode(std::string filename);   // generate node from file
    
    void writeToFile(std::string dir);  // serialize the bnode to a file
    void addKey(std::string data);      // insert key into bnode
};

// insert a value into a bnode
void bnode::addKey(std::string data) {
    keys.push_back(data);
    std::sort(keys.begin(), keys.end());    
}

// serialize the bnode to a file
void bnode::writeToFile(std::string dir) {

    std::string filename = dir + std::to_string(uniqueID) + ".txt";
    std::ofstream file; 
    file.open(filename);

    if (file.is_open()) {
        file << isLeaf << "\n" << uniqueID << "\n";

        // numKeys
        file << keys.size() << " ";
        for (int i = 0; i < keys.size(); i++)
            file << keys[i] << " ";

        //numPtrs
        file << "\n" << ptrs.size() << " ";
        for (int i = 0; i < ptrs.size(); i++)
            file << ptrs[i] << " ";

        file.close();
    } else
        std::cout << "Error: file " << filename << " not found for writing." << "\n";
}

// default constructor
bnode::bnode(int rootID, bool leaf) : isLeaf(leaf), uniqueID(rootID) {} 

//deserialize a bnode from a file (constructor)
bnode::bnode(std::string filename) {
    std::ifstream file;
    file.open(filename);

    if (file.is_open()) {
        file >> isLeaf >> uniqueID;

        int numKeys; file >> numKeys;
        for (int i = 0; i < numKeys; i++) {
            std::string temp; file >> temp;
            keys.push_back(temp);
        }

        int numPtrs; file >> numPtrs;
        for (int i = 0; i < numPtrs; i++) {
            int temp; file >> temp;
            ptrs.push_back(temp);
        }

        file.close();
    } else
        std::cout << "Error: file " << filename << " not found for reading." << "\n";
}

#endif
