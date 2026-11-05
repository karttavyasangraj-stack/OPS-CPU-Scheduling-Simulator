# CPU Scheduling Algorithm Simulator (C++)

A menu-driven C++ mini project demonstrating four CPU scheduling algorithms: FCFS, non-preemptive SJF, non-preemptive Priority, and Round Robin.

## Features
- Accepts 1–20 processes with arrival time, burst time and priority.
- Validates process count, non-negative arrival/priority, positive burst and positive Round Robin quantum.
- Handles CPU idle intervals.
- Displays Gantt chart, completion time, turnaround time, waiting time and response time.
- Calculates average waiting, turnaround and response times.
- Runs one algorithm or all algorithms on the same process set.

## Compile and run
Linux/macOS: `g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o cpu_scheduler && ./cpu_scheduler`

Windows (MinGW): `g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o cpu_scheduler.exe` then run `cpu_scheduler.exe`.

## Formulae
- Turnaround Time = Completion Time − Arrival Time
- Waiting Time = Turnaround Time − Burst Time
- Response Time = First CPU Start Time − Arrival Time

Priority convention: smaller number means higher priority. SJF and Priority are non-preemptive; Round Robin is preemptive. Context-switch overhead is not modelled.

## OOP concepts demonstrated
The project includes an abstract `SchedulingStrategy` base class and four derived scheduler classes. Each overrides the virtual `execute()` method, and the menu invokes algorithms through a base-class pointer to demonstrate inheritance and runtime polymorphism. Process and Gantt-segment records group related data; scheduling and display work is separated into modular functions.
