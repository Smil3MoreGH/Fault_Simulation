#include <iostream>
#include "Circuit.h"

int main(int argc, char* argv[])
{
    Circuit circuit;
    std::string filepathEthernet = "C:/Users/Paul/RiderProjects/Fault_Simulation/Fault_Simulation/Benches/ethernet_synth_NEW.v";
    std::string filepathC17 = "C:/Users/Paul/RiderProjects/Fault_Simulation/Fault_Simulation/Benches/C17_orig.v";
    std::string filepathC17_better = "C:/Users/Paul/RiderProjects/Fault_Simulation/Fault_Simulation/Benches/C17_verbessert.v";
    std::string filepathTestB = "C:/Users/Paul/RiderProjects/Fault_Simulation/Fault_Simulation/Benches/TestB.v";
    
    circuit.loadFromFile(filepathEthernet);
    circuit.runAndPrintGoodSimulation();
    circuit.runFaultedSimulation();
    
    return 0;
}
