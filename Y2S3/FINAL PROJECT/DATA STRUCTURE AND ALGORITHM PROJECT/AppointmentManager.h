#ifndef APPOINTMENTMANAGER_H
#define APPOINTMENTMANAGER_H

#include "AppointmentNode.h"
#include <string>
#include <vector>

class AppointmentManager {
private:
    AppointmentNode* head;

public:
    AppointmentManager();
    ~AppointmentManager();

    void addRecord(const std::string& patientID, const std::string& date, bool silent = false);
    void displayByPatient(const std::string& patientID) const;
    void displayAll() const;
    
    // Add for file I/O
    std::vector<Appointment> getAllAppointments() const;
};

#endif