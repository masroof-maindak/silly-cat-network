#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Node.h"

template <class T>
class LinkedList
{
protected:
    Node<T>* head;
    Node<T>* tail;
    int size;
public:
    // Constructors
    LinkedList() : head(nullptr), tail(nullptr), size(0) {} //Empty
    LinkedList(T _data) : head(new Node<T>(_data)), tail(head), size(1) {} //list with one element
    LinkedList(int _size); // create linked list of given size
    
    // Deep Copy
    LinkedList(const LinkedList& L); // Constructor
    LinkedList& operator=(const LinkedList & L);

    // Getters
    Node<T>* begin() {return head;} //return head
    Node<T>* end() {return tail;} // return tail

    // Insertion
    void insert(T _data); //add node with data at the end of list
    void insert(T _data, int pos); //add node with data at pos-th index

    // Searching
    int findIndex(T _data); //return index of node with matching data
    Node<T>* find(T _data); //return node with matching data
    bool exists(T _data); //return node with matching _data
    T& operator[](int i); //return the data at ith node

    bool erase(Node<T>* it); //remove the matching node from list
    
    // Printing to console
    template <class U>
    friend std::ostream& operator<<(std::ostream& _cout, const LinkedList<U> &L);
    
    // Miscellaneous
    void writeToFile (std::string filename);
    int getSize() {return size;} //return size of list
    void reverse(); //reverse the linked list in O(N) time complexity
    Node<T>* getMid(); //return the middle node of the linked list
    
    // Destructor
    ~LinkedList();
};

//inherit string-only LinkedList class
class LL: public LinkedList<std::string> {
public:
    // Default Constructor
    LL() : LinkedList<std::string>() {}

    // File-based constructor
    LL(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "File to deserialize LL, '" << filename << "' not found." << std::endl;
            this->head = this->tail = nullptr;
            this->size = 0;
            return;            
        }

        this->head = this->tail = nullptr;
        this->size = 0;

        std::string line;
        while (std::getline(file, line)) {
            this->insert(line);
        }

        file.close();
    }

    //Constructor with one value
    LL(std::string _data, int dummy) : LinkedList<std::string>(_data) {}

    //search for node with matching data and erase it
    bool erase (std::string _data) {
        for (Node<std::string>* node = this->head; node != nullptr; node = node->next) {
            if (node->next->data == _data) {
                //store address of where element to be deleted is pointing towards
                Node<std::string>* whereToGoNext = node->next->next;

                //delete requested node and assign stored address to current node
                delete node->next;
                node->next = whereToGoNext;

                size--;

                return true;
            }
        }
        return false;
    }

    // Dump to vector<string>
    std::vector<std::string> vecDump() {
        std::vector<std::string> ans;
        for (Node<std::string>* node = this->head; node != nullptr; node = node->next) {
            ans.push_back(node->data);
        }
        return ans;
    }
};

//serialize
template<class T>
void LinkedList<T>::writeToFile(std::string filename) {
    std::ofstream file(filename);  
    if(!file.is_open()) {
        std::cout << "File to serialize LL to not found: " << filename << "\n";
        return;
    }

    for(Node<T>* node = head; node != nullptr; node = node->next)
        file << node->data << "\n";

    file.close();
}

//get index of a value
template<class T>
inline int LinkedList<T>::findIndex(T _data) {
    int count = 0;
    for(Node<T>* node = head; node != nullptr; node = node->next, count++)
        if(node->data == _data)
            return count;
    return -1;
}

//erase
template<class T>
inline bool LinkedList<T>::erase(Node<T>* it) {
    //if empty list
    if (head == nullptr or it == nullptr)
        return false;

    //if first element
    if(head == it) {
        Node<T>* temp = head;
        head = head->next;
        delete temp;
        size--;
        return true;
    }

    for(Node<T>* node = head; node != nullptr; node = node->next) {
        if(node->next == it) {
            //store address of where deleted element was pointing towards
            Node<T>* whereToGoNext = node->next->next;

            //delete requested node
            delete node->next;

            //assign that stored address to current node
            node->next = whereToGoNext;

            size--;

            return true;
        }
    }

    return false;
}

//cout
template <class T>
std::ostream & operator<<(std::ostream & _cout, const LinkedList<T> &L) {
    for(Node<T>* node = L.head; node != nullptr; node = node->next){
        _cout << node->data << " ";
    }
    _cout << std::endl;
    return _cout;
}

