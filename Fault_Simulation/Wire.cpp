#include "Wire.h"

// Constructor for the Wire class that initializes a wire with a given name.
Wire::Wire(const std::string& name) 
    : name(name), // Set the name of the wire, which is useful for identification purposes.
      value(false) // Initialize the wire's value to false (0) by default.
{
    
}

Wire::~Wire() {
    
}

// Sets a fault condition on the wire, simulating a stuck-at fault.
void Wire::setFault(bool isFaulted, bool faultValue) {
    this->isFaulted = isFaulted; // Indicate whether the wire is faulted.
    this->faultValue = faultValue; // The value to force the wire to if it is faulted (stuck-at-0 or stuck-at-1).
}

// Clears any fault condition previously set on the wire.
void Wire::clearFault() {
    isFaulted = false; // Reset the faulted state to false, indicating no fault.
}

// Retrieves the current value of the wire. If the wire is faulted, it returns the fault value; otherwise, it returns the wire's actual value.
bool Wire::getValue() const {
    return isFaulted ? faultValue : value; // Conditional operator to return the appropriate value based on fault state.
}

// Sets the binary value of the wire.
void Wire::setValue(bool val) {
    value = val; // Assign the specified value to the wire.
}

// Gets the name of the wire.
const std::string& Wire::getName() const {
    return name; // Return the name assigned to the wire.
}