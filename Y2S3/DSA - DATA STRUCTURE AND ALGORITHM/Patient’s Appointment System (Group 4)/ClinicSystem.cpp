#include "ClinicSystem.h"
#include <iostream>
#include <limits>
#include <string>

ClinicSystem::ClinicSystem() {
    std::cout << "Initializing...\n\n";
    loadAllData();  // Load data when system starts
}

void ClinicSystem::run() {
    int choice;
    do {
        std::cout << "\n========== Clinic Patient Queuing System ==========\n";
        std::cout << "1. Patient Registration & Profile Management\n";
        std::cout << "2. Patient Search\n";
        std::cout << "3. Patient Queue Management (Today)\n";
        std::cout << "4. Appointment History\n";
        std::cout << "5. Display All Appointments\n";  // Added option
        std::cout << "0. Exit\n";
        std::cout << "Enter Choice : ";
        std::cin >> choice;

        if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            choice = -1;
        }

        switch(choice) {
            case 1: handlePatientMenu(); break;
            case 2: handleSearchMenu(); break;
            case 3: handleQueueMenu(); break;
            case 4: {
                std::string id;
                std::cin.ignore();
                std::cout << "\n============== Appointment History ================\n\n";
                std::cout << "Enter Patient ID to View History : "; std::getline(std::cin, id);
                std::cout << "\n";
                appointmentManager.displayByPatient(id);
                break;
            }
            case 5: {
                std::cout << "\n================ All Appointments =================\n\n";
                appointmentManager.displayAll();
                break;
            }
            case 0: {
                std::cout << "\nExiting system. Data saved.\n";
                saveAllData();  // Save all data before exiting
                break;
            }
            default: std::cout << "\nInvalid choice. Try again.\n";
        }
    } while(choice != 0);
}

void ClinicSystem::handlePatientMenu() {
    int choice;
    do {
        std::cout << "\n================== Patient Menu ===================\n";
        std::cout << "1. Register New Patient\n";
        std::cout << "2. Edit Patient Profile\n";
        std::cout << "3. Delete Patient Record\n";
        std::cout << "4. Undo Last Operation\n";
        std::cout << "5. Redo Last Operation\n";
        std::cout << "6. Display All Patients\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Enter Choice : ";
        std::cin >> choice;

        if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            choice = -1;
        }

        std::cin.ignore(); // Clear newline for getline

        switch(choice) {
            case 1: {
                std::string name, ic, contact;
                int age;

                std::cout << "\n-------------- Register New Patient ---------------\n\n";
                std::cout << "Enter Name           : "; std::getline(std::cin, name);
                std::cout << "Enter IC Number      : "; std::getline(std::cin, ic);
                std::cout << "Enter Age            : "; std::cin >> age; std::cin.ignore();
                std::cout << "Enter Contact Number : "; std::getline(std::cin, contact);
                std::cout << "\n";

                Patient p = patientManager.addPatientAuto(name, ic, age, contact);
                undoRedoManager.recordAdd(p);

                std::cout << "\nPatient Registered With ID : " << p.getPatientID() << "\n";
                saveAllData();
                break;
            }
            case 2: {
                std::string id;

                std::cout << "\n-------------- Edit Patient Profile ---------------\n\n";
                std::cout << "Enter Patient ID to Edit : "; std::getline(std::cin, id);
                std::cout << "\n";

                PatientNode* node = patientManager.findPatientByID(id);
                if (!node) {
                    std::cout << "Patient not found.\n";
                    break;
                }

                Patient before = node->data;

                std::string name, ic, contact;
                int age;

                std::cout << "New Name      (or Enter to Skip) : ";
                std::getline(std::cin, name);
                if (!name.empty())
                    patientManager.updateName(id, name);

                std::cout << "New IC Number (or Enter to Skip) : ";
                std::getline(std::cin, ic);
                if (!ic.empty())
                    patientManager.updateICNumber(id, ic);

                std::cout << "New Age           (or 0 to Skip) : ";
                std::cin >> age;
                std::cin.ignore();
                if (age > 0)
                    patientManager.updateAge(id, age);

                std::cout << "New Contact   (or Enter to Skip) : ";
                std::getline(std::cin, contact);
                if (!contact.empty())
                    patientManager.updateContactNumber(id, contact);

                Patient after = node->data;

                undoRedoManager.recordEdit(before, after);

                saveAllData();
                break;
            }
            case 3: {
                std::string id;

                std::cout << "\n----------------- Delete Patient ------------------\n\n";
                std::cout << "Enter Patient ID to Delete : "; std::getline(std::cin, id);
                std::cout << "\n";

                PatientNode* node = patientManager.findPatientByID(id);
                if (!node) {
                    std::cout << "Patient not found.\n";
                    break;
                }

                Patient deleted = patientManager.deletePatient(id);
                if(deleted.getPatientID() != "") {
                    undoRedoManager.recordDelete(deleted);
                    saveAllData();  // Save after operation
                }
                break;
            }
            case 4: {
                std::cout << "\n----------------- Undo Operation ------------------\n\n";
                undoRedoManager.undo(patientManager);
                saveAllData();  // Save after undo
                break;
            }
            case 5: {
                std::cout << "\n----------------- Redo Operation ------------------\n\n";
                undoRedoManager.redo(patientManager);
                saveAllData();  // Save after redo
                break;
            }
            case 6: {
                std::cout << "\n------------------ All Patients -------------------\n\n";
                patientManager.displayAllPatients();
                break;
            }
            case 0: {
                break;
            }
            default: {
                std::cout << "\nInvalid choice. Try again.\n";
                break;
            }
        }
    } while(choice != 0);
}

