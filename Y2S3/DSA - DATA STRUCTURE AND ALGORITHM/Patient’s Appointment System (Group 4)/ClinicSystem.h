#ifndef CLINICSYSTEM_H
#define CLINICSYSTEM_H

#include "PatientManager.h"
#include "QueueManager.h"
#include "AppointmentManager.h"
#include "UndoRedoManager.h"
#include "FileHandler.h"
#include <string>

class ClinicSystem {
private:
    PatientManager patientManager;
    QueueManager queueManager;
    AppointmentManager appointmentManager;
    UndoRedoManager undoRedoManager;
    
    std::string patientsFile = "patients.txt";
    std::string appointmentsFile = "appointments.txt";
    std::string queueFile = "queue.txt";

    // Menu handlers
    void handlePatientMenu();
    void handleQueueMenu();
    void handleSearchMenu();
    
    // File operations
    void saveAllData();
    void loadAllData();
    
public:
    ClinicSystem();
    void run();
};

#endif