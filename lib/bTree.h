#ifndef BTREE_H
#define BTREE_H

#include <vector>
#include <fstream>
#include "bnode.h"
#include "queue.h"

#define t 3
#define MAX_CHILDREN 2*t
#define MAX_KEYS 2*t-1
#define MIN_KEYS t-1

class bTree {

protected:

    bnode* root;            //pointer to the root node (kept in memory at all times)
    int rootNodeID;         //uniqueID of the root node     - line 1 of infofile.txt
    std::string treeName;   //name of tree, used to create a directory where we can can dump all nodes + infofile.txt

    void updateRoot (bnode* newRoot) {delete root; root = newRoot; rootNodeID = root->uniqueID; writeMetadata();}
    void writeMetadata();
    int genID() {return (rand() % 1000000000) + (rand() % 1000) + 70;} 

    // Search helper
    int searchInternal(bnode* curr, std::string data); 

    // Print helper
    void printInternal(bnode* curr, int depth);

    // Insertion helper functions
    void splitChild(bnode* internalNode, int indexOfFullChildOfX);
    void insertNonFull(bnode* node, std::string data);

    // Deletion helper functions - Lord save my soul
    void removeRecursive(bnode* curr, std::string data);
    void eraseInternalNode(bnode* curr, std::string data, int index);
    void eraseSibling(bnode* curr, int index, int siblingIndex);
    void eraseMerge(bnode*& curr, int index, int siblingIndex);
    std::string erasePredecessor(bnode* curr);
    std::string eraseSuccessor(bnode* curr);

public:

    // Constructors
    bTree(std::string name) : root(new bnode(69, true)),
                              rootNodeID(69), treeName(name) {}         // default
    bTree(std::string name, int fileCheckFlag);                         // file-based

    // Insertion/Deletion
    void insert(std::string data);
    bool erase(std::string data);

    // Search
    int search(std::string data) {return searchInternal (root, data);}

    // Print
    void print() {printInternal(root, 0);}

    // Destructor
    ~bTree() {writeMetadata(); delete root;};

    // Returns array of all values in tree
    std::vector<std::string> dump();

};

std::vector<std::string> bTree::dump() {
    std::vector<std::string> retValues;
    std::string treePath = "_data/bTrees/" + treeName + "/";
    std::ifstream file (treePath + "infofile.txt");
    
    //start with root while adding its children to queue
    Queue<int> nodeIDs;

    //add root's keys to vector
    for (std::string key : root->keys)
        retValues.push_back(key);

    //add root's children to queue
    for (int ptr : root->ptrs)
        nodeIDs.push(ptr);

    while (!nodeIDs.empty()) {
        //open a child
        std::string filename = treePath + std::to_string(nodeIDs.front()) + ".txt";
        bnode* curr = new bnode(filename);
        nodeIDs.pop();

        //add all of curr's keys to vector
        for (std::string key : curr->keys)
            retValues.push_back(key);

        //add all of curr's children to queue
        for (int ptr : curr->ptrs)
            nodeIDs.push(ptr);

        //delete curr from memory
        delete curr;
    }

    return retValues;
}

