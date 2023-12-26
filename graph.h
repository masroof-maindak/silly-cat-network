#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include "lib/LinkedList.h"
#include "lib/adjacencyList.h"
#include <unordered_map>
#include <sstream>
// #include "lib/bTree.h"

// Note: LL = LinkedList<std::string>

void makeDir(std::string path, std::string newDirName) {
    std::string command = "mkdir -p " + path + newDirName;
    system(command.c_str());
}

class graph {
private:

    /*
    CORE DATA STRUCTURES:
    */

    // 1. 
    // An array of b trees, each index of the array denotes a btree that corresponds to the index of that vertex type in the vertexTypeList
    // Each b tree holds only the unique identifiers of all graph vertices in that vertex type
    // std::vector<bTree> bTreeArray;
    
    // 2/3.
    // list of all the vertex/edge types - this will be kept in memory and read/written to disk at program start/end
    LL vertexTypeList;
    LL edgeTypeList;

    // 4.
    // vector of linked list of AdjList
    // Each index holds a set of adjLists that correspond to the index of that edge type in the edgeTypeList
    std::vector<LinkedList<adjList>> adjListArray;

    /*
    FILE STRUCTURE:

    *  1. _data/vertexTypes.txt                  - just a sequential list of all the vertex types e.g typeX\ntypeY...
    
    *  2. _data/edgeTypes.txt                    - just a sequential list of all the edge types, e.g Relation:typeX_typeY\ntypeA_typeB...
    
    *  3. _data/adjLists/rel-typeX_typeY/z.txt   - Read edgeTypesList, where each entry denotes a directory
    *                                            - infofile.txt is a list of all the files in that directory, one file per line, without the .txt extension
    *                                            - the contents of a directory are a list of files of with name z.txt, where Z is a vertex of typeX 
    *                                            - Each z.txt file comprises a list of strings of vertices of typeY that have a relation with vertexZ 
    *                                            - i.e `FROM` Z `TO` [all lines (vertices) in Z.txt]
    
    *  4. _data/btrees/typeX/infofile.txt...     - stores all the properties in a file, one file per vertex
    *                                            - Only category being updated in real-time thanks to b trees
    *                                            - The rest will be written to when the program ends
    
    *  5. _data/vertexProperties/uniqueKey.txt   - stores all the properties in a file, one file per vertex
    *                                            - in this format: "key1:value1~key2:value2~...keyN:valueN~"
    */

    /*
    PRIVATE HELPER FUNCTIONS:
    */

    // Get the index of that TYPE of {vertex,edge}. If it doesn't exist, put it in the relevant type
    // list, push its entry into the {bTree,adjList}Array and return the final (newly added) index
    int getIndexOfTypeOfVertex(std::string _vertexTypeLabel);
    int getIndexOfTypeOfEdge(std::string label);

    // Write all the data structures to disk
    void dumpGraphData();

    // Used in merge()
    void updateVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties);

public:

    // Constructors
    graph();                  // default
    graph(int fileCheckFlag); // file-based

    bool addVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);
    bool addEdge (std::string _edgeTypeLabel, bool bidirectional,
                  std::string _vertex1ID, std::string _vertex2ID, 
                  std::string _vertex1Type, std::string _vertex2Type);
    void mergeVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);

    //TODO: Filter functions + Relational Queries

};

void graph::dumpGraphData() {
    // Write vertex and edge type lists blindly with LL::writeToFile()
    vertexTypeList.writeToFile("data/vertexTypes.txt");
    edgeTypeList.writeToFile("data/edgeTypes.txt");

    // Btrees get managed simultaneously by bTree class, so no need to write them here

    // Write adjLists
    // The appropriate directory structure is already created in the getIndexOfTypeOfEdge() function
    // So we just need to write the infofile.txt and the z.txt files
    std::vector<std::string> edgeTypes = edgeTypeList.vecDump();

    //Loop through all the linked lists in the AdjListArray
    for (int i = 0; i < edgeTypes.size(); i++) {

        // Generate the directory path (one dir per edge type -> _data/adjLists/rel-typeX_typeY)
        std::string dir = "data/adjLists/" + edgeTypes[i];

        // Create the infofile.txt for each dir (holds all the filenames in the dir)
        std::ofstream infofile(dir + "/infofile.txt");

        // Loop through all the adjLists in the current LL to get the fromNodes, which we will use as filenames
        for (Node<adjList>* curr = adjListArray[i].begin(); curr != nullptr; curr = curr->next) {

            // Append these filenames to the infofile.txt
            infofile << curr->data.fromNode << "\n";

            // Write the `toNodes` to the file which has that `fromNode's` name 
            curr->data.writeToPath(dir + "/" + curr->data.fromNode + ".txt");
        }

        infofile.close();
    }
}

