#include <iostream>
#include "lib/LinkedList.h"

template <class T>
class graph {
private:
    //list of list of nodes (actually a list itself...)
    LinkedList<LinkedList<LinkedList<std::string>>> web;

    //list of all the node types
    LinkedList<std::string> nodeTypeWeb;

    //list of the all the edge types
    LinkedList<std::string> edgeTypeWeb;

    //list of all the edges
    LinkedList<LinkedList<std::string>> edgeWeb;

public:
    void addNode (std::string _nodeLabel, std::string uniqueKey) {
        //first identify the TYPE of the node from the nodeTypeWeb and get its index in web
        int index = nodeTypeWeb.findIndex(_nodeLabel);
        if (index == -1) {
            nodeTypeWeb.insert(_nodeLabel);
            index = nodeTypeWeb.getSize();
        }
        
        //then append it into the relevant list in the web 
        web[index].insert(LinkedList<std::string>(uniqueKey));
    }

    bool removeNode(std::string _nodeLabel, std::string uniqueKey) {
        //first identify the TYPE of the node and get its index in web
        int index = nodeTypeWeb.findIndex(_nodeLabel);
        if (index == -1) 
            return false;

        //now obtain a reference of this list
        LinkedList<LinkedList<std::string>>& listOfThisLabel = web[index];

        //then remove the value from the LL that only houses its type of nodes
        return listOfThisLabel.erase(listOfThisLabel.find(uniqueKey)); 
    }
};
