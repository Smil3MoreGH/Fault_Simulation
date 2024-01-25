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
        
        // Reset all wire values to 0 for the next combination
        //resetAllWires();
    }

    return simulationResults;
}

void Circuit::resetAllWires()
{
    // Reset all wire values to 0 before starting fault simulation
    for (Wire* wire : getAllWires()) {
        wire->setValue(false); // Set to 0
    }

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

void Circuit::runFaultedSimulation() {
    auto goodResults = runGoodSimulation();

    for (Wire* wire : getAllWiresButOutputs()) {
        bool faultDetected[2] = {false, false};  // Track detection for stuck-at-0 and stuck-at-1

        for (int faultType = 0; faultType <= 1; ++faultType) {
            injectFault(wire, faultType);
            std::vector<std::vector<bool>> faultedResults = runGoodSimulation();
            faultDetected[faultType] = compareResults(goodResults, faultedResults, wire, faultType);
            removeFault(wire);
        }

        // Check if faults were undetected for this wire
        for (int faultType = 0; faultType <= 1; ++faultType) {
            if (!faultDetected[faultType]) {
                std::cout << "Fault was undetected for " << wire->getName() << " stuck-at-" << faultType << "\n";
            }
        }
    }
}



bool Circuit::compareResults(const std::vector<std::vector<bool>>& goodResults, 
                             const std::vector<std::vector<bool>>& faultedResults,
                             Wire* wire, int faultType) {
    const size_t numInputs = inputs.size();
    bool faultDetected = false;

    for (size_t i = 0; i < goodResults.size(); ++i) {
        if (goodResults[i] != faultedResults[i]) {
            faultDetected = true;
            // Print the input combination where the fault is detected
            std::cout << "Fault detected on wire " << wire->getName() 
                      << " stuck-at-" << faultType 
                      << " with inputs: ";
            for (size_t j = 0; j < numInputs; ++j) {
                bool inputValue = (i >> j) & 1;
                std::cout << inputs[j]->getName() << ": " << inputValue 
                          << (j < numInputs - 1 ? " | " : " |");
            }
            std::cout << "\n";
            break;  // Optionally break here if only reporting the first detection
        }
    }

    if (!faultDetected) {
        std::cout << "No fault detected on wire " << wire->getName() << " stuck-at-" << faultType << "\n";
    }

    return faultDetected;
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

std::vector<Wire*> Circuit::getAllWires() const {
    std::vector<Wire*> allWires;
    allWires.insert(allWires.end(), inputs.begin(), inputs.end());
    allWires.insert(allWires.end(), outputs.begin(), outputs.end());
    allWires.insert(allWires.end(), internalWires.begin(), internalWires.end());
    return allWires;
}
std::vector<Wire*> Circuit::getAllWiresButOutputs() const {
    std::vector<Wire*> allWires;
    allWires.insert(allWires.end(), inputs.begin(), inputs.end());
    allWires.insert(allWires.end(), internalWires.begin(), internalWires.end());
    return allWires;
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

void Circuit::injectFault(Wire* wire, bool faultType) {
    if (wire) {
        wire->setFault(true, faultType);
    }
}

void Circuit::removeFault(Wire* wire) {
    if (wire) {
        wire->clearFault();
    }
}