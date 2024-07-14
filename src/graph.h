#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <unordered_map>

#include "../lib/LinkedList.h"
#include "../lib/encryption.h"
#include "../lib/bTree/bTree.h"

void makeDir(std::string path, std::string newDirName) {
    std::filesystem::create_directory(path + newDirName);
}

class graph {
private:

    /*
    CORE DATA STRUCTURES:
    */

    // 1.
    // An array of b trees, each index of the array denotes a btree that corresponds to the index of that vertex type in the vertexTypeList
    // Each b tree holds only the unique keys of all graph vertices of that vertex type
    std::vector<bTree> bTreeArray;
    
    // 2/3.
    // list of all the vertex/edge types - this will be kept in memory and read/written to disk at program start/end
    LL vertexTypeList;
    LL edgeTypeList;

    /*
    FILE STRUCTURE:

    *  1. _data/vertexTypes.txt                  - A sequential list of all the vertex types, e.g typeX\ntypeY...
    
    *  2. _data/edgeTypes.txt                    - A sequential list of all the edge types, e.g Relation:typeX_typeY\ntypeA_typeB...
    
    *  3. _data/adjLists/rel-typeX_typeY/z.txt   - Read edgeTypesList, where each entry denotes a directory
    *                                            - infofile.txt is a list of all the files in that directory, one file per line, without the .txt extension
    *                                            - the contents of a directory are a list of files of with name z.txt, where Z is a vertex of typeX 
    *                                            - Each z.txt file comprises a list of strings of vertices of typeY that have a relation with vertexZ 
    *                                            - i.e `FROM` Z `TO` [all lines (vertices) in Z.txt]
    
    *  4. _data/btrees/typeX/infofile.txt...     - stores all the properties in a file, one file per vertex
    *                                            - Only category being updated in real-time thanks to b trees
    *                                            - The rest will be written to when the program ends
    
    *  5. _data/vertexProperties/uniqueKey.txt   - stores all the properties in a file, one file per vertex
    *                                            - Where the file's name is the uniqueKey of the vertex, which is arguably a
    *                                            - security flaw and also invalidates the use of b trees for vertex lookup, but this is a prototype anyway
    *                                            - in this format: "{stringSize}key1:value1~key2:value2~...~keyN:valueN~"
    */

    /*
    PRIVATE HELPER FUNCTIONS:
    */

    // Get the index of that TYPE of {vertex,edge}. If it doesn't exist, put it in the relevant type
    // list, push its entry into {bTreeArray,adjList/[edgetype]/} and return the final (newly added) index
    int getIndexOfTypeOfVertex(std::string _vertexTypeLabel);
    int getIndexOfTypeOfEdge(std::string _edgeTypeLabel);

    void dumpGraphToDisk();
    void updateVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties);
    std::string fetchProperties(std::string uniqueKey);
    std::unordered_map<std::string, std::string> generatePropertiesToMatch(std::string _propertiesToMatch);

public:

    // Constructors
    graph() {makeDir("_data/", "vertexProperties");}      // default - makes an empty directory called vertexProperties
    graph(int fileCheckFlag);                             // file-based - inits graph from files

    bool addVertex (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);
    bool addEdge (int transactionID, std::string _edgeTypeLabel, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type);
    void addVE (int transactionID, std::string _edgeTypeLabel, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type, std::string _vertexProperties);
    void mergeVertex (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);

    //Get a single vertex's properties
    void fetchVertexProperties (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel);

    bool removeVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel);
    bool removeEdge(int transactionID, std::string _edgeTypeLabel, bool bidirectional,
                  std::string _vertex1ID, std::string _vertex2ID, 
                  std::string _vertex1Type, std::string _vertex2Type);
    void removeVE(int transactionID, std::string _edgeTypeLabel, bool bidirectional,
                  std::string _vertex1ID, std::string _vertex2ID, 
                  std::string _vertex1Type, std::string _vertex2Type);

    //receives a vertexTypeLabel, and returns all the vertices of that type that have a common property
    //in the form of a string of uniqueKeys separated by '~'
    void filter (int transactionID, std::string _vertexTypeLabel, std::string _propertiesToMatch);

    //recieves a node from a certain type, another type, a relation, and returns a list of nodes of 
    //the second type that are connected to the first node by that relation, and have a certain set of properties
    //in the form of a string of uniqueKeys separated by '~'
    void relationalQuery (int transactionID, std::string _vertex1ID, std::string _vertex1Type, std::string _vertex2Type, std::string _edgeTypeLabel, std::string _propertiesToMatch);

    /*
    TODO:
    **  Refactor functions to push file they're working on to the filesBeingProcessedQueue, and pop them when done
    **  Refactor functions to wait if the file they're trying to access is in the filesBeingProcessedQueue (include a while loop that checks if the file is in the queue, if it is, sleep for 0.4s)
    */

   ~graph() { dumpGraphToDisk(); }
};

