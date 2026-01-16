#include "FileHandler.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

void FileHandler::savePatients(const PatientManager& pm, const string& fileName) {
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << fileName << " for writing.\n";
        return;
    }
    
    // Get all patients from PatientManager
    vector<Patient> patients = pm.getAllPatients();
    
    for (const Patient& p : patients) {
        file << p.getPatientID() << "|"
             << p.getName() << "|"
             << p.getICNumber() << "|"
             << p.getAge() << "|"
             << p.getContactNumber() << "\n";
    }
    
    file.close();
    cout << patients.size() << " patients saved to " << fileName << endl;
}

void FileHandler::loadPatients(PatientManager& pm, const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "No existing patient data found. Starting fresh.\n";
        return;
    }
    
    string line;
    int count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        istringstream iss(line);
        string id, name, ic, contact, ageStr;
        
        if (getline(iss, id, '|') &&
            getline(iss, name, '|') &&
            getline(iss, ic, '|') &&
            getline(iss, ageStr, '|') &&
            getline(iss, contact)) {
            
            try {
                int age = stoi(ageStr);
                Patient patient(id, name, ic, age, contact);
                pm.addPatient(patient);
                pm.updateNextPatientNumberFromID(id);
                count++;
            } catch (const exception& e) {
                cerr << "Error parsing patient data: " << line << endl;
            }
        }
    }
    file.close();
    cout << count << " patients loaded from " << fileName << endl;
}

void FileHandler::saveAppointments(const AppointmentManager& am, const string& fileName) {
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << fileName << " for writing.\n";
        return;
    }
    
    // Get all appointments
    vector<Appointment> appointments = am.getAllAppointments();

    for (const Appointment& a : appointments) {
        file << a.getPatientID() << "|" << a.getVisitDate() << "\n";
    }
    
    file.close();
    cout << appointments.size() << " appointments saved to " << fileName << endl;
}

void FileHandler::loadAppointments(AppointmentManager& am, const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "No existing appointment data found.\n";
        return;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty() || line == "Appointment Data Saved") continue;
        
        istringstream iss(line);
        string patientID, date;
        
        if (getline(iss, patientID, '|') &&
            getline(iss, date)) {
            am.addRecord(patientID, date, true);
        }
    }
    file.close();
    cout << "Appointments loaded from " << fileName << endl;
}

void FileHandler::saveQueue(const Queue& queue, const string& fileName) {
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << fileName << " for writing.\n";
        return;
    }
    
    // Get all patient IDs from queue
    vector<string> patientIDs = queue.getAllPatientIDs();
    
    for (const string& id : patientIDs) {
        file << id << "\n";
    }
    
    file.close();
    cout << patientIDs.size() << " queue entries saved to " << fileName << endl;
}

void FileHandler::loadQueue(Queue& queue, const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "No existing queue data found.\n";
        return;
    }
    
    string line;
    int count = 0;
    while (getline(file, line)) {
        if (!line.empty()) {
            queue.enqueue(line);
            count++;
        }
    }
    file.close();
    cout << count << " queue entries loaded from " << fileName << endl;
}

string FileHandler::getCurrentDate() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}