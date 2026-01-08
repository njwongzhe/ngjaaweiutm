#ifndef STACK_H
#define STACK_H

#include "StackNode.h"

class Stack {
private:
    StackNode* top;

public:
    Stack();
    ~Stack();

    bool isEmpty() const;
    void push(const Patient& before,
              const Patient& after,
              const std::string& op);
    StackNode* pop();
    StackNode* peek() const;
    void clear();
};

#endif
