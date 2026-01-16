#include "QueueManager.h"
#include <iostream>

QueueManager::QueueManager() {}

void QueueManager::addToQueue(const std::string& patientID) {
    patientQueue.enqueue(patientID);
    std::cout << "Patient " << patientID << " added to queue.\n";
}

void QueueManager::callNext() {
    if (patientQueue.isEmpty()) {
        std::cout << "Queue is empty.\n";
        return;
    }
    std::string nextID = patientQueue.dequeue();
    std::cout << "Next patient called: " << nextID << "\n";
    
    // In a real system, you might want to record the consultation completion
    // with a specific appointment type or status
}

void QueueManager::viewQueue() const {
    patientQueue.displayQueue();
}

void QueueManager::clearQueue() {
    patientQueue.clear();
    std::cout << "Queue cleared.\n";
}