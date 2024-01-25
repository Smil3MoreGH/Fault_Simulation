#include <iostream>
#include "Circuit.h"

int main(int argc, char* argv[])
{
    Circuit circuit;
    std::string filepathEthernet = "C:/Users/Paul/RiderProjects/Fault_Simulation/Fault_Simulation/Benches/ethernet_synth_NEW.v";
    std::string filepathC17 = "C:/Users/Paul/RiderProjects/Fault_Simulation/Fault_Simulation/Benches/C17_orig.v";
    std::string filepathC17_better = "C:/Users/Paul/RiderProjects/Fault_Simulation/Fault_Simulation/Benches/C17_verbessert.v";
    
    circuit.loadFromFile(filepathC17_better);
    circuit.runGoodSimulation();
    circuit.runFaultedSimulation();
    
    return 0;
}
