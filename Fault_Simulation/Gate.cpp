#include "Gate.h"

Gate::Gate(GateType type, Wire* input1, Wire* input2, Wire* output, bool negInput1, bool negInput2)
    : type(type), input1(input1), input2(input2), output(output), 
      negInput1(negInput1), negInput2(negInput2) {
}


Gate::~Gate() {
    
}

void Gate::computeOutput() {
    bool val1 = input1 ? (negInput1 ? !input1->getValue() : input1->getValue()) : false;
    bool val2 = (input2 && type != Gate::NOT) ? (negInput2 ? !input2->getValue() : input2->getValue()) : false;
    bool result;

    switch (type) {
    case AND:
        result = val1 && val2;
        break;
    case OR:
        result = val1 || val2;
        break;
    case NOT:
        result = !val1;  // For NOT gate, only consider the first input
        break;
    default:
        result = false;
        break;
    }

    output->setValue(result);  // Directly set the output value
}