//'rotate' value
void bTree::eraseSibling(bnode* curr, int index, int siblingIndex) {
    
    //open center node
    std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[index]) + ".txt";
    bnode* center = new bnode(filename);

    //taking left most child of right up and bringing separater key down to the left
    if (index < siblingIndex) {

        //open right node
        filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[siblingIndex]) + ".txt";
        bnode* right = new bnode(filename);

        //bring separator key from parent down to center        
        center->keys.push_back(curr->keys[index]);

        //take first key of right up to center where separator key was
        //and delete that key from the right sibling
        curr->keys[index] = (right->keys[0]);
        right->keys.erase(right->keys.begin());

        //remove first child from right and assign as last child of center if needed
        if (!right->isLeaf) {
            center->ptrs.push_back(right->ptrs[0]);
            right->ptrs.erase(right->ptrs.begin());
        }

        //write nodes to file
        center->writeToFile("_data/bTrees/" + treeName + "/");
        right->writeToFile("_data/bTrees/" + treeName + "/");

        //delete from memory
        delete right;

    //taking right most child of left up and bringing separator key down to the right
    } else {

        //open left node
        filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[siblingIndex]) + ".txt";
        bnode* left = new bnode(filename);

        //bring separator key from parent down to center
        center->addKey(curr->keys[index - 1]);

        //take last child from left up to center at index where separator key was
        //and delete that child from left sibling
        curr->keys[index - 1] = left->keys[left->keys.size() - 1];
        left->keys.resize(left->keys.size() - 1);

        //write nodes to file
        center->writeToFile("_data/bTrees/" + treeName + "/");
        left->writeToFile("_data/bTrees/" + treeName + "/");

        //delete from memory
        delete left;
    }

    //update parent's file and delete from memory
    delete center;
    curr->writeToFile("_data/bTrees/" + treeName + "/");
}

//merge two children of curr & median key of curr into big kid
void bTree::eraseMerge (bnode*& curr, int index, int siblingIndex) {

    //open center node
    std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[index]) + ".txt";
    bnode* center = new bnode(filename);

    bnode* newNode;

    //merge right into center
    if (siblingIndex > index) {

        //open right node
        filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[siblingIndex]) + ".txt";
        bnode* right = new bnode(filename);

        //append separator key from parent to center
        //and delete that key from parent
        center->keys.push_back(curr->keys[index]);
        curr->keys.erase(curr->keys.begin() + index);

        //append all keys from right to center
        for(auto key : right->keys)
            center->keys.push_back(key);

        //if center is not a leaf, append all ptrs from right to center
        if (!center->isLeaf)
            for(auto ptr : right->ptrs)
                center->ptrs.push_back(ptr);

        //delete right node, update files
        newNode = center;
        delete right;
        
        //erase ptr from parent that was originally pointing to right child
        int rightPtr = curr->ptrs[siblingIndex];
        curr->ptrs.erase(curr->ptrs.begin() + siblingIndex);

        //delete file that belonged to right child
        std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(rightPtr) + ".txt";
        remove(filename.c_str());

        //write to file
        newNode->writeToFile("_data/bTrees/" + treeName + "/");
        curr->writeToFile("_data/bTrees/" + treeName + "/");
    }

    //merge center into left
    else {
        
        //open left node
        filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[siblingIndex]) + ".txt";
        bnode* left = new bnode(filename);

        //insert separator key from parent into left
        left->keys.push_back(curr->keys[siblingIndex]);

        //append all keys from center to left
        for(auto key : center->keys)
            left->keys.push_back(key);

        //if left is not a leaf, append all ptrs from center to left
        if (!left->isLeaf)
            for(auto ptr : center->ptrs)
                left->ptrs.push_back(ptr);

        //delete center node, update files  
        newNode = left;
        delete center;

        //erase key from parent that was moved to left
        curr->keys.erase(curr->keys.begin() + siblingIndex);

        //erase ptr from parent that was originally pointing to center child
        int centerPtr = curr->ptrs[index];
        curr->ptrs.erase(curr->ptrs.begin() + index);

        //delete file that belonged to center child
        std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(centerPtr) + ".txt";
        remove(filename.c_str());

        //write to file
        newNode->writeToFile("_data/bTrees/" + treeName + "/");
        curr->writeToFile("_data/bTrees/" + treeName + "/");
    }

    //if curr is root and curr has been emptied, delete it and set the child as root
    if (curr == root and curr->keys.empty()) {
        //delete root file
        std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->uniqueID) + ".txt";
        remove(filename.c_str());

        curr = newNode;
        updateRoot(newNode);
    }
}

