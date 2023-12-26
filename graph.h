#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include "lib/LinkedList.h"
// #include "lib/bTree.h"

// Note: LL = LinkedList<std::string>

class graph {
private:

    // 1. 
    // An array of b trees, each index of the array denotes a btree that corresponds to the index of that vertex type in the vertexTypeList
    // Each b tree holds only the unique identifiers of all graph vertices in that vertex type
    // std::vector<bTree> bTreeArray;
    

    // 2/3.
    // list of all the vertex/edge types - this will be kept in memory and read/written to disk at program start/end
    LL vertexTypeList;
    LL edgeTypeList;


    // 4.
    /*

    * vector of pair of AdjList
    * first is string - vertex of typeX where relation is coming FROM
    * second is LL<string> - every element denotes a vertex where relation is going TO

    */
    //each index denotes an edgeType corresponding to the same index in edgeTypeList
    std::vector < std::pair < std::string, LL > > adjListArray;

    // Get the index of that TYPE of vertex. If it doesn't exist, put it in the list and return the index
    int getIndexOfTypeOfVertex(std::string _vertexTypeLabel) {
        int ans = vertexTypeList.findIndex(_vertexTypeLabel);
        if (ans == -1) {
            vertexTypeList.insert(_vertexTypeLabel);
            //TODO: Init a new btree in the bTreeArray with name '_vertexTypeLabel'
            //TODO: Make a new directory _data/bTrees/'_vertexTypeLabel'/
            ans = vertexTypeList.getSize();
        }
        return ans;
    }

    // Get the index of that TYPE of edge. If it doesn't exist, put it in the list and return the index
    int getIndexOfTypeOfEdge(std::string _edgeTypeLabel) {
        int ans = edgeTypeList.findIndex(_edgeTypeLabel);
        if (ans == -1) {
            edgeTypeList.insert(_edgeTypeLabel);
            //TODO: init a new adjList in adjListArray (make_pair<string, LL>())
            //TODO: make a new directory _data/adjLists/'type1_type2'/
            ans = edgeTypeList.getSize();
        }
        return ans;
    }

    //TODO:
    void dumpGraphData() {

        /* 

        FILE STRUCTURE:

        *  1. _data/vertexTypes.txt                  - just a sequential list of all the vertex types e.g typeX\ntypeY...

        *  2. _data/edgeTypes.txt                    - just a sequential list of all the edge types, e.g typeX_typeY\ntypeA_typeB...

        *  3. _data/adjLists/typeX_typeY/vertZ.txt   - Read edgeTypesList, where each entry denotes a directory
        *                                            - where typeX is a name of a vertex type, and typeY is another vertex type
        *                                            - the contents of each directory are as follows: a list of files of with name vertZ.txt, where vertZ is a vertex of typeX 
        *                                            - Each vertZ.txt file comprises a list of strings of vertices of typeY that have a relation with vertexZ (FROM vertZ TO [vertices in vertZ.txt])
        *                                            - the contents of a vertZ.txt file can then be passed to the LL constructor to deserialize it
 
        *  4. _data/btrees/typeX/infofile.txt...     - stores all the properties in a file, one file per vertex
        *                                            - Only category being updated in real-time thanks to b trees
        *                                            - The rest will be written to when the program ends
 
        */


        // Write vertex and edge type lists blindly with LL::writeToFile()
        vertexTypeList.writeToFile("data/vertexTypes.txt");
        edgeTypeList.writeToFile("data/edgeTypes.txt");

        // No need to update _data/bTrees/ as it'll be getting updated in real time by addVertex() and subsequently by bTree::insert()
        // No need to update adjLists/infofile.txt as it'll be getting updated in realtime by addVertex()

        // Make directories by the name of all the strings in infofile.txt, if they don't exist already
        // Loop through each string in infofile.txt, (i.e directory) and make
        // TODO: FIGURE THIS OUT AFTER I FINALIZE ADJLISTARRAY STRUCTURE    
    }

public:

    //basic ass constructor (mid)
    graph();

    //TODO: constructor that loads info from files
    //can serialize {vertex,edge}TypeList instantly with LL constructor on data/{vertex,edge}Types.txt
    //for adjLists, go through data/adjLists/infoFile.txt, and load all type1_type2.txt files into memory,
    //then for b trees
    // go through _data/bTrees/
    graph(int dummy); 

    bool addVertex (std::string _vertexTypeLabel, std::string uniqueKey) {
        
        // Identify the TYPE of the vertex from the vertexTypeList
        int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

        bool exists = true;

        // Search in B tree array at vertexType-th index and check if that vertex exists already
        /*
        if (bTreeArray[vertexType].search(uniqueKey) == nullptr)
            exists = false
        */

        if (exists)
            return false;

        //if it doesn't
        /*
        bTreeArray[vertexType].insert(uniqueKey);
        */

        //TODO: now we need to add the data of this vertex into its file (remember that each vertex has its own file)

        return true;
    }

    void addEdge (std::string _edgeTypeLabel, bool bidirectional,
                  std::string _vertex1ID, std::string _vertex2ID, 
                  std::string _vertex1Type, std::string _vertex2Type) {

        //first identify the TYPE of the edge from the edgeTypeList
        int edgeType = getIndexOfTypeOfEdge(_edgeTypeLabel);

        //check what type of relation we have - in format "node1type_node2type"
        std::string relationType = _vertex1Type + "_" + _vertex2Type;

        //TODO: Add the edge to adjListArray
        //go through vertex1's adj. list element and append in vertex2
        //if bidrectional, do the same for the other way around
    }

};

#endif //GRAPH_H
