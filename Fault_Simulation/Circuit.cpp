#include "Circuit.h"

#include "Parser.h"

Circuit::Circuit() {
    // Constructor implementation (if needed)
}

Circuit::~Circuit() {
    // Destructor implementation: Clean up allocated memory
    for (auto wire : inputs) delete wire;
    for (auto wire : outputs) delete wire;
    for (auto wire : internalWires) delete wire;
    for (auto gate : gates) delete gate;
}

void Circuit::loadFromFile(const std::string& filepath) {
    Parser parser;
    parser.parse(filepath, *this);
}

void Circuit::runGoodSimulation() {
    // TODO: 
}

void Circuit::runFaultedSimulation() {
    // TODO: 
}


Wire* Circuit::findWireByName(const std::string& name) {
    // Iterate through inputs to find the wire
    for (auto& wire : inputs) {
        if (wire->getName() == name) return wire;
    }
    // Iterate through outputs to find the wire
    for (auto& wire : outputs) {
        if (wire->getName() == name) return wire;
    }
    // Iterate through internal wires to find the wire
    for (auto& wire : internalWires) {
        if (wire->getName() == name) return wire;
    }
    return nullptr; // Wire not found
}
void Circuit::addInput(Wire* wire) {
    inputs.push_back(wire);
}

void Circuit::addOutput(Wire* wire) {
    outputs.push_back(wire);
}

void Circuit::addInternalWire(Wire* wire) {
    internalWires.push_back(wire);
}

void Circuit::addGate(Gate* gate) {
    gates.push_back(gate);
}