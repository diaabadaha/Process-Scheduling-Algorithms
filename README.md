# ProcessSchedulingVisualizer

## Overview
This repository contains a C++ program that simulates various CPU scheduling algorithms for a set of processes over a fixed period. It aims to demonstrate the effectiveness and operational differences between multiple scheduling techniques.

## Repository Contents
- `processesScheduling.cpp`: The main C++ source file that implements the scheduling algorithms and simulation logic.
- `scheduling.pdf`: The task description document detailing the processes, scheduling algorithms, and expected outputs.

## Supported Scheduling Algorithms
- **First Come First Served (FCFS)**
- **Shortest Job First (SJF)**
- **Shortest Remaining Time First (SRTF)**
- **Round Robin (RR)** with a quantum of 5 time units
- **Preemptive Priority Scheduling** with aging
- **Non-preemptive Priority Scheduling** with aging

## Features
- Simulates CPU scheduling for a predefined set of processes.
- Calculates and displays the Gantt chart for each scheduling algorithm.
- Computes average waiting time and turnaround time for each algorithm.
- Handles process arrival, burst time, priority, and aging mechanisms.

## How It Works
The program runs a simulation for 200 time units, processing each algorithm as follows:
1. **Initialization**: Load processes from the predefined set in the task description.
2. **Scheduling Execution**: Each algorithm schedules the processes based on its specific criteria.
3. **Output**: The program outputs a Gantt chart, along with the average waiting and turnaround times for each scheduling method.

## Usage
To compile and run the program, use a C++ compiler.

## Contribution
This project serves educational purposes in understanding operating system concepts, specifically in process scheduling. It is designed for students and educators alike to explore and analyze the behavior and efficiency of different scheduling algorithms.
