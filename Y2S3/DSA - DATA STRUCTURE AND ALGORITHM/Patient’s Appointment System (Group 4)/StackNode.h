#ifndef STACKNODE_H
#define STACKNODE_H

#include "Patient.h"
#include <string>

struct StackNode {
    Patient before;
    Patient after;
    std::string operationType;
    StackNode* next;

    StackNode(const Patient& b,
              const Patient& a,
              const std::string& op)
        : before(b), after(a), operationType(op), next(nullptr) {}
};

#endif
