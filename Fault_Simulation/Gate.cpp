#include "Gate.h"

// Constructor for the Gate class.
// Initializes a gate with specified type, inputs, outputs, and negation properties.
Gate::Gate(GateType type, Wire* input1, Wire* input2, Wire* output, bool negInput1, bool negInput2)
    : type(type), // The logical type of the gate (AND, OR, NOT, BUFFER).
      input1(input1), // The first input wire to the gate.
      input2(input2), // The second input wire to the gate (if applicable).
      output(output), // The output wire of the gate.
      negInput1(negInput1), // Indicates if the first input is logically negated before being used.
      negInput2(negInput2) // Indicates if the second input is logically negated before being used.
{
    
}

Gate::~Gate() {
    
}

// Computes and sets the output value of the gate based on its inputs and type.
void Gate::computeOutput() {
    // Calculate the effective value of the first input, applying negation if specified.
    bool val1 = input1 ? (negInput1 ? !input1->getValue() : input1->getValue()) : false;
    // Calculate the effective value of the second input, applying negation if specified.
    // For NOT and BUFFER gates, which do not use a second input, this value is set to false.
    bool val2 = (input2 && type != Gate::NOT && type != Gate::BUFFER) ? (negInput2 ? !input2->getValue() : input2->getValue()) : false;
    bool result; // The result of the gate's logical operation.

    // Determine the result based on the gate's type.
    switch (type) {
        case AND:
            result = val1 && val2; // Logical AND operation.
            break;
        case OR:
            result = val1 || val2; // Logical OR operation.
            break;
        case NOT:
            result = !val1; // Logical NOT operation.
            break;
        case BUFFER:
            result = val1; // BUFFER operation simply passes the input to the output.
            break;
        default:
            result = false; // Default case for safety, should not be reached.
            break;
    }

    // Set the computed result as the output wire's value.
    output->setValue(result);
}