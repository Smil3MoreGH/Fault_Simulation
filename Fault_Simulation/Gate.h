#pragma once
#include "Wire.h"

class Gate {
public:
    enum GateType { AND, OR, NOT, BUFFER };

    // Constructor
    Gate(GateType type, Wire* input1, Wire* input2, Wire* output, bool negInput1 = false, bool negInput2 = false)
        : type(type), input1(input1), input2(input2), output(output),
          negInput1(negInput1), negInput2(negInput2), level(-1) {}

    // Destructor
    ~Gate() {}

    // Method to compute the output based on gate type and inputs
    void computeOutput();

    // Set the level of the gate
    void setLevel(int l) { level = l; }

    // Get the level of the gate
    int getLevel() const { return level; }

private:
    GateType type;
    Wire* input1;
    Wire* input2;
    Wire* output;
    bool negInput1;
    bool negInput2;

    // Level of the gate in the circuit
    int level;
};