graph::graph() {
    //can serialize {vertex,edge}TypeList instantly with LL constructor on data/{vertex,edge}Types.txt
    vertexTypeList = LL("data/vertexTypes.txt");
    edgeTypeList = LL("data/edgeTypes.txt");

    std::vector<std::string> vertexTypes = vertexTypeList.vecDump();
    std::vector<std::string> edgeTypes = edgeTypeList.vecDump();

    //then for b trees, loop through vertex types and pass them as 'name' to btree file constructor.
    for(std::string vertexType : vertexTypes) {
        std::string dir = "data/bTrees/" + vertexType;
        //TODO: remove comment when btreeArr finalized
        /*
        bTreeArray.push_back(bTree(dir, 0);)
        */
    }

    //then for adjLists, loop through edge types (i.e loop through different relation directories)
    for(std::string edgeType : edgeTypes) {
        std::string dir = "data/adjLists/" + edgeType;
        adjListArray.push_back(LinkedList<adjList>());
        std::vector<std::string> fileNames = LL(dir + "/infofile.txt").vecDump();
        // Then loop through every file in that directory and pass it to the adjList constructor
        for(std::string fileName : fileNames) {
            adjListArray.back().insert(adjList(fileName, LL(dir + "/" + fileName + ".txt")));
        }
    }
}

int graph::getIndexOfTypeOfEdge(std::string label) {
    int ans = edgeTypeList.findIndex(label);
    if (ans == -1) {
        edgeTypeList.insert(label);
        ans = edgeTypeList.getSize();
        adjListArray.push_back(LinkedList<adjList>());
        makeDir("_data/adjLists/", label);
    }
    return ans;
}

bool graph::addEdge (std::string relation, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type) {

    // label is brought to format "Relation-typeX_typeY"
    std::string completeLabel = relation + "-" + _vertex1Type + "_" + _vertex2Type;

    // first identify the TYPE of the edge from the edgeTypeList
    int edgeType = getIndexOfTypeOfEdge(completeLabel);

    // check if that vertex1's adj. list FROM element exists, 
    adjList* adjListElement = &adjListArray[edgeType].find(adjList(_vertex1ID, LL()))->data;

    // if it does, just append the new vertex2ID to it, else, create a new adjList element
    if (adjListElement != nullptr)
        adjListElement->toNodes.insert(_vertex2ID);
    else
        adjListArray[edgeType].insert(adjList(_vertex1ID, LL(_vertex2ID, 0)));

    if (bidirectional)
        addEdge(relation, false, _vertex2ID, _vertex1ID, _vertex2Type, _vertex1Type);

    return true;
}

int graph::getIndexOfTypeOfVertex(std::string _vertexTypeLabel) {
    int ans = vertexTypeList.findIndex(_vertexTypeLabel);
    if (ans == -1) {
        vertexTypeList.insert(_vertexTypeLabel);
        //TODO: Init a new btree in the bTreeArray with name '_vertexTypeLabel'
        makeDir("_data/bTrees/", _vertexTypeLabel);
        ans = vertexTypeList.getSize();
    }
    return ans;
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

    std::string writePath = "_data/vertexProperties/" + uniqueKey + ".bin";
    std::ofstream file(writePath, std::ios::binary);
    file.write((char*)_vertexProperties.size(), sizeof(int));
    file.write(_vertexProperties.c_str(), _vertexProperties.size());
    file.close();

    return true;
}

void graph::updateVertex(std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties) {
    
    //first read info from file
    std::string readPath = "_data/vertexProperties/" + uniqueKey + ".bin";
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
