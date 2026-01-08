#include "Patient.h"
#include <iostream>

// Constructors
Patient::Patient() : patientID(""), name(""), icNumber(""), age(0), contactNumber("") {}

// Parameterized Constructor
Patient::Patient(const std::string& id, const std::string& name,
                 const std::string& ic, int age, const std::string& contact)
    : patientID(id), name(name), icNumber(ic), age(age), contactNumber(contact) {}

std::string Patient::getPatientID() const { return patientID; }
std::string Patient::getName() const { return name; }
std::string Patient::getICNumber() const { return icNumber; }
int Patient::getAge() const { return age; }
std::string Patient::getContactNumber() const { return contactNumber; }

void Patient::setName(const std::string& n) { name = n; }
void Patient::setICNumber(const std::string& ic) { icNumber = ic; }
void Patient::setAge(int a) { age = a; }
void Patient::setContactNumber(const std::string& c) { contactNumber = c; }

void Patient::display() const {
    std::cout << "Patient ID : " << patientID << "\n"
              << "Name       : " << name << "\n"
              << "IC Number  : " << icNumber << "\n"
              << "Age        : " << age << "\n"
              << "Contact    : " << contactNumber << "\n";
}
