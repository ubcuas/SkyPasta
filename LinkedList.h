//
// Created by Jonathan Hirsch on 3/1/20.


// Double Ended LinkedList capable of switching out the head
// and removing references from all entries between the old head and the new head.
//

#ifndef SKYPASTA_LINKEDLIST_H
#define SKYPASTA_LINKEDLIST_H

template <class T>
class LinkedList {
private:
    struct Node{
        Node *next = nullptr;
        Node *previous = nullptr;
        T data;
    };

    Node *head = nullptr;
    Node *tail = nullptr;

public:
    LinkedList(){;};
    ~LinkedList(){};
    void setHead(Node *node);
    void pop();
    void push(const T data);
    bool empty(){return head == nullptr;};
    Node *front(){return head;};
};

template<class T>
void LinkedList<T>::push(const T data){
    Node *newNode = new Node();
    newNode->data = data;
    if (head == nullptr){
        head = newNode;
    }
    if (tail != nullptr){
        tail->next = newNode;
        newNode->previous = tail;
    }
    tail = newNode;
}

template<class T>
void LinkedList<T>::pop(){
    if (!empty()) {
        if (head->next) {
            head = head->next;
            if (head->previous) {
                head->previous = nullptr;
            }
        } else{
            head = nullptr;
        }
    }
}

template<class T>
void LinkedList<T>::setHead(Node *node) {
    head = node;
    if (node->previous) {
        node->previous = nullptr;
    }
}

#endif //SKYPASTA_LINKEDLIST_H
