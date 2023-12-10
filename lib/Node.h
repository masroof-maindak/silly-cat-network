#ifndef NODE_H
#define NODE_H

template <class T>
struct Node {
    T data;
    Node<T>* next; //This points to nullptr for tail
    
    Node(); //init next with nullptr
    Node(T _data); //init data w/ _data && next w/ nullptr
    Node(Node<T>* _next); //initialize next w/ _next
    Node(T _data, Node<T>* _next); //initialize data w/ _data and next w/ _next
};

template <class T>
Node<T>::Node() : next(nullptr) {}

template <class T>
Node<T>::Node(T _data) : data(_data), next(nullptr) {}

template <class T>
Node<T>::Node(Node<T>* _next) : next(_next) {}

template <class T>
Node<T>::Node(T _data, Node<T>* _next) : data(_data), next(_next) {}

#endif