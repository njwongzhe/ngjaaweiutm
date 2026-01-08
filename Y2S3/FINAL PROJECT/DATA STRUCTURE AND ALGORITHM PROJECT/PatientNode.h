#ifndef PATIENTNODE_H
#define PATIENTNODE_H

#include "Patient.h"

struct PatientNode {
    Patient data;
    PatientNode* next;

    PatientNode(const Patient& p) : data(p), next(nullptr) {}
};

#endif
