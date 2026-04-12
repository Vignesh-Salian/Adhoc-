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

## How to Run

### Option 1: Standalone Output Simulation (Easiest - Windows/Mac/Linux)
If you only need the simulation output of the frequency hopping logic for logging purposes, you can run the standalone program using standard C++ compilers (like MinGW `g++` on Windows).

**Steps:**
1. Open your terminal (or PowerShell).
2. Navigate to this directory.
3. Compile the code:
   ```bash
   g++ hrma-standalone.cpp -o hrma-standalone
   ```
4. Run the executable:
   - On **Windows**: `.\hrma-standalone.exe`
   - On **Linux/Mac**: `./hrma-standalone`

**Expected Output:**
```
0.00s: [HRMA] Active on Synchronizing Slot (f0)
0.01s: [HRMA] Active on Frequency Pair Slot 1 (f1 & f1*)
        -> Periods: [SYN] | [Hop Request] | [RTS] | [CTS] | Data Transmission
...
```

### Option 2: Running the ns-3 Structural Baseline (Requires Linux/WSL)
If you have a full ns-3 environment set up (typically on Linux or WSL on Windows), you can run the `hrma-simulation.cc` file. Note that since ns-3 does not natively support HRMA, this provides the Ad-Hoc skeleton and schedules the hopping events.

**Steps:**
1. Copy `hrma-simulation.cc` into your ns-3 `scratch` directory.
   ```bash
   cp hrma-simulation.cc /path/to/ns-3-dev/scratch/
   ```
2. Navigate to your main ns-3 directory.
3. Run using the ns-3 build tool:
   ```bash
   ./ns3 run scratch/hrma-simulation
   ```

---
*Created for Ad-Hoc Networks university assignment/research.*
