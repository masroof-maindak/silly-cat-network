#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include "lib/LinkedList.h"
#include "lib/vertexDatum.h"

class graph {
private:

    //List of all vertex IDs
    //    we can afford to keep this in memory, since it's just a list of strings
    //    inner layer of the list is the list of all the vertices of a particular type
    //    Note that the ID of a vertex is unique across all vertices in its type - enforce this later
    LinkedList<LinkedList<vertexDatum>> vertexData;

    //list of all the vertex types - this will be kept in memory
    LinkedList<std::string> vertexTypeList;

    //list of the all the edge types - this will be kept in memory
    LinkedList<std::string> edgeTypeList;

    //adjacency matrix
    //this is the big chunk that's inefficient to store in memory, but we have no option but to do so.
    std::vector<std::vector<int>> adjMatrix;

    //used for assigning cumulative IDs to vertices
    //will be kept on the first line in the vertexIDs.txt file
    int totalVertexCount;

    //here's the plan:
    //    we're maintaining all the vertex IDs in a file somewhere, and keeping it all in memory too
    //    we're not maintaining the edges explicitly AT ALL, we'll just have an adjacency matrix with 0 depicting no edge and some number 'x' depicting an edge's existence
    //    the value of 'x' will vary depending on what KIND of edge it is. So, how do we know what edge _type_ 'x' maps to? We'll be keeping a separate list of edge types.

    //So, what are all the files that we need for (sort of?) metadata?
    //    1. vertexTypes.txt   #just a sequential list of all the vertex types
    //    2. edgeTypes.txt     #just a sequential list of all the edge types
    //    3. vertexData.txt    #the general format of this file is as follows: the first line has the TOTAL NUMBER of vertices.
    //                          if a line starts with an exclamation mark, then it's the NAME of a vertex TYPE, 
    //                          otherwise it's the "UNIQUE-KEY~cumID" of a vertex with '~' as the delimiter
    //    4. adjMatrix.txt     #just a matrix of numbers, where each number represents the type of edge 
    //                          between the vertices at the corresponding row and column

    //So then what is our 'actual' data?
    //    That would be the data of all the vertices.
    //    we can make a separate file for each vertex type, and store the data there sequentially
    //    e.g if there are three vertexTypes, A, B, C, then we'll have three files, A.txt, B.txt, C.txt
    //    and each file will have the data of all the vertices of that type

    //Now, we can theoretically have random access to any given vertex from the file (std::streampos_t readPoint = indexOfThatVertexInItsVertexType * numberOfBytesRequiredToHoldOneVertexOfThatType)
    //    the drawback? we would need to stuff random data into the file for each vertex that doesn't have data for that field
    //    this also means that once we have defined what fields a vertex has, we can't add more fields to it
    //    Now, for some vertices, this doesn't matter, e.g for an account, we don't need anything except name, email, password, and maybe a profile picture
    //    but for other vertices, this is a problem, e.g for a post, we might want an array of strings for comments, and we might want to add more comments later
    //    the 'solution' is to ...
    //    TODO: FIGURE THIS OUT LATER (HARD DIFFICULTY)
    
    int verticesIndexOfVertexTypeAmidstOtherTypes(std::string _vertexTypeLabel) {
        int ans = vertexTypeList.findIndex(_vertexTypeLabel);
        if (ans == -1) {
            vertexTypeList.insert(_vertexTypeLabel);
            ans = vertexTypeList.getSize();
        }
        return ans;
    }

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

    void addvertex (std::string _vertexTypeLabel, std::string uniqueKey) {

        //first identify the TYPE of the vertex from the vertexTypeList and get its index in web
        int vertexType = verticesIndexOfVertexTypeAmidstOtherTypes(_vertexTypeLabel);

        //now search that vertexType to see if a vertex with this ID already exists, if it does, return
        if ( vertexData[vertexType].find( vertexDatum(uniqueKey, 1) ) != nullptr ) {
            std::cerr << "ERROR: A vertex with this ID already exists" << std::endl;
            return;
        }

        //then append it into the relevant list in the web
        vertexData[vertexType].insert( vertexDatum(uniqueKey, totalVertexCount++));
        //we don't mind re-traversing the entire outer list in the vertexData list (via subscript), since it only holds vertex TYPES, and we're not going to have many of those anyway

        //then add a new row and column to the adjMatrix
        //TODO: not sure if this is valid...
        adjMatrix.push_back(std::vector<int>(adjMatrix.size(), 0));
        for (int i = 0; i < adjMatrix.size(); i++) {
            adjMatrix[i].push_back(0);
        }

        //now we need to add the data of this vertex into the file of its type...
        //TODO: FIGURE THIS OUT LATER (EASY DIFFICULTY - just load the file in, append at the end, close the file)
    }

    void addEdge (std::string _edgeTypeLabel, std::string _vertex1ID, std::string _vertex2ID, bool bidirectional, std::string _vertex1Type, std::string _vertex2Type) {

        //first identify the TYPE of the edge from the edgeTypeList and get its index in web
        int edgeType = edgeTypeList.findIndex(_edgeTypeLabel);
        //if that edge doesn't exist
        if (edgeType == -1) {
            //add it
            edgeTypeList.insert(_edgeTypeLabel);
            //get its index
            edgeType = edgeTypeList.getSize();
        }

        int cumIndex1 = findvertexCumulativeID(_vertex1Type, _vertex1ID);
        int cumIndex2 = findvertexCumulativeID(_vertex2Type, _vertex2ID);

        if (cumIndex1 == -1 || cumIndex2 == -1) {
            std::cerr << "ERROR: One of the vertices doesn't exist" << std::endl;
            return;
        }

        //then add a new row and column to the adjMatrix
        adjMatrix[cumIndex1][cumIndex2] = edgeType;
        if (bidirectional)
            adjMatrix[cumIndex2][cumIndex1] = edgeType;
    }
};

#endif //GRAPH_H