void graph::removeVE(int transactionID, std::string _edgeTypeLabel, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type) {
    // Can't fathom this being used for anything other than when a post is deleted, but including for completeness      
    removeEdge(transactionID, _edgeTypeLabel, bidirectional, _vertex1ID, _vertex2ID, _vertex1Type, _vertex2Type);
    removeVertex(transactionID, _vertex1ID, _vertex1Type);
}

void graph::addVE (int transactionID, std::string _edgeTypeLabel, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type, std::string _vertexProperties) {

    //this is practically only going to be used for when a new post is uploaded
    //so we can assume that the vertex1ID is the username of the user who uploaded the post
    //and the vertex2ID is the uniqueKey of the post itself, which gets init now

    addVertex(transactionID, _vertex2ID, _vertex2Type, _vertexProperties);
    addEdge (transactionID, _edgeTypeLabel, bidirectional, _vertex1ID, _vertex2ID, _vertex1Type, _vertex2Type);
}

void graph::fetchVertexProperties (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel) {
    
    // Check if vertex type exists
    int vertexType = vertexTypeList.findIndex(_vertexTypeLabel);
    if (vertexType == -1) {
        answerQueue.push({transactionID, "Failure: Vertex type doesn't exist, so vertex can't exist either."});
        return;
    }

    // check if vertex exists
    if (bTreeArray[vertexType].search(uniqueKey) == -1) {
        answerQueue.push({transactionID, "Failure: Vertex doesn't exist, can't fetch it."});
        return;
    }

    // Fetch properties and push them to answerQueue    
    answerQueue.push({transactionID, fetchProperties(uniqueKey)});
}

void graph::relationalQuery (int transactionID, std::string _vertex1ID, std::string _vertex1Type, std::string _vertex2Type, std::string _edgeTypeLabel, std::string _propertiesToMatch) {
    //Note: relationalQuery assumes that all provided information is valid.

    bool noPropertiesToMatch = (_propertiesToMatch == "");
    std::unordered_map<std::string, std::string> propertiesToMatch = generatePropertiesToMatch(_propertiesToMatch);

    //first get all nodes of type _vertex2Type that have a relation with _vertex1ID, open the adjList for that relation
    std::string completeLabel = _edgeTypeLabel + "-" + _vertex1Type + "_" + _vertex2Type;

    //open the adjList for that relation
    std::string dir = "_data/adjLists/" + completeLabel + "/";
    LL adjList(dir + _vertex1ID + ".txt");
    std::vector<std::string> type2Vertices = adjList.vecDump();

    //now we have all the nodes of type _vertex2Type that have a relation with _vertex1ID, loop through them to match properties
    std::string retVertexList;
    for (std::string vertex : type2Vertices) {
            
            //fetch existing properties in format "key1:value1~key2:value2~...keyN:valueN~"
            std::string properties = fetchProperties(vertex);

            //skip checking the hashmap if the user didn't provide any properties to match with in the first place
            if (noPropertiesToMatch) {
                retVertexList += vertex + "~";
                continue;
            }
    
            //split the properties string by '~'
            std::stringstream ss(properties);
            std::string keyValueString;
            bool match = true;
    
            while (std::getline(ss, keyValueString, '~')) {
                //split the keyValueString string by ':'
                std::string key, value;
                std::stringstream ss2(keyValueString);
                std::getline(ss2, key, ':');
                std::getline(ss2, value, ':');
    
                //if the key exists in propertiesToMatch, check if the values match
                if (propertiesToMatch.find(key) != propertiesToMatch.end()) {
                    //if the value matches, pop the key from propertiesToMatch
                    propertiesToMatch.erase(key);

                    //if the values don't match, break out of the loop as this vertex is invalid now anyway
                    if (propertiesToMatch[key] != value) {
                        match = false;
                        break;
                    }
                }            
            }
    
            //if match is true though, i.e all the values satisfy the requirements set by the user, 
            //and propertiesToMatch is empty i.e all the keys to match have been matched successfully
            //add the node to retNodeList
            if (match and propertiesToMatch.empty())
                retVertexList += vertex + "~";
    }

    if (retVertexList == "")
        retVertexList = "Failure: No nodes of type " + _vertex2Type + " found that have a relation with " + _vertex1ID + " of type " + _vertex1Type + " and match the provided properties.";
    answerQueue.push({transactionID, retVertexList});
}

