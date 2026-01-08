#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

class Appointment {
private:
    std::string patientID;
    std::string visitDate;

public:
    Appointment();
    Appointment(const std::string& pid, const std::string& date);

    // Getters
    std::string getPatientID() const;
    std::string getVisitDate() const;
};

#endif
