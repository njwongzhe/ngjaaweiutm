Patient’s Appointment System
Developed by Group 4 | SCSE2103-02 Data Structure and Algorithm

============================= User Manual =============================

<System Data>
appointment.txt | Store the appointment history.
patients.txt    | Store the patient profile.
queue.txt       | Store the queue list.
*** The system will show error during execution if the file does not exist.
    Please restart the system in this case, so the file and folder will 
    automatically be generated.

<Source Code>
*** Please ensure the following source codes exist in the folder :
Appointment.cpp        |
Appointment.h          |
AppointmentManager.cpp |
AppointmentManager.h   |
AppointmentNode.h      |
ClinicSystem.cpp       |
ClinicSystem.h         |
FileHandler.cpp        |
FileHandler.h          |
main.cpp               | <Location Main Function>
Patient.cpp            |
Patient.h              |
PatientManager.cpp     |
PatientManager.h       |
PatientNode.h          |
Queue.cpp              |
Queue.h                |
QueueManager.cpp       |
QueueManager.h         |
QueueNode.h            |
Stack.cpp              |
Stack.h                |
StackNode.h            |
UndoRedoManager.cpp    |
UndoRedoManager.h      |

<Compilation>
*** Execute this command in command prompt to compile the source code:
g++ Appointment.cpp AppointmentManager.cpp ClinicSystem.cpp FileHandler.cpp main.cpp Patient.cpp PatientManager.cpp Queue.cpp QueueManager.cpp Stack.cpp UndoRedoManager.cpp -o main

<Execution>
*** Run this file to execute the program:
main.exe