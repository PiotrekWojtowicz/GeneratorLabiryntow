#pragma once
#include <iostream>

typedef struct Node {
    int nodeNumber = 0;
    Node* edgeToNode = nullptr;
} Node;

class ListaSasiedztwa {
public:
    Node** matrixList;
    ListaSasiedztwa(){}
    ListaSasiedztwa(size_t height, size_t width) {
        this->SIZE = width * height;
        matrixList = new Node * [this->SIZE];

        for (int i = 0; i < this->SIZE; i++) {
            matrixList[i] = nullptr;
        }

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {

                int parentNode = i * height + j;

                if (i > 0) { // North
                    addToList(parentNode, (i - 1) * height + j);
                }
                if (i < width - 1) { // South
                    addToList(parentNode, (i + 1) * height + j);
                }
                if (j > 0) { // West
                    addToList(parentNode, i * height + (j - 1));
                }
                if (j < height - 1) { // East
                    addToList(parentNode, i * height + (j + 1));
                }
            }
        }

        //printMatrixList(matrixList, this->SIZE);
    }

    ~ListaSasiedztwa() {
        freeMatrixList(matrixList, this->SIZE);
    }
    
private:

    size_t SIZE;

    void addToList(int fromNode, int toNode) {

        Node* parent = matrixList[fromNode];
        Node* nextNode = new Node();

        nextNode->edgeToNode = nullptr;
        nextNode->nodeNumber = toNode;

        if (parent == nullptr) {
            matrixList[fromNode] = nextNode;
        }
        else {
            while (parent->edgeToNode != nullptr) {
                parent = parent->edgeToNode;
            }

            parent->edgeToNode = nextNode;
        }
    }

    void printMatrixList(Node** node, size_t n) {
        Node* temp;
        for (int i = 0; i < n; i++) {
            std::cout << "[" << i << "]\t";
            temp = node[i];
            while (temp != nullptr) {
                std::cout << " -> " << temp->nodeNumber << "\t";
                temp = temp->edgeToNode;
            }
            std::cout << std::endl;
        }
    }

    void freeMatrixList(Node** node, size_t n) {
        Node* temp;
        Node* toBeDestroyed;
        for (int i = 0; i < n; i++) {
            temp = node[i];
            while (temp != nullptr) {
                toBeDestroyed = temp;
                temp = temp->edgeToNode;
                delete toBeDestroyed;
            }
        }
        delete[]node;
    }
};