# Process Scheduling Simulation

## Authors
- **BT22ECI002** Jjateen Gundesha
- **BT22ECI018** B Manish
- **BT22ECI030** Sumit Bharti
- **BT22ECI035** Kaki Sai Chandra Moulyyy
- **BT22ECI036** Mohit Talwar

## Overview
This project implements different CPU scheduling algorithms, including:
- Shortest Remaining Time First (SRTF)
- Shortest Job First (SJF)
- Round Robin (RR)
- Virtual Round Robin (VRR)

The scheduling algorithms are implemented in C, and a `Makefile` is provided for easy compilation and execution.

## Usage
Follow these steps to compile and run the scheduling simulations:

### 1. Clean Previous Builds (Optional)
```sh
make clean
```
This removes any previously compiled binaries and output files.

### 2. Compile All Programs
```sh
make all
```
This compiles all the scheduling algorithms.

### 3. Run a Specific Scheduling Algorithm
- Run Shortest Remaining Time First (SRTF):
  ```sh
  make run_srtf
  ```
  Output will be saved in `srtf_output.txt`.

- Run Shortest Job First (SJF):
  ```sh
  make run_sjf
  ```
  Output will be saved in `sjf_output.txt`.

- Run Round Robin (RR):
  ```sh
  make run_rr
  ```
  Output will be saved in `round_robin_output.txt`.

- Run Virtual Round Robin (VRR):
  ```sh
  make run_vrr
  ```
  Output will be saved in `virtual_round_robin_output.txt`.

### 4. Clean Up
To remove compiled executables and output files:
```sh
make clean
```

## Notes
- Ensure you have `gcc` installed to compile the C programs.
- The outputs are redirected to respective text files for easier analysis.