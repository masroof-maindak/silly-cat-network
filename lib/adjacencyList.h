#include <string>
#include "LinkedList.h"

struct adjList {
    std::string fromNode;
    LL toNodes;

    adjList(std::string _fromNode, LL _toNodes) {
        fromNode = _fromNode;
        toNodes = _toNodes;
    }

    adjList() {
        fromNode = "";
        toNodes = LL();
    }

    void writeToPath(std::string path) {
        std::string filename = path + fromNode;
        toNodes.writeToFile(filename);
    }

    // Used to check the existence of an adjList within a certain LL of a relation
    bool operator==(const adjList& other) const { 
        return fromNode == other.fromNode;
    }
};