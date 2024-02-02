#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

Parser::Parser() {
    
}

Parser::~Parser() {
    
}

// Parses the file at the given filepath and updates the provided Circuit object based on the file's contents.
void Parser::parse(const std::string& filepath, Circuit& circuit) {
    std::ifstream file(filepath); // Opens the file for reading.
    // Check if the file opening was successful.
    if (!file) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filepath << std::endl; // Error message if file cannot be opened.
        return; // Exit the function if file cannot be opened.
    }

    std::string line, partialLine; // Variables to hold the current line and a partial line accumulation.
    // Read the file line by line.
    while (std::getline(file, line)) {
        // Skip lines starting with "//", "module", or "end;" as they are comments or not relevant for parsing.
        if (line.find("//") == 0 || line.find("module") == 0 || line.find("end;") == 0) {
            continue;
        }
        
        // Check if the line contains definitions for inputs, outputs, or wires, and if it does not end with a semicolon.
        if (line.find("input") != std::string::npos) {
            partialLine = line; // Start accumulating lines for a complete 'input' definition.
            // Keep reading and appending lines until a semicolon is found, indicating the end of the definition.
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line; // Append the next line to the current accumulated definition.
            }
            parseInput(partialLine, circuit); // Parse the accumulated 'input' definition.
            
        } else if (line.find("output") != std::string::npos) {
            partialLine = line; // Start accumulating lines for a complete 'output' definition.
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line; // Append the next line.
            }
            parseOutput(partialLine, circuit); // Parse the accumulated 'output' definition.
            
        } else if (line.find("wire") != std::string::npos) {
            partialLine = line; // Start accumulating lines for a complete 'wire' definition.
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line; // Append the next line.
            }
            parseWire(partialLine, circuit); // Parse the accumulated 'wire' definition.
        }
    }
    
    // Reset the file stream to start reading the file again from the beginning.
    file.clear(); // Clears any errors.
    file.seekg(0); // Seeks back to the beginning of the file.

    // Second pass: specifically looking for "assign" statements now that inputs, outputs, and wires have been parsed.
    while (std::getline(file, line)) {
        // Check if the line contains an 'assign' statement.
        if (line.find("assign") != std::string::npos) {
            partialLine = line; // Start accumulating lines for a complete 'assign' definition.
            // Keep reading and appending lines until a semicolon is found, indicating the end of the definition.
            while (partialLine.find(';') == std::string::npos && std::getline(file, line)) {
                partialLine += " " + line; // Append the next line to the current accumulated definition.
            }
            parseAssign(partialLine, circuit); // Parse the accumulated 'assign' definition.
        }
    }
}

// Parses a line from the file that specifies input pins for the circuit.
void Parser::parseInput(const std::string& line, Circuit& circuit) {
    // Search for the position of the "input" keyword in the line.
    size_t inputPos = line.find("input");
    // If "input" is found in the line, proceed with parsing.
    if (inputPos != std::string::npos) {
        // Extract the part of the line following "input", which contains the input definitions.
        std::string inputs = line.substr(inputPos + 5); // Skip over the "input" keyword itself (+5 to move past it).
        // Remove leading whitespace from the extracted part to get the clean input definitions.
        inputs.erase(0, inputs.find_first_not_of(" \t\n"));

        // Use a string stream to separate the inputs listed in the extracted string by commas.
        std::istringstream iss(inputs);
        std::string token; // Variable to hold each parsed input name.

        // Iterate through the extracted inputs, separated by commas.
        while (std::getline(iss, token, ',')) {
            // Trim whitespace and other characters from each input name.
            token = trim(token);

            // Check for and remove a trailing semicolon, which might be present after the last input definition.
            size_t endPos = token.find(';');
            if (endPos != std::string::npos) {
                token = token.substr(0, endPos); // Remove the semicolon to clean up the input name.
            }

            // Create a new wire for each input and add it to the circuit's list of inputs.
            Wire* newWire = new Wire(token); // Instantiate a new wire object with the parsed input name.
            circuit.addInput(newWire); // Add the newly created wire to the circuit as an input.
            // Debugging message to console (commented out): indicates creation of a new input wire.
            //std::cout << "Input Wire erstellt: " << token << std::endl;
        }
    }
}

// Parses a line from the file that specifies output pins for the circuit.
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
            // Debugging message to console (commented out): indicates creation of a new output wire.
            //std::cout << "Output Wire erstellt: " << token << std::endl;
        }
    }
}

// Parses a line from the file that specifies internal wires for the circuit.
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
            // Debugging message to console (commented out): indicates creation of a new wire.
            //std::cout << "Internal Wire erstellt: " << token << std::endl;
        }
    }
}

