#include "AppointmentManager.h"
#include <iostream>

// constructor (Create Linked List)
AppointmentManager::AppointmentManager() : head(nullptr) {}

// destructor (Destroy Linked List)
AppointmentManager::~AppointmentManager() {
    while(head) {
        AppointmentNode* del = head;
        head = head->next;
        delete del;
    }
}

// Add appointmentNode to the end of the linked list
void AppointmentManager::addRecord(const std::string& patientID, const std::string& date, bool silent) {
    AppointmentNode* newNode = new AppointmentNode(Appointment(patientID, date));

    if (!head) {
        head = newNode;
    } else {
        AppointmentNode* curr = head;
        while (curr->next)
            curr = curr->next;
        curr->next = newNode;
    }

    if (!silent)
        std::cout << "Appointment added.\n";
}

// Display appointments for a specific patient
void AppointmentManager::displayByPatient(const std::string& patientID) const {
    AppointmentNode* curr = head;
    bool found = false;

    while (curr) {
        if (curr->data.getPatientID() == patientID) {
            std::cout << "PatientID: "
                      << curr->data.getPatientID()
                      << " | Date: "
                      << curr->data.getVisitDate()
                      << "\n";
            found = true;
        }
        curr = curr->next;
    }

    if (!found)
        std::cout << "No appointments found for this patient.\n";
}

// Display all appointments
void AppointmentManager::displayAll() const {
    if (!head) {
        std::cout << "No appointments recorded.\n";
        return;
    }

    AppointmentNode* curr = head;
    while (curr) {
        std::cout << "PatientID: "
                  << curr->data.getPatientID()
                  << " | Date: "
                  << curr->data.getVisitDate()
                  << "\n";
        curr = curr->next;
    }
}

// Retrieve all appointments for file I/O
std::vector<Appointment> AppointmentManager::getAllAppointments() const {
    std::vector<Appointment> appointments;
    AppointmentNode* curr = head;

    while (curr) {
        appointments.push_back(curr->data);
        curr = curr->next;
    }

    return appointments;
}