std::unordered_map<std::string, std::string> graph::generatePropertiesToMatch(std::string _propertiesToMatch) {

    //generate hashmap of properties to match
    std::unordered_map<std::string, std::string> propertiesToMatch;

    //split the properties string by '~'
    std::stringstream ss(_propertiesToMatch);
    std::string keyValueString;
    while (std::getline(ss, keyValueString, '~')) {
        //split the keyValueString string by ':'
        std::string key, value;
        std::stringstream ss2(keyValueString);
        std::getline(ss2, key, ':');
        std::getline(ss2, value, ':');

        propertiesToMatch[key] = value;
    }

    return propertiesToMatch;
}

void graph::filter (int transactionID, std::string _vertexTypeLabel, std::string _propertiesToMatch) {

    bool noPropertiesToMatch = (_propertiesToMatch == "");
    std::unordered_map<std::string, std::string> propertiesToMatch = generatePropertiesToMatch(_propertiesToMatch);

    //Note: Filter assumes that the all provided information is valid.
    std::string retVertexList;

    //first acquire all vertices of that type
    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

    //open the btree file for that type and dump contents to vector
    std::vector<std::string> vertexList = bTreeArray[vertexType].dump();

    //loop through vector of vertices, open their files, check if they have the properties we want, if they do, add them to retNodeList
    for (std::string vertex : vertexList) {
        
        //skip checking the hashmap if the user didn't provide any properties to match with in the first place
        if (noPropertiesToMatch) {
            retVertexList += vertex + "~";
            continue;
        }

        //fetch existing properties in format "key1:value1~key2:value2~...keyN:valueN~"
        std::string properties = fetchProperties(vertex);

        //split the properties string by '~'
        std::stringstream ss(properties);
        std::string keyValueString;
        bool match = true;

        std::unordered_map<std::string, std::string> propertiesToMatchBak = propertiesToMatch;

        while (std::getline(ss, keyValueString, '~')) {
            //split the keyValueString string by ':'
            std::string key, value;
            std::stringstream ss2(keyValueString);
            std::getline(ss2, key, ':');
            std::getline(ss2, value, ':');

            //if the key exists in propertiesToMatch, then
            if (propertiesToMatchBak.find(key) != propertiesToMatch.end()) {

                //if the values DON'T match, break out of the loop as this vertex is invalid now anyway
                if (propertiesToMatchBak[key] != value) {
                    match = false;
                    break;
                
                //if the value DOES match, pop the key from propertiesToMatch
                } else {
                    propertiesToMatchBak.erase(key);
                }
            }            
        }

        //if match is true though, i.e all the values satisfy the requirements set by the user, add the node to retNodeList
        if (match)
            retVertexList += vertex + "~";
    }

    if (retVertexList == "")
        retVertexList = "Failure: No nodes of type " + _vertexTypeLabel + " found that match the provided properties.";
    answerQueue.push({transactionID, retVertexList});
}

