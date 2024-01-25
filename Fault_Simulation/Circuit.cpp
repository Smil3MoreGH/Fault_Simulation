#include "Circuit.h"
#include <iostream>
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
    const size_t numInputs = inputs.size();
    const size_t numOutputs = outputs.size();
    const size_t numCombinations = 1 << numInputs;  // 2^n combinations

    for (size_t i = 0; i < numCombinations; ++i) {
        // Set inputs based on the current combination
        std::cout << "inputs: ";
        for (size_t j = 0; j < numInputs; ++j) {
            bool inputValue = (i >> j) & 1;  // Get the j-th bit of i
            inputs[j]->setValue(inputValue);
            std::cout << inputValue << (j < numInputs - 1 ? "," : " ");
        }

        // Compute the output of each gate
        for (auto& gate : gates) {
            gate->computeOutput();
        }

        // Print the outputs
        std::cout << "gives outputs: ";
        for (size_t k = 0; k < numOutputs; ++k) {
            std::cout << outputs[k]->getValue() << (k < numOutputs - 1 ? "," : "\n");
        }
    }
}

void Circuit::runFaultedSimulation() {
    // TODO: Build a Stuck-at 0 and 1 fault list for all possible wires they can be either stuckat1 or stuckat0, then inject a fault one by one, then calculate these and save all results. Finally, Compare results of runGoodSimulation and runFaultedSimulation.
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