// Parses an assignment line to create and add a corresponding gate to the circuit.
void Parser::parseAssign(const std::string& line, Circuit& circuit) {
    // Create an input string stream from the part of the line after "assign".
    std::istringstream iss(line.substr(line.find("assign") + 6)); // "+ 6" to skip over the word "assign".
    std::string leftPart, rightPart; // To hold the gate output and input expressions.

    // Split the line at the '=' to separate the gate output from its input expression.
    std::getline(iss, leftPart, '=');
    std::getline(iss, rightPart);

    // Trim whitespace from around the gate output and input expressions.
    leftPart = trim(leftPart);
    rightPart = trim(rightPart);

    // Determine the type of gate based on the presence of logical operators in the input expression.
    Gate::GateType gateType;
    char delimiter = ' '; // Initialize with a default value, not used directly.
    // A flag to check if the gate could be either NOT or BUFFER, which don't use '&' or '|'.
    bool isPossibleNotorBUFFER = rightPart.find("&") == std::string::npos && rightPart.find("|") == std::string::npos;
    
    if (rightPart.find("&") != std::string::npos) {
        gateType = Gate::AND; // Found an AND gate.
        delimiter = '&';
    } else if (rightPart.find("|") != std::string::npos) {
        gateType = Gate::OR; // Found an OR gate.
        delimiter = '|';
    } else if (isPossibleNotorBUFFER) {
        // If neither '&' nor '|' is found, check for a NOT gate or assume BUFFER.
        if (rightPart.find("~") != std::string::npos) {
            gateType = Gate::NOT; // Found a NOT gate.
            delimiter = 'N'; // Not used for NOT gates, but indicates negation.
        } else {
            gateType = Gate::BUFFER; // No operator found, assume BUFFER.
            delimiter = 'B'; // Not used for BUFFER gates, but placeholder.
        }
    }

    std::string input1, input2; // Variables to hold the names of the input wires.
    Wire* wireInput1 = nullptr;
    Wire* wireInput2 = nullptr;
    bool negInput1 = false; // Flag to indicate if the first input is negated.
    bool negInput2 = false; // Flag to indicate if the second input is negated.

    // Parse the inputs based on the determined gate type.
    if (gateType == Gate::AND || gateType == Gate::OR) {
        // For AND and OR gates, split the input expression at the logical operator.
        std::istringstream inout(rightPart);
        std::getline(inout, input1, delimiter);
        std::getline(inout, input2);
        // Trim and check for negation in each input.
        input1 = trim(input1);
        input2 = trim(input2);
        negInput1 = !input1.empty() && input1[0] == '~'; // Check if input1 starts with '~'.
        if (negInput1) {
            input1.erase(0, 1); // Remove '~' to clean the input name.
        }
        negInput2 = !input2.empty() && input2[0] == '~'; // Check if input2 starts with '~'.
        if (negInput2) {
            input2.erase(0, 1); // Remove '~' to clean the input name.
        }
        // Find or create the wires based on the input names.
        wireInput1 = circuit.findWireByName(input1);
        wireInput2 = circuit.findWireByName(input2);
        
    } else if (gateType == Gate::NOT || gateType == Gate::BUFFER) {
        // For NOT and BUFFER gates, only one input is used.
        // Check and handle negation for NOT gates; BUFFER gates do not use '~'.
        negInput1 = !rightPart.empty() && rightPart[0] == '~';
        if (negInput1) {
            rightPart.erase(0, 1); // Remove '~' for NOT gates.
        }
        wireInput1 = circuit.findWireByName(trim(rightPart)); // Find or create the wire for the input.
    }

    // Find or create the wire for the gate output.
    Wire* wireOutput = circuit.findWireByName(leftPart);
    if (!wireOutput) {
        std::cout << "leftPart nicht gefunden: " << leftPart << std::endl; // Error handling if output wire not found.
    }
    // If the necessary wires are found, create and add the gate to the circuit.
    if (wireOutput && wireInput1) {
        Gate* newGate = new Gate(gateType, wireInput1, wireInput2, wireOutput, negInput1, negInput2);
        circuit.addGate(newGate); // Add the new gate to the circuit.
        //std::cout << "Gate erstellt: " << wireOutput->getName() << std::endl; // Debugging message.
    }
}

// Removes leading and trailing whitespace and semicolons from the input string.
std::string Parser::trim(const std::string& str) {
    // Find the index of the first character that is not a space, tab, or newline.
    size_t start = str.find_first_not_of(" \t\n");
    // If such a character does not exist, the string is either empty or only contains whitespace.
    if (start == std::string::npos) {
        // In this case, return an empty string as there are no characters to keep.
        return "";
    }
    // Find the index of the last character that is not a space, tab, newline, or semicolon.
    size_t end = str.find_last_not_of(" \t\n;");
    // effectively removing any leading or trailing whitespace or semicolons.
    return str.substr(start, end - start + 1);
}