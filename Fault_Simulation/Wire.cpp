#include "Wire.h"

Wire::Wire(const std::string& name) : name(name), value(false) {
    
}

Wire::~Wire() {
    
}

void Wire::setFault(bool isFaulted, bool faultValue) {
    this->isFaulted = isFaulted;
    this->faultValue = faultValue;
}

void Wire::clearFault() {
    isFaulted = false;
}

bool Wire::getValue() const {
    return isFaulted ? faultValue : value;
}

void Wire::setValue(bool val) {
    value = val;
}

const std::string& Wire::getName() const {
    return name;
}