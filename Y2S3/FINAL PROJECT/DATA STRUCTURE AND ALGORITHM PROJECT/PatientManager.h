#ifndef PATIENTMANAGER_H
#define PATIENTMANAGER_H

#include "PatientNode.h"
#include <string>
#include <vector>

class PatientManager {
private:
    PatientNode* head;
    int nextPatientNumber;

    std::string generatePatientID();

public:
    PatientManager();
    ~PatientManager();

    Patient addPatientAuto(const std::string& name,
                           const std::string& ic,
                           int age,
                           const std::string& contact);
    void updateNextPatientNumberFromID(const std::string& id);

    // CRUD operations
    void addPatient(const Patient& p);
    Patient editPatient(const std::string& id, const Patient& updated);
    Patient deletePatient(const std::string& id);

    // Search
    PatientNode* findPatientByID(const std::string& id);
    PatientNode* findPatientByName(const std::string& name);

    // Update individual fields
    bool updateName(const std::string& id, const std::string& name);
    bool updateICNumber(const std::string& id, const std::string& ic);
    bool updateAge(const std::string& id, int age);
    bool updateContactNumber(const std::string& id, const std::string& contact);

    // Display
    void displayPatient(const std::string& id);
    void displaySortedByName() const;
    void displaySortedByIC() const;
    void displayAllPatients();

    // For file I/O
    std::vector<Patient> getAllPatients() const;  // Add this
    PatientNode* getHead() const { return head; } // Add this for traversal
};

#endif