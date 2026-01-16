#include "PatientManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// constructor
PatientManager::PatientManager() : head(nullptr), nextPatientNumber(1) {}

// destructor
PatientManager::~PatientManager() {
    while (head) {
        PatientNode* delNode = head;
        head = head->next;
        delete delNode;
    }
}

std::string PatientManager::generatePatientID() {
    std::ostringstream oss;
    oss << "P" << std::setw(4) << std::setfill('0') << nextPatientNumber++;
    return oss.str();
}

Patient PatientManager::addPatientAuto(const std::string& name,
                                       const std::string& ic,
                                       int age,
                                       const std::string& contact) {
    std::string id = generatePatientID();
    Patient p(id, name, ic, age, contact);
    addPatient(p);
    return p;
}

void PatientManager::updateNextPatientNumberFromID(const std::string& id) {
    // Expected format: P0001
    if (id.length() < 2) return;

    try {
        int number = std::stoi(id.substr(1));
        if (number >= nextPatientNumber) {
            nextPatientNumber = number + 1;
        }
    } catch (...) {
        // Ignore malformed IDs safely
    }
}

// CRUD operations
void PatientManager::addPatient(const Patient& p) {
    PatientNode* newNode = new PatientNode(p);
    if (!head) {
        head = newNode;
    } else {
        PatientNode* currNode = head;
        while (currNode->next) currNode = currNode->next;
        currNode->next = newNode;
    }
    std::cout << "Patient added successfully.\n";
}

Patient PatientManager::editPatient(const std::string& id, const Patient& updated) {
    PatientNode* node = findPatientByID(id);
    if (!node) {
        std::cout << "Patient not found.\n";
        return Patient();
    }
    Patient old = node->data;
    node->data = updated;
    std::cout << "Patient updated successfully.\n";
    return old;
}

Patient PatientManager::deletePatient(const std::string& id) {
    if (!head) return Patient();
    PatientNode* currNode = head;
    PatientNode* prev = nullptr;
    while (currNode && currNode->data.getPatientID() != id) {
        prev = currNode;
        currNode = currNode->next;
    }
    if (!currNode) {
        std::cout << "Patient not found.\n";
        return Patient();
    }
    Patient deleted = currNode->data;
    if (!prev) head = currNode->next;
    else prev->next = currNode->next;
    delete currNode;
    std::cout << "Patient deleted successfully.\n";
    return deleted;
}

PatientNode* PatientManager::findPatientByID(const std::string& id) {
    PatientNode* currNode = head;
    while (currNode) {
        if (currNode->data.getPatientID() == id) return currNode;
        currNode = currNode->next;
    }
    return nullptr;
}

PatientNode* PatientManager::findPatientByName(const std::string& name) {
    PatientNode* currNode = head;
    while (currNode) {
        if (currNode->data.getName() == name) return currNode;
        currNode = currNode->next;
    }
    return nullptr;
}

bool PatientManager::updateName(const std::string& id, const std::string& name) {
    PatientNode* node = findPatientByID(id);
    if (!node) return false;
    node->data.setName(name);
    return true;
}

bool PatientManager::updateICNumber(const std::string& id, const std::string& ic) {
    PatientNode* node = findPatientByID(id);
    if (!node) return false;
    node->data.setICNumber(ic);
    return true;
}

bool PatientManager::updateAge(const std::string& id, int age) {
    PatientNode* node = findPatientByID(id);
    if (!node) return false;
    node->data.setAge(age);
    return true;
}

bool PatientManager::updateContactNumber(const std::string& id, const std::string& contact) {
    PatientNode* node = findPatientByID(id);
    if (!node) return false;
    node->data.setContactNumber(contact);
    return true;
}

void PatientManager::displayPatient(const std::string& id) {
    PatientNode* node = findPatientByID(id);
    if (!node) {
        std::cout << "Patient not found.\n";
        return;
    }
    node->data.display();
}

void PatientManager::displaySortedByName() const {
    std::vector<Patient> patients = getAllPatients();

    // Simple bubble sort by name
    for (size_t i = 0; i < patients.size(); ++i) {
        for (size_t j = 0; j + 1 < patients.size() - i; ++j) {
            if (patients[j].getName() > patients[j + 1].getName()) {
                std::swap(patients[j], patients[j + 1]);
            }
        }
    }

    if (patients.empty()) {
        std::cout << "No patients registered.\n";
        return;
    }

    std::cout << "---------------------------------------------------\n";
    for (const Patient& p : patients) {
        p.display();
        std::cout << "---------------------------------------------------\n";
    }
}

void PatientManager::displaySortedByIC() const {
    std::vector<Patient> patients = getAllPatients();

    // Simple bubble sort by IC number
    for (size_t i = 0; i < patients.size(); ++i) {
        for (size_t j = 0; j + 1 < patients.size() - i; ++j) {
            if (patients[j].getICNumber() > patients[j + 1].getICNumber()) {
                std::swap(patients[j], patients[j + 1]);
            }
        }
    }

    if (patients.empty()) {
        std::cout << "No patients registered.\n";
        return;
    }

    std::cout << "---------------------------------------------------\n";
    for (const Patient& p : patients) {
        p.display();
        std::cout << "---------------------------------------------------\n";
    }
}

void PatientManager::displayAllPatients() {
    if (!head) {
        std::cout << "No patients registered.\n";
        return;
    }
    PatientNode* currNode = head;
    std::cout << "---------------------------------------------------\n";
    while (currNode) {
        currNode->data.display();
        std::cout << "---------------------------------------------------\n";
        currNode = currNode->next;
    }
}

std::vector<Patient> PatientManager::getAllPatients() const {
    std::vector<Patient> patients;
    PatientNode* temp = head;
    while (temp) {
        patients.push_back(temp->data);
        temp = temp->next;
    }
    return patients;
}