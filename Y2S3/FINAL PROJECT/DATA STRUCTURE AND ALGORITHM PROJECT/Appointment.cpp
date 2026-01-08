#include "Appointment.h"
#include <iostream>

Appointment::Appointment() : patientID(""), visitDate("") {}

Appointment::Appointment(const std::string& pid, const std::string& date)
    : patientID(pid), visitDate(date) {}

std::string Appointment::getPatientID() const { return patientID; }

std::string Appointment::getVisitDate() const { return visitDate; }