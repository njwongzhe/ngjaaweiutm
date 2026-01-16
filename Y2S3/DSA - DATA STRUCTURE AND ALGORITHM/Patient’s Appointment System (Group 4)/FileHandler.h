#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "PatientManager.h"
#include "AppointmentManager.h"
#include "Queue.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

class FileHandler {
public:
    static void savePatients(const PatientManager& pm, const std::string& fileName);
    static void loadPatients(PatientManager& pm, const std::string& fileName);
    
    static void saveAppointments(const AppointmentManager& am, const std::string& fileName);
    static void loadAppointments(AppointmentManager& am, const std::string& fileName);
    
    static void saveQueue(const Queue& queue, const std::string& fileName);
    static void loadQueue(Queue& queue, const std::string& fileName); 
    
    static std::string getCurrentDate();
};

#endif