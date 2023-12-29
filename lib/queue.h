#ifndef QUEUE_H
#define QUEUE_H

template <class T>
class Queue
{
protected:
    T *arr;
    int capacity;
    int _front;
    int _back;
public:
    Queue() : arr(new T[3]), capacity(3), _front(0), _back(0) {}
    int size() {return _back - _front;}
    bool empty() {return _front == _back;}
    T front() {return arr[_front];}
    T back() {return arr[_back];}
    void push(T);
    bool pop();
    void print();    
    ~Queue() {delete[] arr;}
};

//print
template <class T>
void Queue<T>::print() {
    for(int i = _front; i < _back; i++)
        std::cout << arr[i] << "\n";
}

//push
template <class T>
void Queue<T>::push(T data) {
    if(_back == capacity) {
        capacity *= 2;
        int width = size();

        T* temp = new T[capacity];
        for(int i = 0; i < width; i++)
            temp[i] = arr[i + _front];
        delete[] arr;

        arr = temp;
        _front = 0;
        _back = width;
    }
    arr[_back++] = data;
}

//pop
template <class T>
bool Queue<T>::pop() {
    if(_front == _back)
        return false;
    _front++;
    return true;
}

#endif /* QUEUE_H */