bool graph::removeEdge(int transactionID, std::string relation, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type) {

    // Check if vertex1 and vertex2 exist in our bTrees
    int vertex1Type = getIndexOfTypeOfVertex(_vertex1Type);
    int vertex2Type = getIndexOfTypeOfVertex(_vertex2Type);

    if (bTreeArray[vertex1Type].search(_vertex1ID) == -1 or bTreeArray[vertex2Type].search(_vertex2ID) == -1) {
        answerQueue.push({transactionID, "Failure: Vertex doesn't exist, can't remove edge from it."});
        return false;
    }
    
    // label is brought to format "Relation-typeX_typeY"
    std::string completeLabel = relation + "-" + _vertex1Type + "_" + _vertex2Type;

    // first identify the TYPE of the edge from the edgeTypeList
    int edgeType = edgeTypeList.findIndex(completeLabel);

    //if that edge type doesn't exist, what are we supposed to be removing?
    if(edgeType == -1) {
        answerQueue.push({transactionID, "Failure: Edge type doesn't exist, so edge can't exist either."});
        return false;
    }

    //open up the files
    std::string dir = "_data/adjLists/" + completeLabel + "/";
    LL info(dir + "infofile.txt");

    //if vertex1ID doesn't exist in the infofile.txt, return false
    if (!info.exists(_vertex1ID)) {
        answerQueue.push({transactionID, "Failure: Requested vertex has no edges of type " + completeLabel});
        return false;
    }

    //else open the file for vertex1ID
    LL vertex1AdjList(dir + _vertex1ID + ".txt");

    //remove vertex2ID from it
    if (!vertex1AdjList.erase(vertex1AdjList.find(_vertex2ID))) {
        answerQueue.push({transactionID, "Failure: " + _vertex1ID + " has no edge to " + _vertex2ID + " of type " + completeLabel});
        return false;
    }

    //write changes to disk
    vertex1AdjList.writeToFile(dir + _vertex1ID + ".txt");

    if (bidirectional) {
        //if vertex2ID doesn't exist in the infofile.txt, return false
        if (!info.exists(_vertex2ID)) {
            answerQueue.push({transactionID, "Failure: Requested vertex has no edges of type " + completeLabel});
            return false;
        }

        //open the file for vertex2ID
        LL vertex2AdjList(dir + _vertex2ID + ".txt");

        //remove vertex1ID from it
        if (!vertex2AdjList.erase(vertex2AdjList.find(_vertex1ID))) {
            answerQueue.push({transactionID, "Failure: " + _vertex2ID + " has no edge to " + _vertex1ID + " of type " + completeLabel});
            return false;
        }

        // and write changes to disk
        vertex2AdjList.writeToFile(dir + _vertex2ID + ".txt");
    }

    answerQueue.push({transactionID, "Success: Edge removed"});
    return true;
}

bool graph::removeVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel) {

    int vertexType = vertexTypeList.findIndex(_vertexTypeLabel);
    if (vertexType == -1) {// If vertex type doesn't exist, what are we supposed to be removing?
        answerQueue.push({transactionID, "Failure: Vertex type doesn't exist, so vertex can't exist either."});
        return false;
    }

    // If removal from btree successful
    if (bTreeArray[vertexType].erase(uniqueKey)) {
        // Delete the properties file
        std::string filepath = "_data/vertexProperties/" + uniqueKey + ".bin";
        std::remove(filepath.c_str());
        answerQueue.push({transactionID, "Success: Vertex removed"});
        return true;
    }

    answerQueue.push({transactionID, "Failure: Vertex doesn't exist, can't remove it."});
    return false;
}

void graph::dumpGraphToDisk() {
    // Write vertex and edge type lists unconditionally with LL::writeToFile()
    vertexTypeList.writeToFile("_data/vertexTypes.txt");
    edgeTypeList.writeToFile("_data/edgeTypes.txt");
    // Btrees get managed in real time by the bTree class, so no need to write them here
    // Adj lists get managed in real time by whatever function accesses them, so no need to write them here
}

