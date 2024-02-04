#pragma once
#include <map>
#include <stack>
#include <vector>
#include <string>
#include "Wire.h"
#include "Gate.h"

class Circuit {
public:
    Circuit();
    ~Circuit();

    void loadFromFile(const std::string& filepath);
    void runAndPrintGoodSimulation();
    void runFaultedSimulation();
    void printGoodSimulationResultsToConsole(const std::vector<std::vector<bool>>& results);
    bool compareResultsToConsole(const std::vector<std::vector<bool>>& goodResults, const std::vector<std::vector<bool>>& faultedResults, Wire* wire, int faultType);

    std::vector<std::vector<bool>> runGoodSimulation();
    void printGoodSimulationResults(const std::vector<std::vector<bool>>& results);
    bool compareResults(const std::vector<std::vector<bool>>& goodResults, 
                             const std::vector<std::vector<bool>>& faultedResults,
                             Wire* wire, int faultType);

    std::vector<Wire*> inputs;
    std::vector<Wire*> outputs;
    std::vector<Wire*> internalWires;
    std::vector<Gate*> gates;
    std::map<Gate*, std::vector<Gate*>> adjList;

    void buildGraph();
    void topologicalSortUtil(Gate* gate, std::map<Gate*, bool>& visited, std::stack<Gate*>& stack);
    std::stack<Gate*> topologicalSort();

    Wire* findWireByName(const std::string& name);
    std::vector<Wire*> getAllWires() const;
    std::vector<Wire*> getAllWiresButOutputs() const;
    void addInput(Wire* wire);
    void addOutput(Wire* wire);
    void addInternalWire(Wire* wire);
    void addGate(Gate* gate);
    void injectFault(Wire* wire, bool faultType);
    void removeFault(Wire* wire);
};