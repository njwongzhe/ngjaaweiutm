#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <iostream>

class Patient {
private:
    std::string patientID;
    std::string name;
    std::string icNumber;
    int age;
    std::string contactNumber;

public:
    // Constructors
    Patient();
    Patient(const std::string& id, const std::string& name,
            const std::string& ic, int age, const std::string& contact);

    // Accessors (Getters)
    std::string getPatientID() const;
    std::string getName() const;
    std::string getICNumber() const;
    int getAge() const;
    std::string getContactNumber() const;

    // Mutators (Setters)
    void setName(const std::string& name);
    void setICNumber(const std::string& ic);
    void setAge(int age);
    void setContactNumber(const std::string& contact);

    // Display
    void display() const;
};

#endif
