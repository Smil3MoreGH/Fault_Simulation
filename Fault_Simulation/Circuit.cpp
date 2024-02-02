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

// Runs a comprehensive simulation of the circuit for all possible input combinations and collects the results.
std::vector<std::vector<bool>> Circuit::runGoodSimulation() {
    // Determine the total number of input wires and output wires in the circuit.
    const size_t numInputs = inputs.size();
    const size_t numOutputs = outputs.size();
    // Calculate the total number of input combinations possible based on the number of input wires.
    // This uses bitwise left shift to compute 2^numInputs.
    const size_t numCombinations = 1 << numInputs;
    // Initialize a container to store the simulation results for each input combination.
    std::vector<std::vector<bool>> simulationResults;
    // Prepare the circuit for simulation by building a graph representation of all gates and their connections.
    buildGraph();

    // Order the gates in a sequence that respects their dependencies using topological sorting.
    // This ensures that each gate is computed only after all its inputs have been resolved.
    std::stack<Gate*> sortedGates = topologicalSort();

    // Iterate over every possible combination of input values.
    for (size_t i = 0; i < numCombinations; ++i) {
        // Set the input values for the current combination.
        // Each bit of 'i' represents the value for one input wire.
        for (size_t j = 0; j < numInputs; ++j) {
            bool inputValue = (i >> j) & 1; // Extract the j-th bit of 'i' to use as the input value.
            inputs[j]->setValue(inputValue); // Set the value of the j-th input wire.
        }
        // Create a temporary copy of the sorted gates for this simulation iteration.
        // This is necessary because we will be modifying the stack by popping elements.
        std::stack<Gate*> tempSortedGates = sortedGates;
        // Compute the outputs by iterating through the gates in topologically sorted order.
        while (!tempSortedGates.empty()) {
            Gate* currentGate = tempSortedGates.top(); // Get the next gate to compute.
            currentGate->computeOutput(); // Compute the output of this gate based on its inputs.
            tempSortedGates.pop(); // Remove this gate from the temporary stack.
        }
        // Collect the output values for the current input combination.
        std::vector<bool> currentOutput;
        for (size_t k = 0; k < numOutputs; ++k) {
            currentOutput.push_back(outputs[k]->getValue()); // Get the value of the k-th output wire.
        }
        // Add the collected output values to the simulation results.
        simulationResults.push_back(currentOutput);
    }
    // Return the collected simulation results.
    return simulationResults;
}

// Constructs a directed graph representing the circuit's gates and their dependencies.
void Circuit::buildGraph() {
    // First, clear any existing graph data to prepare for building a new graph.
    adjList.clear();
    // Iterate through each gate in the circuit to establish its relationships within the graph.
    // This loop examines every gate to determine how gates are interconnected through their input and output wires.
    for (auto& gate : gates) {
        // Check if the current gate produces an output.
        // A gate must have an output wire to influence other gates within the circuit.
        if (gate->getOutput() != nullptr) {
            // For each gate that has an output, examine all other gates to identify which ones are dependent on this output.
            // A dependent gate is one that uses the current gate's output as an input to its own operation.
            for (auto& dependentGate : gates) {
                // Check if the dependent gate uses the current gate's output as either of its inputs.
                if (dependentGate->getInput1() == gate->getOutput() || 
                    dependentGate->getInput2() == gate->getOutput()) {
                    // If the dependent gate is found, it means the current gate directly influences the dependent gate.
                    // Therefore, add an edge in the graph from the current gate to the dependent gate to represent this relationship.
                    adjList[gate].push_back(dependentGate);
                }
            }
        }
    }
    // By the end of this process, `adjList` contains a complete representation of the circuit as a directed graph.
    // Each gate (node) is connected to the gates (nodes) that depend on its output, allowing for analysis of the circuit's computational dependencies, such as through topological sorting.
}

