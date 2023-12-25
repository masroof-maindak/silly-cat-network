#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include "lib/LinkedList.h"
// #include "lib/vertexDatum.h"

class graph {
private:

    //An array of b trees, each index of the array holds a btree that corresponds to the edge type in the edgeTypeList
    //Each b tree holds only the unique identifiers of all graph vertices in that edge type
    LinkedList<LinkedList<std::string>> vertexData; //TODO:change this declaration

    //list of all the vertex/edge types - this will be kept in memory and read/written at program run/closing
    LinkedList<std::string> vertexTypeList;
    LinkedList<std::string> edgeTypeList;
    LinkedList<std::string> adjListType; //string of what a certain type of adjacency list holds, e.g "cat_cat", "cat_post", etc.

    //vector of pair of string and adj. list
    //`first` in outer pair tells what its corresponding '`second`' holds, e.g "cat_cat", "cat_post", etc.
    //first's `second` is a pair itself and the inner adjacency list
    //inner pair is adj. list where `first` is a string and `second` is a linked list of all the nodes that have a relation with `first`
    std::vector< std::pair < std::string, std::pair < std::string, LinkedList<std::string> > > >;

    //So, what are all the files that we need for (sort of?) metadata?
    //    1. vertexTypes.txt   #just a sequential list of all the vertex types
    //    2. edgeTypes.txt     #just a sequential list of all the edge types
    //    3. vertexData.txt    #the general format of this file is as follows: the first line has the TOTAL NUMBER of vertices.
    //                          if a line starts with an exclamation mark, then it's the NAME of a vertex TYPE, 
    //                          otherwise it's the "UNIQUE-KEY~cumID" of a vertex with '~' as the delimiter
    //    4. AdjList.txt 

    //So then what is our 'actual' data?
    //    That would be the data of all the vertices.
    //    we store all the properties in a file, one per vertex, inside `VERTEX_DATA/NODETYPE/VERTEX_UNIQUE_KEY.bin`
    
    //get the index of that TYPE of vertex. And if it doesn't exist, then put it in the list and return the index
    int getIndexOfTypeOfVertex(std::string _vertexTypeLabel) {
        int ans = vertexTypeList.findIndex(_vertexTypeLabel);
        if (ans == -1) {
            vertexTypeList.insert(_vertexTypeLabel);
            ans = vertexTypeList.getSize();
        }
        return ans;
    }

    //get the index of that TYPE of edge. And if it doesn't exist, then put it in the list and return the index
    int getIndexOfTypeOfEdge(std::string _edgeTypeLabel) {
        int ans = edgeTypeList.findIndex(_edgeTypeLabel);
        if (ans == -1) {
            edgeTypeList.insert(_edgeTypeLabel);
            ans = edgeTypeList.getSize();
        }
        return ans;
    }

    //return a vertex' CUMULATIVE INTEGER ID (i.e what number of vertex was it at the time it was inserted)
    int findvertexCumulativeID(std::string _vertexTypeLabel, std::string _vertexID) {
        //if that type of vertex doesn't exist, we out
        int vertexTypeIndex = vertexTypeList.findIndex(_vertexTypeLabel);
        if (vertexTypeIndex == -1)
            return -1;

        Node <vertexDatum>* curr = vertexData[vertexTypeIndex].find( vertexDatum(_vertexID, 1) );

        //if that vertex doesn't exist, we out
        if (curr == nullptr)
            return -1;

        return curr->data.cumulativeID;
    }

public:

    //basic ass constructor
    graph() : totalVertexCount(0) {};

    //constructor that loads info from files
    // graph();

    bool addVertex (std::string _vertexTypeLabel, std::string uniqueKey) {
        flag = "null";

        // Identify the TYPE of the vertex from the vertexTypeList
        int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

        bool exists = false;
        //TODO: search in B tree array at vertexType-th index and check if that vertex exists already

        if (exists)
            return false;

        //if it doesn't
        //TODO: insert it in btree

        //TODO: now we need to add the data of this vertex into its file (remember that each vertex has its own file)

        return true;
    }

    void addEdge (std::string _edgeTypeLabel, std::string _vertex1ID, std::string _vertex2ID, bool bidirectional, std::string _vertex1Type, std::string _vertex2Type) {

        //first identify the TYPE of the edge from the edgeTypeList
        int edgeType = getIndexOfTypeOfEdge(_edgeTypeLabel);

        //check what type of relation we have - in format "node1type_node2type"
        std::string relationType = _vertex1Type + "_" + _vertex2Type;

        //TODO:
        //check if relation type exists, if it does, go through vertex1's adj. list element and append in vertex2
        //if not, first add this relation type to the vector
};

#endif //GRAPH_H
