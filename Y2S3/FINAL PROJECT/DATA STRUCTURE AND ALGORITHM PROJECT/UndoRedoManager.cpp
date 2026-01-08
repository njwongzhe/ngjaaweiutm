#include "UndoRedoManager.h"
#include <iostream>

UndoRedoManager::UndoRedoManager() {}

void UndoRedoManager::recordAdd(const Patient& p) {
    undoStack.push(p, p, "ADD");
    clearRedo();
}

void UndoRedoManager::recordDelete(const Patient& p) {
    undoStack.push(p, p, "DELETE");
    clearRedo();
}

void UndoRedoManager::recordEdit(const Patient& before, const Patient& after) {
    undoStack.push(before, after, "EDIT");
    clearRedo();
}

void UndoRedoManager::undo(PatientManager& pm) {
    StackNode* node = undoStack.pop();
    if (!node) {
        std::cout << "Nothing to undo.\n";
        return;
    }

    if (node->operationType == "ADD") pm.deletePatient(node->before.getPatientID());
    else if (node->operationType == "DELETE") pm.addPatient(node->before);
    else if (node->operationType == "EDIT") pm.editPatient(node->before.getPatientID(), node->before);

    redoStack.push(node->before, node->after, node->operationType);
    delete node;
}

void UndoRedoManager::redo(PatientManager& pm) {
    StackNode* node = redoStack.pop();
    if (!node) {
        std::cout << "Nothing to redo.\n";
        return;
    }

    if (node->operationType == "ADD") {
        pm.addPatient(node->after);
    }
    else if (node->operationType == "DELETE") {
        pm.deletePatient(node->after.getPatientID());
    }
    else if (node->operationType == "EDIT") {
        pm.editPatient(node->after.getPatientID(), node->after);
    }

    undoStack.push(node->before, node->after, node->operationType);
    delete node;
}

void UndoRedoManager::clearRedo() {
    redoStack.clear();
}
