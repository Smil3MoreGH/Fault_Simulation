#pragma once
#include "Wire.h"

class Gate {
public:
    enum GateType { AND, OR, NOT, BUFFER };
    Gate(GateType type, Wire* input1, Wire* input2, Wire* output, bool negInput1 = false, bool negInput2 = false);
    ~Gate();

    void computeOutput();

private:
    GateType type;
    Wire* input1;
    Wire* input2;
    Wire* output;
    bool negInput1;
    bool negInput2;
};