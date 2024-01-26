#pragma once
#include "Wire.h"

class Gate {
public:
    enum GateType { AND, OR, NOT, BUFFER };
    Gate(GateType type, Wire* input1, Wire* input2, Wire* output, bool negInput1 = false, bool negInput2 = false);
    ~Gate();

    void computeOutput();
    void setLevel(int l) { level = l; }
    int getLevel() const { return level; }

private:
    GateType type;
    Wire* input1;
    Wire* input2;
    Wire* output;
    bool negInput1;
    bool negInput2;
    int level;
};