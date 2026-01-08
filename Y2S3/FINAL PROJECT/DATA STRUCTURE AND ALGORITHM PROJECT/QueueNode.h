#ifndef QUEUENODE_H
#define QUEUENODE_H

#include <string>

struct QueueNode {
    std::string patientID;
    QueueNode* next;

    QueueNode(const std::string& id) : patientID(id), next(nullptr) {}
};

#endif
