#pragma once
#include <vector>
#include <string>
#include "Wire.h"
#include "Gate.h"

class Circuit {
public:
    Circuit();
    ~Circuit();

    void loadFromFile(const std::string& filepath);

    std::vector<std::vector<bool>> runGoodSimulation();

    void runFaultedSimulation();
    void printGoodSimulationResults(const std::vector<std::vector<bool>>& results);
    void runAndPrintGoodSimulation();

    std::vector<Wire*> inputs;
    std::vector<Wire*> outputs;
    std::vector<Wire*> internalWires;
    std::vector<Gate*> gates;

    Wire* findWireByName(const std::string& name);
    void addInput(Wire* wire);
    void addOutput(Wire* wire);
    void addInternalWire(Wire* wire);
    void addGate(Gate* gate);
};