graph::graph(int fileCheckFlag) {
    // Can serialize {vertex,edge}TypeList instantly with LL constructor on data/{vertex,edge}Types.txt
    vertexTypeList = LL("_data/vertexTypes.txt");
    edgeTypeList = LL("_data/edgeTypes.txt");

    std::vector<std::string> vertexTypes = vertexTypeList.vecDump();
    std::vector<std::string> edgeTypes = edgeTypeList.vecDump();

    // Then for b trees, loop through vertex types and pass them as 'name' to btree file constructor.
    // The btree class will handle the rest
    for(std::string vertexType : vertexTypes) {
        bTreeArray.push_back(bTree(vertexType, 0));
    }

    // No need to do anything for adj lists, they will be created/read/modified on the fly
}

int graph::getIndexOfTypeOfEdge(std::string _edgeTypelabel) {
    int ans = edgeTypeList.findIndex(_edgeTypelabel);
    if (ans == -1) {
        edgeTypeList.insert(_edgeTypelabel);
        makeDir("_data/adjLists/", _edgeTypelabel);
        std::ofstream file("_data/adjLists/" + _edgeTypelabel + "/infofile.txt");
        file.close();
        ans = edgeTypeList.getSize() - 1;
    }
    return ans;
}

bool graph::addEdge (int transactionID, std::string relation, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type) {

    int vertex1Type = vertexTypeList.findIndex(_vertex1Type);
    int vertex2Type = vertexTypeList.findIndex(_vertex2Type);

    if(vertex1Type == -1 or vertex2Type == -1) {
        answerQueue.push({transactionID, "Failure: Vertex type doesn't exist, so edge can't exist either."});
        return false;
    }

    if (bTreeArray[vertex1Type].search(_vertex1ID) == -1 or bTreeArray[vertex2Type].search(_vertex2ID) == -1) {
        answerQueue.push({transactionID, "Failure: Vertex doesn't exist, can't add edge to it."});
        return false;
    }

    std::string completeLabel = relation + "-" + _vertex1Type + "_" + _vertex2Type;

    int edgeType = getIndexOfTypeOfEdge(completeLabel);

    //go to that directory's infofile.txt and check if the vertex1ID exists in it
    //i.e if there is an adjList element where fromNode is vertex1ID
    std::string dir = "_data/adjLists/" + completeLabel + "/";
    LL info(dir + "infofile.txt");

    bool vertex1Exists = info.exists(_vertex1ID);
    bool vertex2Exists = info.exists(_vertex2ID);

    // if vertex1ID doesn't exist in the infofile.txt, create a new file for it and add vertex2ID to it
    if (!vertex1Exists) {
        std::ofstream file(dir + _vertex1ID + ".txt");
        file << _vertex2ID << "\n";
        file.close();
        info.insert(_vertex1ID);
    } else {
        LL vertex1AdjList(dir + _vertex1ID + ".txt");
        if (vertex1AdjList.exists(_vertex2ID)) {
            answerQueue.push({transactionID, "Failure: unidirectional edge already exists, can't add it again."});
            return false;
        } else {
            vertex1AdjList.insert(_vertex2ID);
            vertex1AdjList.writeToFile(dir + _vertex1ID + ".txt");
        }
    }

    if (bidirectional) {
        // we've already guaranteed that vertex1 and vertex2 are real.
        // and if the relation is bidrectional, it's almost certain that vertex1ID won't exist in vertex2ID's adjList
        // so we can just append vertex1ID to vertex2ID's adjList without checking

        if (!vertex2Exists) {
            info.insert(_vertex2ID);
            std::ofstream file(dir + _vertex2ID + ".txt");
            file << _vertex1ID << "\n";
            file.close();
        } else {
            LL vertex2AdjList(dir + _vertex2ID + ".txt");
            vertex2AdjList.insert(_vertex1ID);
            vertex2AdjList.writeToFile(dir + _vertex2ID + ".txt");
        }
    }
    
    info.writeToFile(dir + "infofile.txt");

    std::string feedback = "Success: edge added ";
    feedback += (bidirectional ? "bidirectionally" : "unidirectionally");
    answerQueue.push({transactionID, feedback});
    return true;
}

