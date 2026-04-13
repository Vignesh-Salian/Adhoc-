# HRMA (Hop Reservation Multiple Access) Protocol Simulation

This repository contains simulation code for the **Hop Reservation Multiple Access (HRMA)** protocol, a multi-channel MAC protocol based on very slow frequency-hopping spread spectrum (FHSS) radios.

## About the Protocol
HRMA is a time-slot reservation protocol where each time slot is assigned a separate frequency channel. 
- It uses a dedicated synchronizing channel (`f0`).
- The remaining frequencies are divided into $M$ frequency pairs (`fi`, `fi*`).
- Each time slot is divided into four periods: **Synchronizing (SYN)**, **Hop Request (HR)**, **RTS**, and **CTS** prior to Data transmission.

## Files Included
1. `hrma-standalone.cpp`: A native, standalone C++ script that accurately models the time-slotted frequency hopping logic and log output without requiring the massive ns-3 installation.
2. `hrma-simulation.cc`: An architectural C++ starter script for the **ns-3 Network Simulator** to set up the Ad-Hoc environment for the HRMA protocol.

---

## Getting Started (Linux/cloning)

To get this code onto a Linux machine:
```bash
git clone https://github.com/Vignesh-Salian/Adhoc-.git
cd Adhoc-
```

### Option 1: Standalone Output Simulation (Easiest - Windows/Mac/Linux)
If you only need the simulation output of the frequency hopping logic for logging purposes, you can run the standalone program.

**Linux/Mac Steps:**
1. Make the run script executable:
   ```bash
   chmod +x run.sh
   ```
2. Run the simulation:
   ```bash
   ./run.sh
   ```

**Manual Compilation:**
1. Compile the code:
   ```bash
   g++ hrma-standalone.cpp -o hrma-standalone
   ```
2. Run the executable:
   - On **Linux/Mac**: `./hrma-standalone`
   - On **Windows**: `.\hrma-standalone.exe`

**Expected Output:**
```
0.00s: [HRMA] Active on Synchronizing Slot (f0)
0.01s: [HRMA] Active on Frequency Pair Slot 1 (f1 & f1*)
        -> Periods: [SYN] | [Hop Request] | [RTS] | [CTS] | Data Transmission
...
```

### Option 2: Running the ns-3 Structural Baseline (Requires Linux/WSL)
If you have a full ns-3 environment set up (typically on Linux or WSL on Windows), you can run the `hrma-simulation.cc` file. This version now includes **FlowMonitor** and **Gnuplot** integration to generate performance graphs.

**Prerequisites:**
You must have `gnuplot` installed on your Linux system:
```bash
sudo apt update && sudo apt install gnuplot -y
```

**Steps:**
1. Copy `hrma-simulation.cc` into your ns-3 `scratch` directory.
   ```bash
   cp hrma-simulation.cc /path/to/ns-3-dev/scratch/
   ```
2. Navigate to your main ns-3 directory.
3. Run the simulation:
   ```bash
   ./ns3 run scratch/hrma-simulation
   ```
4. **Generate Graphical Plots:**
   Once the simulation finished, it will create `.plt` files. Run this to generate the PNG graph:
   ```bash
   gnuplot hrma-throughput.plt
   ```
   You will now see a file named `hrma-throughput.png` in your folder!

5. **Detailed Stats:** 
   The simulation also generates `hrma-results.xml` containing detailed flow-level statistics (Delay, Jitter, Packet Loss).

6. **Network Animation (NetAnim):**
   The simulation automatically produces `hrma-animation.xml`. Use this file in the NetAnim tool to visualize node movement and packet exchanges:
   - Open NetAnim on your Linux system.
   - Click "Open" and select `hrma-animation.xml`.
   - Press "Play" to watch the simulation.

---
*Created for Ad-Hoc Networks university assignment/research.*
