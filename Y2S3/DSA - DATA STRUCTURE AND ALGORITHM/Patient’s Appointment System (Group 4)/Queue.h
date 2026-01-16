#ifndef QUEUE_H
#define QUEUE_H

#include "QueueNode.h"
#include <vector>
#include <string>

class Queue {
private:
    QueueNode* front;
    QueueNode* rear;
    int size;

public:
    Queue();
    ~Queue();

    bool isEmpty() const;
    int getSize() const;

    void enqueue(const std::string& patientID);
    std::string dequeue();
    std::string peek() const;

    void displayQueue() const;
    void clear();

    // For file I/O
    std::vector<std::string> getAllPatientIDs() const;
    QueueNode* getFront() const { return front; }
};

#endif