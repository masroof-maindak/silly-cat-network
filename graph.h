/* “An idiot admires complexity, a genius admires simplicity" ~ Terry Davis. */

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "lib/bTree.h"
#include "lib/LinkedList.h"
#include "lib/encryption.h"

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
    int getIndexOfTypeOfEdge(std::string label);

    // Write all the data structures to disk
    void dumpGraphData();

    // Used in merge()
    void updateVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties);

    // Used to check properties of a vertex, used in filtering
    std::string fetchProperties(std::string uniqueKey);

    // Genereates a hashmap of properties to match from a string of properties
    std::unordered_map<std::string, std::string> generatePropertiesToMatch(std::string _propertiesToMatch);

public:

    // Constructors
    graph() {}                // default - does nothing really
    graph(int fileCheckFlag); // file-based - inits graph from files

    bool addVertex (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);
    bool addEdge (int transactionID, std::string _edgeTypeLabel, bool bidirectional,
                  std::string _vertex1ID, std::string _vertex2ID, 
                  std::string _vertex1Type, std::string _vertex2Type);
    void mergeVertex (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties);

    bool removeVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel);
    bool removeEdge(int transactionID, std::string _edgeTypeLabel, bool bidirectional,
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
    *   Application backend (is client + generates perfect commands) + frontend
    */

   ~graph() { dumpGraphData(); }
};

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
                    //if the values don't match, break out of the loop as this vertex is invalid now anyway
                    if (propertiesToMatch[key] != value) {
                        match = false;
                        break;
                    }
                }            
            }
    
            //if match is true though, i.e all the values satisfy the requirements set by the user, add the node to retNodeList
            if (match)
                retVertexList += vertex + "~";
    }

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

    //first acquire all nodes of that type
    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

    //open the btree file for that type
    std::vector<std::string> vertexList = bTreeArray[vertexType].dump();

    //loop through vector of nodes, open their files, check if they have the properties we want, if they do, add them to retNodeList
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

        while (std::getline(ss, keyValueString, '~')) {
            //split the keyValueString string by ':'
            std::string key, value;
            std::stringstream ss2(keyValueString);
            std::getline(ss2, key, ':');
            std::getline(ss2, value, ':');

            //if the key exists in propertiesToMatch, check if the values match
            if (propertiesToMatch.find(key) != propertiesToMatch.end()) {
                //if the values don't match, break out of the loop as this vertex is invalid now anyway
                if (propertiesToMatch[key] != value) {
                    match = false;
                    break;
                }
            }            
        }

        //if match is true though, i.e all the values satisfy the requirements set by the user, add the node to retNodeList
        if (match)
            retVertexList += vertex + "~";
    }

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
    if (!vertex1AdjList.erase(_vertex2ID)) {
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
        if (!vertex2AdjList.erase(_vertex1ID)) {
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

void graph::dumpGraphData() {
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

int graph::getIndexOfTypeOfEdge(std::string label) {
    int ans = edgeTypeList.findIndex(label);
    if (ans == -1) {
        edgeTypeList.insert(label);
        makeDir("_data/adjLists/", label);
        std::ofstream file("_data/adjLists/" + label + "/infofile.txt");
        file.close();
        ans = edgeTypeList.getSize() - 1;
    }
    return ans;
}

bool graph::addEdge (int transactionID, std::string relation, bool bidirectional, std::string _vertex1ID, std::string _vertex2ID, std::string _vertex1Type, std::string _vertex2Type) {

    // Check if vertex1 and vertex2 exist in our bTrees
    int vertex1Type = vertexTypeList.findIndex(_vertex1Type);
    int vertex2Type = vertexTypeList.findIndex(_vertex2Type);

    //if either of the vertex types don't exist, what are we supposed to be adding an edge to?
    if(vertex1Type == -1 or vertex2Type == -1) {
        answerQueue.push({transactionID, "Failure: Vertex type doesn't exist, so edge can't exist either."});
        return false;
    }

    //check if both vertices exist in their respective bTrees
    if (bTreeArray[vertex1Type].search(_vertex1ID) == -1 or bTreeArray[vertex2Type].search(_vertex2ID) == -1) {
        answerQueue.push({transactionID, "Failure: Vertex doesn't exist, can't add edge to it."});
        return false;
    }

    // label is brought to format "Relation-typeX_typeY"
    std::string completeLabel = relation + "-" + _vertex1Type + "_" + _vertex2Type;

    // first identify the TYPE of the edge from the edgeTypeList, this will also make the directory for that edge if it doesn't exist
    int edgeType = getIndexOfTypeOfEdge(completeLabel);

    //go to that directory's infofile.txt and check if the vertex1ID exists in it
    //i.e if there is an adjList element where fromNode is vertex1ID
    std::string dir = "_data/adjLists/" + completeLabel + "/";
    LL info(dir + "infofile.txt");

    bool vertex1Exists = info.exists(_vertex1ID);
    bool vertex2Exists = info.exists(_vertex2ID);

    // if vertex1ID doesn't exist in the infofile.txt, create a new file for it and add vertex2ID to it
    if (!vertex1Exists) {

        //create a new file for vertex1ID
        std::ofstream file(dir + _vertex1ID + ".txt");
        
        //add vertex2ID to it
        file << _vertex2ID << "\n";
        file.close();
        
        //add vertex1ID to info LL
        info.insert(_vertex1ID);

    } else {
        //open the file for vertex1ID
        LL vertex1AdjList(dir + _vertex1ID + ".txt");

        //if vertex2ID already exists in vertex1's adjList, return false
        if (vertex1AdjList.exists(_vertex2ID)) {
            answerQueue.push({transactionID, "Failure: unidirectional edge already exists, can't add it again."});
            return false;
        }

        //else insert it
        else {
            vertex1AdjList.insert(_vertex2ID);

            // and write changes to disk
            vertex1AdjList.writeToFile(dir + _vertex1ID + ".txt");
        }
    }

    if (bidirectional) {
        //we've already guaranteed that vertex1 and vertex2 are real.
        //and if the relation is bidrectional, it's almost certain that vertex1ID won't exist in vertex2ID's adjList
        //so we can just append vertex1ID to vertex2ID's adjList without checking

        if (!vertex2Exists) {
            //insert it in info file
            info.insert(_vertex2ID);

            //create a new file for vertex2ID
            std::ofstream file(dir + _vertex2ID + ".txt");

            //add vertex1ID to it
            file << _vertex1ID << "\n";
            file.close();

        } else {
            //open the file for vertex2ID
            LL vertex2AdjList(dir + _vertex2ID + ".txt");

            //insert vertex1ID
            vertex2AdjList.insert(_vertex1ID);

            // and write changes to disk
            vertex2AdjList.writeToFile(dir + _vertex2ID + ".txt");
        }
    }
    
    //update info file
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

        bTree testT = bTreeArray[bTreeArray.size() - 1];
        
        
        makeDir("_data/bTrees/", _vertexTypeLabel);
        ans = vertexTypeList.getSize() - 1;
    }
    return ans;
}

bool graph::addVertex (int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties) {

    // Identify the TYPE of the vertex from the vertexTypeList
    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

    // Search in B tree array at vertexType-th index and check if that vertex exists already
    if (bTreeArray[vertexType].search(uniqueKey) != -1) {
        answerQueue.push({transactionID, "Failure: Vertex already exists, can't add it again."});
        return false;
    }

    bTreeArray[vertexType].insert(uniqueKey);

    //_vertexProperties is in form: "key1:value1~key2:value2~...keyN:valueN~"
    std::string writePath = "_data/vertexProperties/" + uniqueKey + ".bin";
    std::ofstream file(writePath, std::ios::binary);
    
    //scan string for a value that starts with '!', replace it with its output after badHasher()
    checkStringForEncryptables(_vertexProperties, 0);
    
    //write string size
    file.write((char*)_vertexProperties.size(), sizeof(int));

    //write properties string
    file.write(_vertexProperties.c_str(), _vertexProperties.size());
    file.close();

    answerQueue.push({transactionID, "Success: Vertex added"});
    return true;
}

std::string graph::fetchProperties (std::string uniqueKey) {
    //This function assumes that the vertex exists, and is called only after checking that it does
    std::ifstream file("_data/vertexProperties/" + uniqueKey + ".bin", std::ios::binary);

    //read string size
    int fileSize;
    file.read((char*)&fileSize, sizeof(int));

    //read string contents
    std::string properties; properties.resize(fileSize);
    file.read(&properties[0], fileSize);

    //decrypt data if needed
    checkStringForEncryptables(properties, 1);

    //return string
    file.close();
    return properties;
}

void graph::updateVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string newProperties) {
    
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

        //if such a 'value' exists that is prepended by '!', 
        //pass it through the hasher to encrypt it before writing
        if (key[0] == '!') //e.g !password
            badHasher(value);

        newPropertiesMap[key] = value;
    }

    // Go through through newProperties and update oldProperties
    for (auto it = newPropertiesMap.begin(); it != newPropertiesMap.end(); it++)
        oldPropertiesMap[it->first] = it->second;

    // Coalesce hashmap into a string
    std::string properties;
    for (auto it = oldPropertiesMap.begin(); it != oldPropertiesMap.end(); it++)
        properties += it->first + ":" + it->second + "~";

    // Write the string to the file
    std::ofstream file2(readPath, std::ios::binary);
    file2.write((char*)properties.size(), sizeof(int));
    file2.write(properties.c_str(), properties.size());
    file2.close();

    answerQueue.push({transactionID, "Success: Vertex updated"});
}

void graph::mergeVertex(int transactionID, std::string uniqueKey, std::string _vertexTypeLabel, std::string _vertexProperties) {

    int vertexType = getIndexOfTypeOfVertex(_vertexTypeLabel);

    // Search in B tree array at vertexType-th index and check if that vertex exists already
    if (bTreeArray[vertexType].search(uniqueKey) != -1)
        addVertex (transactionID, uniqueKey, _vertexTypeLabel, _vertexProperties);
    else
        updateVertex (transactionID, uniqueKey, _vertexTypeLabel, _vertexProperties);
}

#endif //GRAPH_H