//reverse in O(n)
template <class T>
inline void LinkedList<T>::reverse() {
    //if only 1 element
    if (head == tail) {return;}
    
    Node<T>* prev;
    Node<T>* node = head->next;
    Node<T>* whereToGoNext;

    //for first iteration, make head tail!
    tail = head;
    tail->next = nullptr;
    prev = tail;
    
    while(node != nullptr) {
        //store where I was supposed to go originally
        //if I weren't reversing
        whereToGoNext = node->next;
        
        //if next == nullptr, that must mean tail encountered, which means we must set it as head now.
        if (whereToGoNext == nullptr) {
            head = node;
            node->next = prev;
            break;
        } 

        //change the address I was supposed to go to
        //to the one behind me.
        node->next = prev;

        //store where I am currently
        prev = node;
        
        //increment node to where I was originally supposed to go
        node = whereToGoNext;
    }
}

//get mid node
template <class T>
inline Node<T>* LinkedList<T>::getMid() {
    Node<T>* fast = head;
    Node<T>* slow = head;

    while (fast != nullptr) {
        fast = fast->next;

        if(fast == nullptr){
            return slow;
        }

        fast = fast->next;

        if(fast == nullptr){
            return slow;
        }

        slow = slow->next;
    }

    return nullptr;
}

//exists or not?
template <class T>
inline bool LinkedList<T>::exists(T _data) {
    for (Node<T>* node = head; node != nullptr; node = node->next)
        if (node->data == _data)
            return true;
    return false;
}

//searcher
template <class T>
inline Node<T>* LinkedList<T>::find(T _data) {
    for (Node<T>* node = head; node != nullptr; node = node->next)
        if (node->data == _data)
            return node;
    return nullptr;
}

//assignment operator
template <class T>
inline LinkedList<T> &LinkedList<T>::operator=(const LinkedList &L) {
    //check if assigning to itself
    if(this == &L)
        return *this;
    //else

    //deallocate head and tail
    head = tail = nullptr;
    
    //if L is empty...
    if(L.head == nullptr)
        return *this;

    //if L is not empty...
    for(Node<T>* node = L.head; node != nullptr; node = node->next) {
        if(head == nullptr) {
            head = new Node<T>(node->data);
            tail = head;
        } else {
            tail->next = new Node<T>(node->data);
            tail = tail->next;
        }
    }

    size = L.size;
    return *this;
}

//create empty linked list of given size
template <class T>
inline LinkedList<T>::LinkedList(int _size) {
    head = tail = nullptr;
    if(_size == 0) {return;}
    if(_size < 0) {throw std::out_of_range("Size cannot be negative");}
    for (int i = 0; i < _size; i++) {
        //If the list is empty, init a node.
        if (head == nullptr) {
            head = new Node<T>();
            tail = head;
        //else append a new node to the end of the list
        } else {
            tail->next = new Node<T>();
            tail = tail->next;
        }
    }
    size = _size;
}

//copy constructor
template <class T>
inline LinkedList<T>::LinkedList(const LinkedList &L) {
    head = tail = nullptr;
    for (Node<T>* node = L.head; node != nullptr; node = node->next) {
        if (head == nullptr) {
            // Handle the case where the list is empty
            head = new Node<T>(node->data);
            tail = head;
        } else {
            tail->next = new Node<T>(node->data);
            tail = tail->next;
        }
    }
    size = L.size;
}

//append at end
template <class T>
inline void LinkedList<T>::insert(T _data) {
    if (head == nullptr) {
        head = new Node<T> (_data);
        tail = head;
    } else {
        tail->next = new Node<T> (_data);
        tail = tail->next;
    }
    size++;
}

//append at given position
template <class T>
inline void LinkedList<T>::insert(T _data, int pos) {
    //empty list
    if(head == nullptr)
        return;

    //if pos exceeds size
    if(pos > size) {
        std::cout << "Index of insertion exceeds size." << std::endl; 
        return;
    }

    //if insertion at tail
    if(pos == size) {
        insert(_data);
        return;
    }

    //if insertion at head
    if(pos == 0) {
        //create new node and store data in it
        Node<T>* newNode = new Node<T> (_data);
        newNode->next = head;
        head = newNode;
        
    //else if normal insertion
    } else {
        Node<T>* node = head;
        int count = 0;
        while(node->next != tail and count < pos - 1) {
            node = node->next;
            count++;
        }
        //node is now behind the node that is to be replaced

        //store the node that is going to be shifted from it's position
        Node<T>* temp = node->next;

        //overwrite new node in position of predecessor
        node->next = new Node<T> (_data);

        //shift to this new node
        node = node->next;

        //make the newly installed node point to the lad it replaced.    
        node->next = temp;
    }

    //increment list size
    size++;
}

//subscript
template <class T>
inline T& LinkedList<T>::operator[](int index) {
    int count = 0;
    for (Node<T>* node = head; node != nullptr; node = node->next, count++) {
        if (index == count) {
            return node->data;
        }
    }
    throw std::out_of_range("Index out of bounds");
}

//destructor
template <class T>
inline LinkedList<T>::~LinkedList() {
    Node<T>* curr = head;
    while (curr != nullptr) {
        Node<T>* temp = curr->next;
        delete curr;
        curr = temp;
    }
}

#endif
