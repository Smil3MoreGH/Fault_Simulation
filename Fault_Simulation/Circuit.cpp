#include "Circuit.h"
#include <fstream>
#include <iostream>
#include <stack>
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

    // Build the graph for the circuit
    buildGraph();

    // Perform topological sort to get gates in the order they should be computed
    std::stack<Gate*> sortedGates = topologicalSort();

    for (size_t i = 0; i < numCombinations; ++i) {
        for (size_t j = 0; j < numInputs; ++j) {
            bool inputValue = (i >> j) & 1;
            inputs[j]->setValue(inputValue);
        }
        // Compute outputs using topologically sorted gates
        std::stack<Gate*> tempSortedGates = sortedGates; // Copy sorted gates for this iteration
        while (!tempSortedGates.empty()) {
            Gate* currentGate = tempSortedGates.top();
            currentGate->computeOutput();
            tempSortedGates.pop();
        }
        // Collect output values for this combination
        std::vector<bool> currentOutput;
        for (size_t k = 0; k < numOutputs; ++k) {
            currentOutput.push_back(outputs[k]->getValue());
        }
        // Store the collected output values
        simulationResults.push_back(currentOutput);
        // Optionally, reset all wire values to 0 for the next combination
        //resetAllWires();
    }
    return simulationResults;
}


void Circuit::buildGraph() {
    // Reset or initialize the adjacency list
    adjList.clear();
    // Iterate over all gates to build the graph
    for (auto& gate : gates) {
        // If the gate has an output wire, find which gates use this wire as an input
        if (gate->getOutput() != nullptr) {
            for (auto& dependentGate : gates) {
                if (dependentGate->getInput1() == gate->getOutput() || 
                    dependentGate->getInput2() == gate->getOutput()) {
                    adjList[gate].push_back(dependentGate);
                    }
            }
        }
    }
}
void Circuit::topologicalSortUtil(Gate* gate, std::map<Gate*, bool>& visited, std::stack<Gate*>& stack) {
    visited[gate] = true;
    // Recursively call this function for all gates dependent on this gate
    for (auto& dependentGate : adjList[gate]) {
        if (!visited[dependentGate]) {
            topologicalSortUtil(dependentGate, visited, stack);
        }
    }
    // Push current gate to stack which stores the result
    stack.push(gate);
}

std::stack<Gate*> Circuit::topologicalSort() {
    std::stack<Gate*> stack;
    std::map<Gate*, bool> visited;
    // Mark all the gates as not visited
    for (auto& gate : gates) {
        visited[gate] = false;
    }
    // Call the recursive helper function to store the Topological Sort
    for (auto& gate : gates) {
        if (!visited[gate]) {
            topologicalSortUtil(gate, visited, stack);
        }
    }
    return stack;
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

    std::ofstream outFile("simulation_results.txt"); // Open file for writing

    for (size_t i = 0; i < numCombinations; ++i) {
        outFile << "inputs: ";
        for (size_t j = 0; j < numInputs; ++j) {
            outFile << ((i >> j) & 1) << (j < numInputs - 1 ? "," : " ");
        }

        outFile << "gives outputs: ";
        for (size_t k = 0; k < results[i].size(); ++k) {
            outFile << results[i][k] << (k < results[i].size() - 1 ? "," : "\n");
        }
    }

    outFile.close(); // Close the file
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

    std::ofstream outFile("stuck-at-results.txt", std::ios::app); // Open file in append mode

    for (size_t i = 0; i < goodResults.size(); ++i) {
        if (goodResults[i] != faultedResults[i]) {
            faultDetected = true;
            outFile << "\\" << wire->getName() 
                    << " stuck-at-" << faultType 
                    << " with inputs: ";
            for (size_t j = 0; j < numInputs; ++j) {
                bool inputValue = (i >> j) & 1;
                outFile << inputValue << (j < numInputs - 1 ? ", " : "");
            }
            outFile << "\n";
            break;  // Optionally break here if only reporting the first detection
        }
    }

    if (!faultDetected) {
        outFile << "No fault detected on wire \\" << wire->getName() << " stuck-at-" << faultType << "\n";
    }

    outFile.close();  // Don't forget to close the file
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