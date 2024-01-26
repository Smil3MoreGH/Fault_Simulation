#pragma once
#include "Wire.h"

class Gate {
public:
    enum GateType { AND, OR, NOT, BUFFER };
    Gate(GateType type, Wire* input1, Wire* input2, Wire* output, bool negInput1 = false, bool negInput2 = false);
    ~Gate();

    void computeOutput();
    Wire* getInput1() const { return input1; }
    Wire* getInput2() const { return input2; }
    Wire* getOutput() const { return output; }

private:
    GateType type;
    Wire* input1;
    Wire* input2;
    Wire* output;
    bool negInput1;
    bool negInput2;
};