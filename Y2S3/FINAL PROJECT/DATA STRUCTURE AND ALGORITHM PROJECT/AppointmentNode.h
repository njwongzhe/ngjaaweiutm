#ifndef APPOINTMENTNODE_H
#define APPOINTMENTNODE_H

#include <string>
#include "Appointment.h"

struct AppointmentNode {
    Appointment data;
    AppointmentNode* next;

    AppointmentNode(const Appointment& a) : data(a), next(nullptr) {}
};

#endif
