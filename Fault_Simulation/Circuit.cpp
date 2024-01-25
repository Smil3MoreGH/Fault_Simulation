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
void Circuit::runAndPrintGoodSimulation() {
    auto results = runGoodSimulation();
    printGoodSimulationResults(results);
}



std::vector<std::vector<bool>> Circuit::runGoodSimulation() {
    const size_t numInputs = inputs.size();
    const size_t numOutputs = outputs.size();
    const size_t numCombinations = 1 << numInputs;  

    std::vector<std::vector<bool>> simulationResults;

    for (size_t i = 0; i < numCombinations; ++i) {
        for (size_t j = 0; j < numInputs; ++j) {
            bool inputValue = (i >> j) & 1;
            inputs[j]->setValue(inputValue);
        }

        for (auto& gate : gates) {
            gate->computeOutput();
        }

        // Collect output values for this combination
        std::vector<bool> currentOutput;
        for (size_t k = 0; k < numOutputs; ++k) {
            currentOutput.push_back(outputs[k]->getValue());
        }

        // Store the collected output values
        simulationResults.push_back(currentOutput);
    }

    return simulationResults;
}

void Circuit::printGoodSimulationResults(const std::vector<std::vector<bool>>& results) {
    const size_t numInputs = inputs.size();
    const size_t numCombinations = 1 << numInputs;

    for (size_t i = 0; i < numCombinations; ++i) {
        std::cout << "inputs: ";
        for (size_t j = 0; j < numInputs; ++j) {
            std::cout << ((i >> j) & 1) << (j < numInputs - 1 ? "," : " ");
        }

        std::cout << "gives outputs: ";
        for (size_t k = 0; k < results[i].size(); ++k) {
            std::cout << results[i][k] << (k < results[i].size() - 1 ? "," : "\n");
        }
    }
}

/*
void Circuit::runFaultedSimulation() {
    auto goodResults = runGoodSimulation();

    for (Wire* wire : getAllWires()) {
        // For each wire, create two faults: stuck-at-0 and stuck-at-1
        for (int faultType = 0; faultType <= 1; ++faultType) {
            // Inject fault
            injectFault(wire, faultType);

            // Run simulation with the fault
            std::vector<std::vector<bool>> faultedResults = runSimulationWithFault();

            // Compare faulted results with good results
            compareResults(goodResults, faultedResults);

            // Remove fault before next iteration
            removeFault(wire, faultType);
        }
    }
}
*/

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