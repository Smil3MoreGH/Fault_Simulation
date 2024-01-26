#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>


Parser::Parser() {
    
}

void Parser::parse(const std::string& filepath, Circuit& circuit) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filepath << std::endl;
        return;
    }

    std::string line, partialLine;
    while (std::getline(file, line))
    {
        if (line.find("//") == 0 || line.find("module") == 0 || line.find("end;") == 0) {
            continue;
        }
        // Prüfen, ob die Zeile mit 'input' beginnt und ob sie ein Semikolon enthält
        if (line.find("input") != std::string::npos) {
            partialLine = line;
            // Weiterlesen und Anhängen der Zeilen, bis ein Semikolon gefunden wird
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line; // Füge die nächste Zeile hinzu
            }
            parseInput(partialLine, circuit);
            
        } else if (line.find("output") != std::string::npos) {
            partialLine = line;
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line;
            }
            parseOutput(partialLine, circuit);
            
        } else if (line.find("wire") != std::string::npos) {
            partialLine = line;
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line;
            }
            parseWire(partialLine, circuit);
        }
    }
    
    file.clear();
    file.seekg(0);
    
    while (std::getline(file, line)) {
        if (line.find("assign") != std::string::npos) {
            partialLine = line;
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line;
            }
            parseAssign(partialLine, circuit);
        }
    }
}

void Parser::parseInput(const std::string& line, Circuit& circuit) {
    // Entfernen des "input"-Teils und Trimmen der Zeile
    size_t inputPos = line.find("input");
    if (inputPos != std::string::npos) {
        std::string inputs = line.substr(inputPos + 5); // "+ 5" um "input" zu überspringen
        inputs.erase(0, inputs.find_first_not_of(" \t\n"));

        std::istringstream iss(inputs);
        std::string token;

        while (std::getline(iss, token, ',')) {  // Zerlegen anhand von Kommas
            token = trim(token);

            // Entfernen von zusätzlichen Zeichen am Ende (falls vorhanden)
            size_t endPos = token.find(';');
            if (endPos != std::string::npos) {
                token = token.substr(0, endPos);
            }

            // Erstellen des Gates und Hinzufügen zum Schaltkreis
            Wire* newWire = new Wire(token);
            circuit.addInput(newWire);
            //std::cout << "Input Wire erstellt: " << token << std::endl;
        }
    }
}

void Parser::parseOutput(const std::string& line, Circuit& circuit) {
    size_t outputPos = line.find("output");
    if (outputPos != std::string::npos) {
        std::string outputs = line.substr(outputPos + 6);
        outputs.erase(0, outputs.find_first_not_of(" \t\n"));

        std::istringstream iss(outputs);
        std::string token;

        while (std::getline(iss, token, ',')) {
            token = trim(token);
            
            size_t endPos = token.find(';');
            if (endPos != std::string::npos) {
                token = token.substr(0, endPos);
            }

            Wire* newWire = new Wire(token);
            circuit.addOutput(newWire);
            //std::cout << "Output Wire erstellt: " << token << std::endl;
        }
    }
}

void Parser::parseWire(const std::string& line, Circuit& circuit) {
    size_t wirePos = line.find("wire");
    if (wirePos != std::string::npos) {
        std::string wires = line.substr(wirePos + 4);
        wires.erase(0, wires.find_first_not_of(" \t\n"));

        std::istringstream iss(wires);
        std::string token;

        while (std::getline(iss, token, ',')) {
            token = trim(token);
            
            size_t endPos = token.find(';');
            if (endPos != std::string::npos) {
                token = token.substr(0, endPos);
            }

            Wire* newWire = new Wire(token);
            circuit.addInternalWire(newWire);
            //std::cout << "Internal Wire erstellt: " << token << std::endl;
        }
    }
}

void Parser::parseAssign(const std::string& line, Circuit& circuit) {
    std::istringstream iss(line.substr(line.find("assign") + 6)); // Skip "assign" and parse
    std::string leftPart, rightPart;

    std::getline(iss, leftPart, '=');
    std::getline(iss, rightPart);

    leftPart = trim(leftPart);
    rightPart = trim(rightPart);

    Gate::GateType gateType;
    char delimiter = ' '; // Initialize with a default value
    bool isPossibleNotorBUFFER = rightPart.find("&") == std::string::npos && rightPart.find("|") == std::string::npos;
    if (rightPart.find("&") != std::string::npos) {
        gateType = Gate::AND;
        delimiter = '&';
    } else if (rightPart.find("|") != std::string::npos) {
        gateType = Gate::OR;
        delimiter = '|';
    } else if (isPossibleNotorBUFFER) {
        // Check for '~' to distinguish between NOT and BUFFER
        if (rightPart.find("~") != std::string::npos) {
            gateType = Gate::NOT;
            delimiter = 'N';
        } else {
            gateType = Gate::BUFFER;
            delimiter = 'B';
        }
    }

    std::string input1, input2;
    Wire* wireInput1 = nullptr;
    Wire* wireInput2 = nullptr;
    bool negInput1 = false;
    bool negInput2 = false;

    if (gateType == Gate::AND || gateType == Gate::OR) {
        std::istringstream inout(rightPart);
        std::getline(inout, input1, delimiter);
        std::getline(inout, input2);

        input1= trim(input1);
        input2 = trim(input2);
        
        // Check for '~' and update negation variables for each input
        negInput1 = !input1.empty() && input1[0] == '~';
        if (negInput1) {
            input1.erase(0, 1);  // Remove '~' if present
        }
        negInput2 = !input2.empty() && input2[0] == '~';
        if (negInput2) {
            input2.erase(0, 1);  // Remove '~' if present
        }
        
        wireInput1 = circuit.findWireByName(input1);
        wireInput2 = circuit.findWireByName(input2);

    } else if (gateType == Gate::NOT) {
        // For NOT gates, only one input is used
        bool negInput1 = !rightPart.empty() && rightPart[0] == '~';
        if (negInput1) {
            rightPart.erase(0, 1);  // Remove '~' if present
        }
        wireInput1 = circuit.findWireByName(trim(rightPart));
    } else if (gateType == Gate::BUFFER) {
        // For BUFFER gates, treat the right part as a single input wire
        wireInput1 = circuit.findWireByName(trim(rightPart));
    }

    Wire* wireOutput = circuit.findWireByName(leftPart);
    if (!wireOutput) {
        std::cout << "leftPart nicht gefunden: " << leftPart << std::endl;
    }
    // Create the gate if required wires are found
    if (wireOutput && wireInput1) {
        // negInput1 and negInput2 are already set correctly above
        Gate* newGate = new Gate(gateType, wireInput1, wireInput2, wireOutput, negInput1, negInput2);
        circuit.addGate(newGate);
        //std::cout << "Gate erstellt: " << wireOutput->getName() << std::endl;
    }
}

std::string Parser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n");
    if (start == std::string::npos) {
        // The string consists only of spaces or is empty.
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n;");
    return str.substr(start, end - start + 1);
}