int graph::getIndexOfTypeOfVertex(std::string _vertexTypeLabel) {
    int ans = vertexTypeList.findIndex(_vertexTypeLabel);
    if (ans == -1) {
        vertexTypeList.insert(_vertexTypeLabel);
        bTree newT (_vertexTypeLabel);
        bTreeArray.push_back(newT);
        makeDir("_data/bTrees/", _vertexTypeLabel);
        ans = vertexTypeList.getSize() - 1;
    }
    return ans;
}

bool graph::addVertex (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties) {
    
    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);
    if (bTreeArray[vertexType].search(uniqueKey) != -1) {
        answerQueue.push({transactionID, "Failure: Vertex already exists, can't add it again."});
        return false;
    }

    bTreeArray[vertexType].insert(uniqueKey);

    //_vertexProperties is in form: "key1:value1~key2:value2~...keyN:valueN~"
    std::string writePath = "_data/vertexProperties/" + uniqueKey + ".bin";
    std::ofstream file(writePath, std::ios::binary);
    
    checkStringForEncryptables(_vertexProperties, 0);
    
    int stringSize = _vertexProperties.size();
    file.write((char*)&stringSize, sizeof(int));

    file.write(&_vertexProperties[0], _vertexProperties.size());
    file.close();

    answerQueue.push({transactionID, "Success: Vertex added"});
    return true;
}

std::string graph::fetchProperties (std::string uniqueKey) {
    //This function assumes that the vertex exists

    std::ifstream file("_data/vertexProperties/" + uniqueKey + ".bin", std::ios::binary);

    int stringSize;
    file.read((char*)&stringSize, sizeof(int));

    std::string properties; 
    properties.resize(stringSize);
    file.read(&properties[0], stringSize);

    checkStringForEncryptables(properties, 1);

    file.close();
    return properties;
}

void graph::updateVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties) {
    
    std::string readPath = "_data/vertexProperties/" + uniqueKey + ".bin";
    std::ifstream file(readPath, std::ios::binary);
    
    int fileSize;
    file.read((char*)&fileSize, sizeof(int));

    std::string oldProperties; oldProperties.resize(fileSize);
    file.read(&oldProperties[0], fileSize);
    file.close();

    // Convert both strings into hashmaps
    // They're in the format: "key1:value1~key2:value2~...keyN:valueN~"
    // Split them by '~' and then by ':'
    std::unordered_map<std::string, std::string> oldPropertiesMap, newPropertiesMap;

    std::stringstream oldProp (oldProperties);
    std::string keyValueString;
    while (std::getline (oldProp, keyValueString, '~')) {
        std::string key, value;
        std::stringstream ss2(keyValueString);
        std::getline(ss2, key, ':');
        std::getline(ss2, value, ':');
        oldPropertiesMap[key] = value;
    }

    std::stringstream newProp(newProperties);
    while (std::getline(newProp, keyValueString, '~')) {
        std::string key, value;
        std::stringstream ss4(keyValueString);
        std::getline(ss4, key, ':');
        std::getline(ss4, value, ':');

        if (key[0] == '!') //e.g `!password`
            badHasher(value);

        newPropertiesMap[key] = value;
    }

    for (auto it = newPropertiesMap.begin(); it != newPropertiesMap.end(); it++)
        oldPropertiesMap[it->first] = it->second;

    std::string properties;
    for (auto it = oldPropertiesMap.begin(); it != oldPropertiesMap.end(); it++)
        properties += it->first + ":" + it->second + "~";

    std::ofstream file2(readPath, std::ios::binary);
    int stringSize = properties.size();
    file2.write((char*)&stringSize, sizeof(int));
    file2.write(&properties[0], stringSize);
    file2.close();

    answerQueue.push({transactionID, "Success: Vertex updated"});
}

void graph::mergeVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties) {

    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

    if (bTreeArray[vertexType].search(uniqueKey) == -1)
        addVertex (transactionID, uniqueKey, _vertexTypeLabel, _vertexProperties);
    else
        updateVertex (transactionID, uniqueKey, _vertexTypeLabel, _vertexProperties);
}

#endif //GRAPH_H
