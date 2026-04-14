#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

// This is a standalone simulation that mimics the ns-3 event scheduler
// to demonstrate the HRMA time-slotted frequency hopping output.

void AdvanceToNextSlot(double startTime, int startSlot, int totalSlots, double maxTime) {
    double currentTime = startTime;
    int currentSlot = startSlot;
    double slotDuration = 0.01; // 10ms per slot

    while (currentTime < maxTime) {
        if (currentSlot == 0) {
            cout << fixed << setprecision(2) << currentTime 
                 << "s: [HRMA] Active on Synchronizing Slot (f0)\n";
        } else {
            cout << fixed << setprecision(2) << currentTime 
                 << "s: [HRMA] Active on Frequency Pair Slot " << currentSlot 
                 << " (f" << currentSlot << " & f" << currentSlot << "*)\n";
            cout << "        -> Periods: [SYN] | [Hop Request] | [RTS] | [CTS] | Data Transmission\n";
        }

        currentSlot = (currentSlot + 1) % (totalSlots + 1);
        currentTime += slotDuration;
    }
}

int main() {
    int numNodes = 4;
    cout << "Setting up HRMA conceptual network topology with " << numNodes << " nodes.\n";
    cout << "Starting Simulation...\n";
    
    // M frequency pairs as described in the slides
    int M = 5; 
    double simulationTime = 0.12; // Simulate for 120ms
    
    AdvanceToNextSlot(0.0, 0, M, simulationTime);
    
    cout << "Simulation Finished.\n";
    return 0;
}
