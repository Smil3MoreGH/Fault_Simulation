#pragma once
#include "Circuit.h"
#include <string>

class Parser
{
public:
    Parser();
    // Methode zum Parsen der Datei und zum Aufbau des Schaltkreises
    void parse(const std::string& filepath, Circuit& circuit);


private:
    // Hilfsmethoden für das Parsen spezifischer Elemente der Datei
    void parseInput(const std::string& line, Circuit& circuit);
    void parseOutput(const std::string& line, Circuit& circuit);
    void parseWire(const std::string& line, Circuit& circuit);
    void parseAssign(const std::string& line, Circuit& circuit);
    std::string trim(const std::string& str);
        
};