void bTree::eraseInternalNode(bnode* curr, std::string data, int index) {
    
    //if leaf, delete data and update file
    if (curr->isLeaf) {
        if (curr->keys[index] == data) {
            curr->keys.erase(curr->keys.begin() + index);
            curr->writeToFile("_data/bTrees/" + treeName + "/");
        }
        return;
    }

    //open left
    std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[index]) + ".txt";
    bnode* left = new bnode(filename);

    //open right
    filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[index + 1]) + ".txt";
    bnode* right = new bnode(filename);

    //if child has more than t keys, replace data with predecessor
    if (left->keys.size() > MIN_KEYS) {
        std::string predecessor = erasePredecessor(left);
        curr->keys[index] = predecessor;
        curr->writeToFile("_data/bTrees/" + treeName + "/");
        delete left;
        return;

    } else if (right->keys.size() > MIN_KEYS) {
        std::string successor = eraseSuccessor(right);
        curr->keys[index] = successor;
        curr->writeToFile("_data/bTrees/" + treeName + "/");
        delete right;
        return;

    } else {
        
        eraseMerge(curr, index, index + 1);

        // have to re-open left now because separator key, and right were merged into it.
        delete left; delete right;
        
        filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[index]) + ".txt";
        left = new bnode(filename);

        //Propagate down to left child and delete data, till we hit a leaf
        eraseInternalNode(left, data, t - 1);
    }

    delete left; 
    delete right;
}

std::string bTree::erasePredecessor (bnode* curr) {

    if (curr->isLeaf) {
        //delete last key, update file, return string
        std::string predecessor = curr->keys[curr->keys.size() - 1];
        curr->keys.erase(curr->keys.begin() + curr->keys.size() - 1);
        curr->writeToFile("_data/bTrees/" + treeName + "/");
        return predecessor;
    }

    //n = index of last child
    int n = curr->keys.size() - 1;

    //open child
    std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[n]) + ".txt";
    bnode* child = new bnode(filename);

    if (child->keys.size() > MIN_KEYS)
        eraseSibling(curr, n + 1, n);
    
    else
        eraseMerge(curr, n, n + 1);

    std::string predecessor = erasePredecessor(child);
    delete child;
    return predecessor;
}

std::string bTree::eraseSuccessor (bnode* curr) {

    if (curr->isLeaf) {
        //delete first key, update file, return string
        std::string successor = curr->keys[0];
        curr->keys.erase(curr->keys.begin());
        curr->writeToFile("_data/bTrees/" + treeName + "/");
        return successor;
    }

    //open child
    std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[1]) + ".txt";
    bnode* child = new bnode(filename);

    if (child->keys.size() > MIN_KEYS)
        eraseSibling(curr, 0, 1);
    
    else
        eraseMerge(curr, 0, 1);

    delete child;

    //open child2
    filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[0]) + ".txt";
    bnode* child2 = new bnode(filename);

    std::string succesor = eraseSuccessor(child2);
    delete child2;
    return succesor;
}