// This function visits gates in a depth-first manner and records the order of traversal. / Utility function for performing depth-first search (DFS) as part of a topological sort.
void Circuit::topologicalSortUtil(Gate* gate, std::map<Gate*, bool>& visited, std::stack<Gate*>& stack) {
    // Mark the current gate as visited to prevent revisiting and to handle cyclic dependencies.
    visited[gate] = true;
    // Recursively explore all gates that are directly dependent on the current gate.
    // This is to ensure that all dependencies of a gate are processed before the gate itself.
    for (auto& dependentGate : adjList[gate]) {
        // Check if the dependent gate has not been visited yet.
        if (!visited[dependentGate]) {
            // If the dependent gate is unvisited, recursively call this function to visit it.
            // This step ensures that we dive deeper into the dependency chain before processing the current gate.
            topologicalSortUtil(dependentGate, visited, stack);
        }
    }
    // Once all dependencies of the current gate have been processed (due to recursion reaching the base case), push the current gate onto the stack.
    // This action effectively places the gate in a post-order sequence, where a gate appears in the sequence only after all gates dependent on it have been included.
    stack.push(gate);
    // The use of a stack here is critical for topological sorting: it reverses the order of processing, ensuring that gates are processed from least dependent to most dependent.
    // This is essential for computing gate outputs in a logical sequence during simulation.
}

// Conducts a topological sort on the circuit's gates to determine an order in which they can be evaluated so that each gate is computed only after all its dependencies.
std::stack<Gate*> Circuit::topologicalSort() {
    std::stack<Gate*> stack; // A stack to hold the gates in topologically sorted order.
    std::map<Gate*, bool> visited; // A map to track whether each gate has been visited during the sort.
    // Initialize all gates as not visited by setting their corresponding values in the 'visited' map to false.
    // This preparation step is crucial for the depth-first search (DFS) algorithm, ensuring that each gate is visited exactly once during the sorting process.
    for (auto& gate : gates) {
        visited[gate] = false;
    }
    // Iterate over all gates in the circuit to ensure that each gate is processed, even if the circuit contains disconnected components or if not all gates are reachable from a single starting point.
    for (auto& gate : gates) {
        // If the current gate has not yet been visited, initiate a depth-first search from it.
        // This call will explore the gate, along with all gates that are dependent on it, marking them as visited, and placing them onto the stack in an order such
        // that a gate is only placed on the stack after all gates that depend on its output have been placed on the stack.
        if (!visited[gate]) {
            topologicalSortUtil(gate, visited, stack);
        }
    }
    // Return the stack containing the gates in topologically sorted order.
    // The stack's top will have the gate that should be computed last, while the stack's bottom will have the gate that should be computed first.
    // This order ensures that when gates are popped from the stack and processed, each gate is processed only after all its input dependencies have been resolved.
    return stack;
}

// Writes the results of the circuit simulation to a text file.
void Circuit::printGoodSimulationResults(const std::vector<std::vector<bool>>& results) {
    // Determine the number of input wires to calculate the total number of possible input combinations.
    const size_t numInputs = inputs.size();
    // Calculate the total number of input combinations using bitwise left shift, effectively computing 2^numInputs.
    const size_t numCombinations = 1 << numInputs;
    // Create an output file stream to write the simulation results into a file named "simulation_results.txt".
    std::ofstream outFile("simulation_results.txt"); // Open file for writing
    
    // Iterate through each possible input combination to document the corresponding output values.
    for (size_t i = 0; i < numCombinations; ++i) {
        // Begin each line by listing the input combination being considered.
        outFile << "inputs: ";
        // Iterate through each input wire to display the current combination's value for that input.
        for (size_t j = 0; j < numInputs; ++j) {
            // Calculate the value of the j-th input for the i-th combination by right-shifting i by j positions
            // and isolating the least significant bit (& 1). This effectively extracts the j-th bit of i.
            outFile << ((i >> j) & 1) << (j < numInputs - 1 ? "," : " ");
            // This produces a sequence of 0s and 1s representing the current input state, separated by commas.
        }
        // After listing the inputs, document the corresponding outputs generated by the circuit.
        outFile << "gives outputs: ";
        // Iterate through the output values stored in the results vector for the current input combination.
        for (size_t k = 0; k < results[i].size(); ++k) {
            // Write each output value, followed by a comma unless it's the last value, in which case end the line.
            outFile << results[i][k] << (k < results[i].size() - 1 ? "," : "\n");
        }
        // This loop results in a line for each input combination, showing the input values and their resulting outputs.
    }
    // After documenting all input-output combinations, close the file to ensure data is written and resources are released.
    outFile.close();
}

