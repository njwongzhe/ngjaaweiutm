#include "Queue.h"
#include <iostream>
#include <vector>

Queue::Queue() : front(nullptr), rear(nullptr), size(0) {}

Queue::~Queue() { clear(); }

bool Queue::isEmpty() const { return front == nullptr; }

int Queue::getSize() const { return size; }

void Queue::enqueue(const std::string& patientID) {
    QueueNode* newNode = new QueueNode(patientID);
    if (isEmpty()) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    size++;
}

std::string Queue::dequeue() {
    if (isEmpty()) return "";
    QueueNode* currNode = front;
    std::string id = front->patientID;
    front = front->next;
    if (!front) rear = nullptr;
    delete currNode;
    size--;
    return id;
}

std::string Queue::peek() const {
    if (isEmpty()) return "";
    return front->patientID;
}

void Queue::displayQueue() const {
    if (isEmpty()) {
        std::cout << "Queue is empty.\n";
        return;
    }
    QueueNode* currNode = front;
    std::cout << "Current Queue (Size: " << size << "):\n";
    int position = 1;
    while (currNode) {
        std::cout << position << ". " << currNode->patientID << "\n";
        currNode = currNode->next;
        position++;
    }
}

void Queue::clear() {
    while (!isEmpty()) {
        dequeue();
    }
}

std::vector<std::string> Queue::getAllPatientIDs() const {
    std::vector<std::string> patientIDs;
    QueueNode* currNode = front;
    while (currNode) {
        patientIDs.push_back(currNode->patientID);
        currNode = currNode->next;
    }
    return patientIDs;
}