#include "Wire.h"

Wire::Wire(const std::string& name) : name(name), value(false) {
    
}

Wire::~Wire() {
    
}

bool Wire::getValue() const {
    return value;
}

void Wire::setValue(bool val) {
    value = val;
}

const std::string& Wire::getName() const {
    return name;
}