// Conducts a fault simulation for the entire circuit, testing for stuck-at-0 and stuck-at-1 faults on all wires except outputs.
void Circuit::runFaultedSimulation() {
    // First, run a simulation of the circuit without any faults to establish a baseline of correct behavior.
    auto goodResults = runGoodSimulation();

    // Iterate over all wires in the circuit, except for the output wires, to inject faults.
    // Output wires are typically excluded because faults are usually injected into internal or input wires to see their effect on outputs.
    for (Wire* wire : getAllWiresButOutputs()) {
        // Initialize an array to track whether each type of fault (stuck-at-0, stuck-at-1) is detected.
        bool faultDetected[2] = {false, false};

        // For each wire, simulate both stuck-at-0 and stuck-at-1 faults.
        for (int faultType = 0; faultType <= 1; ++faultType) {
            // Inject a fault into the current wire, where 'faultType' determines the nature of the fault.
            injectFault(wire, faultType);
            // Run the simulation again with the fault injected to observe the circuit's behavior under faulty conditions.
            std::vector<std::vector<bool>> faultedResults = runGoodSimulation();
            // Compare the results of the faulted simulation with the results of the good simulation to determine if the fault was detected.
            // A fault is considered detected if the output of the circuit with the fault differs from the output of the circuit without faults.
            faultDetected[faultType] = compareResults(goodResults, faultedResults, wire, faultType);
            // Remove the injected fault from the wire, restoring it to its normal state before proceeding to the next fault type or wire.
            removeFault(wire);
        }

        // After testing both fault types for the current wire, check if either fault went undetected.
        for (int faultType = 0; faultType <= 1; ++faultType) {
            if (!faultDetected[faultType]) {
                // If a fault was undetected, output a message indicating the wire and the type of fault that was missed.
                std::cout << "Fault was undetected for " << wire->getName() << " stuck-at-" << faultType << "\n";
            }
        }
    }
    // This process effectively tests the circuit's ability to detect stuck-at faults in all non-output wires,
    // highlighting potential vulnerabilities in the circuit's design or testing methodology.
}

// Compares the outputs from a fault-free simulation against outputs with a fault injected to determine if the fault is detectable.
bool Circuit::compareResults(const std::vector<std::vector<bool>>& goodResults, const std::vector<std::vector<bool>>& faultedResults, Wire* wire, int faultType) {
    // Count the number of input wires to the circuit, which determines the number of input combinations tested.
    const size_t numInputs = inputs.size();
    // Initialize a flag to track if the fault affects the circuit's behavior (output) in any way.
    bool faultDetected = false;

    // Open a file in append mode to log the results of the fault injection tests.
    // This allows for multiple fault simulation results to be recorded in the same file without overwriting previous data.
    std::ofstream outFile("stuck-at-results.txt", std::ios::app);

    // Iterate through all the input combinations tested in the simulations.
    for (size_t i = 0; i < goodResults.size(); ++i) {
        // Compare the output for the current input combination between the fault-free and fault-injected simulations.
        if (goodResults[i] != faultedResults[i]) {
            // If there is a discrepancy, it means the fault has an observable impact on the circuit's output, thus detected.
            faultDetected = true;
            // Log the detection, specifying the wire with the fault, the type of fault, and the input combination that led to detection.
            outFile << "\\" << wire->getName() 
                    << " stuck-at-" << faultType 
                    << " with inputs: ";
            // Iterate through each input wire to document the binary input combination tested.
            for (size_t j = 0; j < numInputs; ++j) {
                // Calculate the value of each input wire for the current combination by bitwise operations.
                bool inputValue = (i >> j) & 1;
                // Write the input value to the file, separated by commas.
                outFile << inputValue << (j < numInputs - 1 ? ", " : "");
            }
            outFile << "\n";
            // After logging the first detection, exit the loop. This is because identifying one instance of fault detection is often sufficient to prove the fault's impact.
            // However, this behavior can be changed to log all detections.
            break;
        }
    }
    // If after examining all input combinations, no fault impact is detected, log that the fault was undetectable for this wire.
    if (!faultDetected) {
        outFile << "No fault detected on wire \\" << wire->getName() << " stuck-at-" << faultType << "\n";
    }
    // Ensure the file is properly closed after writing to prevent data loss and release system resources.
    outFile.close();
    // Return the flag indicating whether the fault was detected. This information can be used for further analysis or reporting.
    return faultDetected;
}

