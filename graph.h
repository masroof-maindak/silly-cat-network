#include <iostream>
#include "lib/LinkedList.h"

struct nodeDatum {
    std::string uniqueKey;
    int cumulativeID;
};

template <class T>
class graph {
private:
    
    //here's the plan:
    // we're maintaining all the node IDs in a file somewhere, and keeping it all in memory at first
    // we're not maintaining the edges explicitly at all, we'll just have an adjacency matrix with 0 depicting no edge and some number 'x' depicting an edge
    // the value of 'x' will vary depending on what kind of edge it is. So how do we know what edge type 'x' maps to? we'll have a separate list of edge types

    //List of all node IDs
    //we can afford to keep this in memory, since it's just a list of strings
    //inner layer of the list is the list of all the nodes of a particular type
    //Note that the ID of a node is unique across all nodes in its type
    LinkedList<LinkedList<nodeDatum>> nodeData;

    //list of all the node types
    LinkedList<std::string> nodeTypeList;

    //list of the all the edge types
    LinkedList<std::string> edgeTypeList;

    //adjacency matrix
    std::vector<std::vector<int>> adjMatrix;

    //used for assigning cumulative IDs to nodes
    int totalNodeCount;

    //so, what are all the files that we need for (sort of) metadata?
    //  1. nodeTypes.txt #just a sequential list of all the node types
    //  2. edgeTypes.txt #just a sequential list of all the edge types
    //  3. nodeIDs.txt #the general format of this file is as follows: if a line starts with an exclamation mark, then it's the name of a node type, otherwise it's the ID of a node
    //  4. adjMatrix.txt #just a matrix of numbers, where each number represents the type of edge between the nodes at the corresponding row and column

    //So then what is our 'actual' data?
    //  That would be the data of all the nodes.
    //  we can make a separate file for each node type, and store the data there sequentially
    //  e.g if there are three nodetypes, A, B, C, then we'll have three files, A.txt, B.txt, C.txt
    //  and each file will have the data of all the nodes of that type

    //Now, we can theoretically have random access to any given node from the file (std::streampos_t readPoint = indexOfThatNodeInItsNodeType * numberOfBytesRequiredToHoldOneNodeOfThatType)
    //  the drawback? we would need to stuff random data into the file for each node that doesn't have data for that field
    //  this also means that once we have defined what fields a node has, we can't add more fields to it
    //  Now, for some nodes, this doesn't matter, e.g for an account, we don't need anything except name, email, password, and maybe a profile picture
    //  but for other nodes, this is a problem, e.g for a post, we might want an array of strings for comments, and we might want to add more comments later
    //  the 'solution' is to define a maximum size for 
    //  FIGURE THIS OUT LATER (HARD DIFFICULTY)

    
    int IndexOfNodeTypeAmidstOtherTypes(std::string _nodeTypeLabel) {
        int ans = nodeTypeList.findIndex(_nodeTypeLabel);
        if (ans == -1) {
            nodeTypeList.insert(_nodeTypeLabel);
            ans = nodeTypeList.getSize();
        }
        return ans;
    }

    int findNodeCumulativeID(std::string _nodeTypeLabel, std::string _nodeID) {
        //if that type doesn't exist, we out
        int nodeTypeIndex = nodeTypeList.findIndex(_nodeTypeLabel);
        if (nodeTypeIndex == -1)
            return -1;

        nodeDatum* node = nodeIDs[nodeTypeIndex].find(_nodeID);

        //if that node doesn't exist, we out
        if (node == nullptr)
            return -1;

        return node->cumulativeID;
    }

public:
    void addNode (std::string _nodeTypeLabel, std::string uniqueKey, LinkedList<std::string> nodeData) {

        //first identify the TYPE of the node from the nodeTypeList and get its index in web
        int nodeType = IndexOfNodeTypeAmidstOtherTypes(_nodeTypeLabel);
        
        //then append it into the relevant list in the web 
        nodeData[nodeType].insert(nodeDatum(uniqueKey, totalNodeCount++));

        //then add a new row and column to the adjMatrix
        adjMatrix.push_back(std::vector<int>(adjMatrix.size(), 0));
        for (int i = 0; i < adjMatrix.size(); i++) {
            adjMatrix[i].push_back(0);
        }

        //now we need to add the data of this node into the file of its type...
        //FIGURE THIS OUT LATER (EASY DIFFICULTY)
    }

    void addEdge (std::string _edgeTypeLabel, std::string _node1ID, std::string _node2ID) {

        //first identify the TYPE of the edge from the edgeTypeList and get its index in web
        int edgeType = edgeTypeList.findIndex(_edgeTypeLabel);
        //if that edge doesn't exist
        if (edgeType == -1) {
            //add it
            edgeTypeList.insert(_edgeLabel);
            //get its index
            edgeType = edgeTypeList.getSize();
        }

        int index1 = findNodeCumulativeID(_node1ID);
        int index2 = findNodeCumulativeID(_node2ID);
        //FIGURE THIS OUT LATER (MEDIUM DIFFICULTY)

        if (index1 == -1 || index2 == -1) {
            std::cerr << "ERROR: One of the nodes doesn't exist" << std::endl;
            return;
        }

        //then add a new row and column to the adjMatrix
        //the problem is that the adjMatrix holds the nodes in the order in which they were added, so we need a way to figure out the cumulative index...
    }
};
