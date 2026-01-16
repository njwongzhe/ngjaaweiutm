#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include "Queue.h"
#include <string>

class QueueManager {
private:
    Queue patientQueue;

public:
    QueueManager();
    
    void addToQueue(const std::string& patientID);
    void callNext();
    void viewQueue() const;
    void clearQueue();
    
    // file I/O access
    Queue& getQueue() { return patientQueue; } 
    const Queue& getQueue() const { return patientQueue; } 
};

#endif