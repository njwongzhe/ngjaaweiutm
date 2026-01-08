#include "Stack.h"
#include <iostream>

Stack::Stack() : top(nullptr) {}

Stack::~Stack() { clear(); }

bool Stack::isEmpty() const { return top == nullptr; }

void Stack::push(const Patient& before,
                 const Patient& after,
                 const std::string& op) {
    StackNode* node = new StackNode(before, after, op);
    node->next = top;
    top = node;
}

StackNode* Stack::pop() {
    if (isEmpty()) return nullptr;
    StackNode* currNode = top;
    top = top->next;
    currNode->next = nullptr;
    return currNode;
}

StackNode* Stack::peek() const { return top; }

void Stack::clear() {
    while (!isEmpty()) {
        StackNode* node = pop();
        delete node;
    }
}