void bTree::removeRecursive(bnode* curr, std::string data) {

    //find index of data in curr
    int count = 0;
    while (count < curr->keys.size() and data > curr->keys[count])
        count++;
    
    //if curr is a leaf...
    if (curr->isLeaf) {
        //if value to delete found
        if (count < curr->keys.size() and data == curr->keys[count]) {\
            //erase data and update file
            curr->keys.erase(curr->keys.begin() + count);
            curr->writeToFile("_data/bTrees/" + treeName + "/");
        }
        return; //either we've successfully deleted or hit a dead end. In which case, return.
    }
    
    //If we are not in a leaf,
    //open curr's child at the count-th index (i.e value exists down this path...)
    std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[count]) + ".txt";
    bnode* child = new bnode(filename);

    //if value to delete found in curr and curr isn't a leaf, call eraseInternalNode and we're done
    if (count < curr->keys.size() and data == curr->keys[count]) {
        return eraseInternalNode(curr, data, count);
    
    //shortcut: if child has more than t keys, recurse down it
    } else if (child->keys.size() > MIN_KEYS) {
        return removeRecursive(child, data);

    } else {

        if (count != 0 and (count + 2) < curr->keys.size()) {
            
            //load in left sibling of child
            filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[count - 1]) + ".txt";
            bnode* leftSibling = new bnode(filename);

            //load in right sibling of child
            filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[count + 1]) + ".txt";
            bnode* rightSibling = new bnode(filename);

            //if left sibling has more than t keys, erase from it
            if (leftSibling->keys.size() > MIN_KEYS)
                eraseSibling(curr, count, count - 1);
            
            //if right sibling has more than t keys, erase from it
            else if (rightSibling->keys.size() > MIN_KEYS)
                eraseSibling(curr, count, count + 1);

            //if both siblings have t-1 keys, merge with one of them
            else
                eraseMerge(curr, count, count + 1);

            //delete siblings from memory
            delete leftSibling; delete rightSibling;

        //child node lies on leftmost end of curr (therefore has no left sibling)
        } else if (count == 0) {
            
            //load in right sibling of child
            filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[count + 1]) + ".txt";
            bnode* rightSibling = new bnode(filename);

            //if right sibling has more than minimum req. keys, erase from it (i.e rotate left)
            if (rightSibling->keys.size() > MIN_KEYS)
                eraseSibling(curr, 0, 1);

            //if not, merge with it
            else
                eraseMerge(curr, 0, 1);

            //free memory
            delete rightSibling;

        //child node lies on rightmost end of curr (therefore has no right sibling)
        } else if (count + 1 == curr->keys.size()) {

            //load in left sibling of child
            filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[count - 1]) + ".txt";
            bnode* leftSibling = new bnode(filename);

            //if left sibling has more than minimum req. keys, erase from it (i.e rotate right)
            if (leftSibling->keys.size() > MIN_KEYS)
                eraseSibling(curr, count, count - 1);

            //if not, merge with it
            else
                eraseMerge(curr, count, count - 1);

            //free memory
            delete leftSibling;

        }

        //we must load the child in again because it could have been altered as a result of some merge/split operation above
        delete child;
        filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[count]) + ".txt";
        child = new bnode(filename);

        return removeRecursive(child, data);
    }
}

bool bTree::erase(std::string data) {
    if (searchInternal(root, data) == -1)
        return false;
    else {
        removeRecursive(root, data);
        return true;
    }
}

void bTree::printInternal(bnode* curr, int depth) {
    std::cout << "Depth " << depth << " | ";

    //print curr's children
    for(int i = 0; i < curr->keys.size(); i++) {
        std::cout << curr->keys[i] << " ";
    }

    std::cout << "\n";
    depth++;

    //if curr is not a leaf, print its children
    if (!curr->isLeaf) {
        for (int i = 0; i < curr->ptrs.size(); i++) {
            std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[i]) + ".txt";
            bnode* child = new bnode(filename);
            printInternal(child, depth);
            delete child;
        }
    }
}

void bTree::insert(std::string data) {
    //start with root
    bnode* curr = root;

    //if root is full, create a new node
    if (curr->keys.size() == MAX_KEYS) {
        bnode* newRoot = new bnode(genID(), false); //create new root with false leaf and randomID
        newRoot->ptrs.push_back(curr->uniqueID);    //add prev root as child of new root
        updateRoot(newRoot);                        //update root pointer, delete old root from memory, update infofile
        splitChild(newRoot, 0);                     //split prev root and move 1 key to new root
        insertNonFull(newRoot, data);               //insert data into new root
    } else {
        insertNonFull(curr, data);
    }
}

