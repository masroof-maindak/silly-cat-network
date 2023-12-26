#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include "lib/LinkedList.h"
#include <unordered_map>
#include <sstream>
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

    void makeDir(std::string path, std::string newDirName) {
        std::string command = "mkdir -p " + path + newDirName;
        system(command.c_str());
    }

    // Get the index of that TYPE of vertex. If it doesn't exist, put it in the list and return the index
    int getIndexOfTypeOfVertex(std::string _vertexTypeLabel) {
        int ans = vertexTypeList.findIndex(_vertexTypeLabel);
        if (ans == -1) {
            vertexTypeList.insert(_vertexTypeLabel);
            //TODO: Init a new btree in the bTreeArray with name '_vertexTypeLabel'
            makeDir("_ata/bTrees/", _vertexTypeLabel);
            ans = vertexTypeList.getSize();
        }
        return ans;
    }

    // Get the index of that TYPE of edge. If it doesn't exist, put it in the list and return the index
    int getIndexOfTypeOfEdge(std::string label) {
        int ans = edgeTypeList.findIndex(_edgeTypeLabel);
        if (ans == -1) {
            edgeTypeList.insert(_edgeTypeLabel);
            //TODO: init a new adjList in adjListArray (make_pair<string, LL>())
            //TODO: make a new directory _data/adjLists/'relation-typeX_typeY'/
            makeDir("_data/adjLists/", newDir);
            ans = edgeTypeList.getSize();
        }
        return ans;
    }

    void dumpGraphData() {

        /* 

        FILE STRUCTURE:

        *  1. _data/vertexTypes.txt                  - just a sequential list of all the vertex types e.g typeX\ntypeY...

        *  2. _data/edgeTypes.txt                    - just a sequential list of all the edge types, e.g Relation:typeX_typeY\ntypeA_typeB...

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


        // TODO: Figure out deserializing adjaceny lists...
    }

    void updateVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties);

public:

    //basic ass constructor (mid)
    graph();

    //can serialize {vertex,edge}TypeList instantly with LL constructor on data/{vertex,edge}Types.txt
    //then for b trees, loop through vertex types and pass them as 'name' to btree file constructor. It'll handle the rest.
    //TODO: Figure out serializing adjacency lists...
    graph(int fileFlagCheck); 

    bool addVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);

    bool addEdge (std::string _edgeTypeLabel, bool bidirectional,
                  std::string _vertex1ID, std::string _vertex2ID, 
                  std::string _vertex1Type, std::string _vertex2Type);

    void mergeVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);

};

bool graph::addEdge (std::string _edgeTypeLabel, bool bidirectional,
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

bool graph::addVertex (std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties) {

    // Identify the TYPE of the vertex from the vertexTypeList
    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

    bool exists = true;

    // Search in B tree array at vertexType-th index and check if that vertex exists already
    //TODO: remove comment when btreeArr finalized
    /*
    if (bTreeArray[vertexType].search(uniqueKey) == nullptr)
        exists = false
    */

    if (exists)
        return false;

    //TODO: remove comment when btreeArr finalized
    /*
    bTreeArray[vertexType].insert(uniqueKey);
    */

    //_vertexProperties is in form: "key1:value1~key2:value2~...keyN:valueN~"

    std::string writePath;
    //TODO: Get proper path
    std::ofstream file(writePath, std::ios::binary);
    file.write((char*)_vertexProperties.size(), sizeof(int));
    file.write(_vertexProperties.c_str(), _vertexProperties.size());
    file.close();

    return true;
}

void graph::updateVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties) {
    
    //first read info from file
    std::string readPath;
    //TODO: Get proper path
    std::ifstream file(readPath, std::ios::binary);
    
    // Read int size to get string size
    int fileSize;
    file.read((char*)&fileSize, sizeof(int));

    // Read the file into a string and close
    std::string oldProperties; oldProperties.resize(fileSize);
    file.read(&oldProperties[0], fileSize);
    file.close();

    // Convert both strings into hashmaps
    // They're in the format: "key1:value1~key2:value2~...keyN:valueN~"
    // Split them by '~' and then by ':'
    std::unordered_map<std::string, std::string> oldPropertiesMap, newPropertiesMap;

    // Split the oldProperties string by '~'
    std::stringstream oldProp (oldProperties);
    std::string keyValueString;
    while (std::getline (oldProp, keyValueString, '~')) {
        // Split the keyValueString string by ':'
        std::string key, value;
        std::stringstream ss2(keyValueString);
        std::getline(ss2, key, ':');
        std::getline(ss2, value, ':');
        oldPropertiesMap[key] = value;
    }

    std::stringstream newProp(newProperties);
    // Split the newProperties string by '~'
    while (std::getline(newProp, keyValueString, '~')) {
        // Split the keyValueString string by ':'
        std::string key, value;
        std::stringstream ss4(keyValueString);
        std::getline(ss4, key, ':');
        std::getline(ss4, value, ':');
        newPropertiesMap[key] = value;
    }

    // Go through through newProperties and update oldProperties
    for (auto it = newPropertiesMap.begin(); it != newPropertiesMap.end(); it++) {
        oldPropertiesMap[it->first] = it->second;
    }

    // Coalesce hashmap into a string
    std::string properties;
    for (auto it = oldPropertiesMap.begin(); it != oldPropertiesMap.end(); it++) {
        properties += it->first + ":" + it->second + "~";
    }

    // Write the string to the file
    std::ofstream file2(readPath, std::ios::binary);
    file2.write((char*)properties.size(), sizeof(int));
    file2.write(properties.c_str(), properties.size());
    file2.close();

}

void graph::mergeVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties) {

    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

    bool exists = true;

    // Search in B tree array at vertexType-th index and check if that vertex exists already
    //TODO: remove comment when btreeArr finalized
    /*
    if (bTreeArray[vertexType].search(uniqueKey) == nullptr)
        exists = false
    */

    if (!exists)
        addVertex (uniqueKey, _vertexTypeLabel, _vertexProperties);
    else
        updateVertex(uniqueKey, _vertexTypeLabel, _vertexProperties);
}

#endif //GRAPH_H
