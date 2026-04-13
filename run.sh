#!/bin/bash

# HRMA Standalone Simulation Runner for Linux

# Check if g++ is installed
if ! command -v g++ &> /dev/null
then
    echo "g++ could not be found. Please install it with: sudo apt install g++"
    exit
fi

echo "Compiling HRMA Standalone Simulation..."
g++ hrma-standalone.cpp -o hrma-standalone

if [ $? -eq 0 ]; then
    echo "Compilation successful. Running simulation..."
    ./hrma-standalone
else
    echo "Compilation failed."
fi

echo ""
echo "------------------------------------------------"
echo "Note for ns-3 users:"
echo "To run the full ns-3 simulation with graphs, copy hrma-simulation.cc to your ns-3 scratch folder."
echo "Then run: ./ns3 run scratch/hrma-simulation"
echo "Followed by: gnuplot hrma-throughput.plt"
echo "------------------------------------------------"