// Searches for a wire by its name within the circuit and returns a pointer to the wire if found.
Wire* Circuit::findWireByName(const std::string& name) {
    // First, iterate through the list of input wires in the circuit.
    for (auto& wire : inputs) {
        if (wire->getName() == name) {
            // If a match is found, return a pointer to this wire.
            return wire;
        }
    }
    // If the wire was not found among the inputs, proceed to search among the output wires.
    for (auto& wire : outputs) {
        if (wire->getName() == name) {
            // If a match is found among the outputs, return a pointer to this wire.
            return wire;
        }
    }
    // If the wire was not found among the inputs or outputs, search among the internal wires.
    for (auto& wire : internalWires) {
        if (wire->getName() == name) {
            // If a match is found among the internal wires, return a pointer to this wire.
            return wire;
        }
    }
    // return nullptr to indicate that no wire with the specified name exists in the circuit.
    return nullptr; 
}

// Gathers all wires in the circuit into a single vector.
std::vector<Wire*> Circuit::getAllWires() const {
    std::vector<Wire*> allWires; // Initialize an empty vector to hold all wires.

    // Insert input wires into the 'allWires' vector.
    allWires.insert(allWires.end(), inputs.begin(), inputs.end());
    // Insert output wires into the 'allWires' vector in a similar manner to input wires.
    allWires.insert(allWires.end(), outputs.begin(), outputs.end());
    // Insert internal wires into the 'allWires' vector.
    allWires.insert(allWires.end(), internalWires.begin(), internalWires.end());
    // Return the compiled vector containing all wires in the circuit.
    return allWires;
}

// Gathers all wires in the circuit, excluding output wires, into a single vector.
std::vector<Wire*> Circuit::getAllWiresButOutputs() const {
    std::vector<Wire*> allWires; // Initialize an empty vector to hold the relevant wires.

    // Insert input wires into the 'allWires' vector.
    allWires.insert(allWires.end(), inputs.begin(), inputs.end());
    // Insert internal wires into the 'allWires' vector.
    allWires.insert(allWires.end(), internalWires.begin(), internalWires.end());
    // Return the compiled vector containing input and internal wires, but excluding output wires.
    return allWires;
}

// Adds a wire to the list of input wires for the circuit.
void Circuit::addInput(Wire* wire) {
    inputs.push_back(wire);
}

// Adds a wire to the list of output wires for the circuit.
void Circuit::addOutput(Wire* wire) {
    outputs.push_back(wire);
}

// Adds a wire to the list of internal wires for the circuit.
void Circuit::addInternalWire(Wire* wire) {
    internalWires.push_back(wire);
}

// Adds a wire to the list of gates for the circuit.
void Circuit::addGate(Gate* gate) {
    gates.push_back(gate);
}

// Adds a fault to a gate.
void Circuit::injectFault(Wire* wire, bool faultType) {
    if (wire) {
        wire->setFault(true, faultType);
    }
}

// Removes a fault from a gate.
void Circuit::removeFault(Wire* wire) {
    if (wire) {
        wire->clearFault();
    }
}