void bTree::splitChild (bnode* internalNode, int i) {
    std::string treePath = "_data/bTrees/" + treeName + "/";
    // i = index of the child of internalNode that is full

    //access this child of the internalNode, lets call it y
    std::string filename = treePath + std::to_string(internalNode->ptrs[i]) + ".txt";
    bnode* fullChild = new bnode(filename);

    //create a new node, z, and add it to x's list of children
    bnode* newNode = new bnode(genID(), fullChild->isLeaf);
    internalNode->ptrs.push_back(newNode->uniqueID);

    //insert the MEDIAN key of y into internalNode
    internalNode->addKey(fullChild->keys[t-1]);

    //shift the last t-1 keys of the fullchild to the new node
    for (int j = 0; j < t-1; j++)
        newNode->keys.push_back(fullChild->keys[j+t]);

    //keep the first t-1 keys as is in the fullchild
    fullChild->keys.resize(t-1);

    //if fullChild is an internal node, shift it's last t CHILDREN to the new node
    if (!fullChild->isLeaf) {
        for (int j = 0; j < t; j++)
            newNode->ptrs.push_back(fullChild->ptrs[j+t]);
        fullChild->ptrs.resize(t);
    }

    //write all 3 nodes to their respective files
    internalNode->writeToFile(treePath);
    fullChild->writeToFile(treePath);
    newNode->writeToFile(treePath);

    // delete children from memory
    delete fullChild; delete newNode;
    // not the internal node because it might be the root
}

void bTree::insertNonFull (bnode* node, std::string data) {
    //Recursively propagate down the tree until we find a leaf to insert to, 
    //if the child we want to propagate to is full, split it

    std::string treePath = "_data/bTrees/" + treeName + "/";

    int count = node->keys.size() - 1;

    //if node is a leaf...
    if (node->isLeaf) {

        //insert data into node, write, and we're done
        node->keys.push_back("");
        while (count >= 0 and data < node->keys[count]) {
            node->keys[count + 1] = node->keys[count];
            count--;
        }
        node->keys [count+1] = data;

        node->writeToFile(treePath);

    //if not a leaf
    } else {

        //find the child node where data should be inserted
        while (count >= 0 and data < node->keys[count])
            count--;
        count++;

        //open it
        std::string filename = treePath + std::to_string(node->ptrs[count]) + ".txt";
        bnode* child = new bnode(filename);

        //if it has full children, split it (pre-emptive splitting)
        if (child->keys.size() == MAX_KEYS) {
            splitChild(node, count);
            // if data is greater than the key we just inserted into node 
            // And we know a key was inserted because split child inserts the median
            // increment count, to move one index to the right
            if (data > node->keys[count])
                count++;
            delete child;
        }

        // open the child again, since it might have changed after the conditional above
        filename = treePath + std::to_string(node->ptrs[count]) + ".txt";
        child = new bnode(filename);

        //recursively insert data into child
        insertNonFull(child, data);

        //delete child from memory
        delete child;
    }
}

// recursively searching a b tree for a value
int bTree::searchInternal (bnode* curr, std::string data) {
    int count = 0;

    while (count < curr->keys.size() and data > curr->keys[count])
        count++;

    if (data == curr->keys[count] and count < curr->keys.size()) {
        return curr->uniqueID;
    } else if (curr->isLeaf) {
        return -1;
    } else {
        std::string filename = "_data/bTrees/" + treeName + "/" + std::to_string(curr->ptrs[count]) + ".txt";
        bnode* newNode = new bnode(filename);
        return searchInternal (newNode, data);
        delete newNode;
    }

    return -1;
}

// File constructor: Loads in nodeCount, rootNodeID, and subsequently the root node
bTree::bTree(std::string name, int fileCheckFlag) {
    treeName = name;
    std::string treePath = "_data/bTrees/" + treeName + "/";
    std::ifstream file (treePath + "infofile.txt");
    if (file.is_open()) {
        
        //read in root node ID and number of bnodes in the tree
        file >> rootNodeID;
        file.close();

        // read root node into memory from separate file
        std::string rootFileName = treePath + std::to_string(rootNodeID) + ".txt";
        root = new bnode(rootFileName);

    } else {
        std::cout << "Unable to open file for reading";
    }
}

// write the root node's uniqueID in the tree to infofile.txt
void bTree::writeMetadata () {
    std::ofstream file ("_data/bTrees/" + treeName + "/" + "infofile.txt");
    if (file.is_open()) {
        file << rootNodeID;
        file.close();
    } else {
        std::cout << "Unable to open file for writing";
    }
}

#endif
