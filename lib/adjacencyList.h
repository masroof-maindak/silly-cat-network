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

    bool operator==(const adjList& other) const {
        return fromNode == other.fromNode;
    }
};