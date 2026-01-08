#ifndef UNDOREDOMANAGER_H
#define UNDOREDOMANAGER_H

#include "Stack.h"
#include "PatientManager.h"

class UndoRedoManager {
private:
    Stack undoStack;
    Stack redoStack;

public:
    UndoRedoManager();

    void recordAdd(const Patient& p);
    void recordEdit(const Patient& before, const Patient& after);
    void recordDelete(const Patient& p);

    void undo(PatientManager& pm);
    void redo(PatientManager& pm);

    void clearRedo();
};

#endif