void ClinicSystem::handleSearchMenu() {
    int choice;
    do {
        std::cout << "\n=================== Search Menu ===================\n";
        std::cout << "1. Search by Patient ID\n";
        std::cout << "2. Search by Patient Name\n";
        std::cout << "3. Display All Patients\n";
        std::cout << "4. Display Patients Sorted by Name\n";
        std::cout << "5. Display Patients Sorted by IC\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Enter Choice : ";
        std::cin >> choice;
        
        if(std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            choice = -1;
        }
        std::cin.ignore(); // Clear newline
        
        switch(choice) {
            case 1: {
                std::string id;

                std::cout << "\n------------------ Search by ID -------------------\n\n";
                std::cout << "Enter Patient ID : "; std::getline(std::cin, id);
                std::cout << "\n";

                patientManager.displayPatient(id);
                break;
            }
            case 2: {
                std::string name;

                std::cout << "\n----------------- Search by Name ------------------\n\n";
                std::cout << "Enter Patient Name : "; std::getline(std::cin, name);
                std::cout << "\n";

                PatientNode* node = patientManager.findPatientByName(name);
                if (node) {
                    node->data.display();
                } else {
                    std::cout << "Patient not found.\n";
                }
                break;
            }
            case 3: {
                std::cout << "\n------------------ All Patients -------------------\n\n";
                patientManager.displayAllPatients();
                break;
            }
            case 4: {
                std::cout << "\n----------------- Sorted by Name ------------------\n\n";
                patientManager.displaySortedByName();
                break;
            }
            case 5: {
                std::cout << "\n------------------ Sorted by IC -------------------\n\n";
                patientManager.displaySortedByIC();
                break;
            }
            case 0: {
                break;
            }
            default: {
                std::cout << "\nInvalid choice. Try again.\n";
                break;
            }
        }
    } while(choice != 0);
}

void ClinicSystem::handleQueueMenu() {
    int choice;
    do {
        std::cout << "\n=================== Queue Menu ====================\n\n";
        std::cout << "1. Add Patient to Queue\n";
        std::cout << "2. Call Next Patient\n";
        std::cout << "3. View Queue\n";
        std::cout << "4. Clear Queue\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Enter Choice : ";
        std::cin >> choice; std::cin.ignore();

        switch(choice) {
            case 1: {
                std::string id;

                std::cout << "\n-------------- Add Patient to Queue ---------------\n\n";
                std::cout << "Enter Patient ID : "; std::getline(std::cin, id);
                std::cout << "\n";
                
                // Check if patient exists
                PatientNode* patient = patientManager.findPatientByID(id);
                if (!patient) {
                    std::cout << "Patient not found. Please register first.\n";
                    break;
                }
                
                queueManager.addToQueue(id);
                
                // Record appointment with current date
                std::string currentDate = FileHandler::getCurrentDate();
                appointmentManager.addRecord(id, currentDate);
                saveAllData();  // Save after operation
                break;
            }
            case 2: {
                std::cout << "\n---------------- Call Next Patient ----------------\n\n";
                queueManager.callNext();
                saveAllData();  // Save after operation
                break;
            }
            case 3: {
                std::cout << "\n------------------ Current Queue ------------------\n\n";
                queueManager.viewQueue();
                break;
            }
            case 4: {
                std::cout << "\n------------------- Clear Queue -------------------\n\n";
                queueManager.clearQueue();
                saveAllData();  // Save after operation
                break;
            }
            default: break;
        }
    } while(choice != 0);
}

void ClinicSystem::saveAllData() {
    std::cout << "\n";
    FileHandler::savePatients(patientManager, patientsFile);
    FileHandler::saveAppointments(appointmentManager, appointmentsFile);
    FileHandler::saveQueue(queueManager.getQueue(), queueFile);
    std::cout << "All data saved successfully.\n";
}

void ClinicSystem::loadAllData() {
    FileHandler::loadPatients(patientManager, patientsFile);
    FileHandler::loadAppointments(appointmentManager, appointmentsFile);
    FileHandler::loadQueue(queueManager.getQueue(), queueFile);
    std::cout << "All data loaded successfully.\n";
}