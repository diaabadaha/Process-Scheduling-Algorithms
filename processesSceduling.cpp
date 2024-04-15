#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <algorithm>

#define TIME_LIMIT 200
#define Q 5

using namespace std;

class Process {
public:
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
    int comesBackAfter;
    int waitingTime;
    int turnaroundTime;
    int completionTime;
    int executionTime;
    int IO_time;
    int remainingTime;
    int priorityCounter;
    int tempPrio;
    int readyArrival;
    int waiting;
    bool isTurnedAround;
    int totalProcessWaiting;

    Process(int id, int arrival, int burst, int prio, int comesBack) {
        this->pid = id;
        this->arrivalTime = arrival;
        this->burstTime = burst;
        this->priority = prio;
        this->comesBackAfter = comesBack;
        this->waitingTime = 0;
        this->turnaroundTime = 0;
        this->executionTime = 0;
        this->IO_time = 0;
        this->remainingTime = burst;
        this->priorityCounter = 0;
        this->tempPrio = prio;
        this->readyArrival = 0;
        this->waiting = 0;
        this->totalProcessWaiting = 0;
    }

    void displayInfo() {
        cout << "Process " << this->pid << ": Waiting time = "
             << this->waitingTime << "Turnaround time = " << this->turnaroundTime;
    }

    void displayFullInfo() {
        cout << "Process " << this->pid << ": Arrival Time: " << this->arrivalTime
             << ", Burst Time: " << this->burstTime << ", Priority: " << this->priority
             << ", Waiting Time: " << this->waitingTime << ", Turnaround Time: "
             << this->turnaroundTime << ", Completion Time: " << this->completionTime << endl;
    }

    bool operator<(const Process& other) const {
        return comesBackAfter < other.comesBackAfter;
    }
};

vector<Process> createProcesses() {
    return {
            Process(1, 0, 10, 3, 2),
            Process(2, 1, 8, 2, 4),
            Process(3, 3, 14, 3, 6),
            Process(4, 4, 7, 1, 8),
            Process(5, 6, 5, 0, 3),
            Process(6, 7, 4, 1, 6),
            Process(7, 8, 6, 2, 9)
    };
}

// Define a comparison function for the priority queue.
auto compareProcesses = [](Process* a, Process* b) {
    return a->comesBackAfter > b->comesBackAfter;
};

void FCFS(vector<Process> processes) {
    queue<Process*> readyQueue;
    Process* CPU_process = nullptr;
    priority_queue<Process*, vector<Process*>, decltype(compareProcesses)> I_O(compareProcesses);

    cout << "\nGantt Chart:\n";

    for (int currTime = 0; currTime <= TIME_LIMIT; currTime++) {
        queue<Process*> tempQueue = readyQueue;

        while (!tempQueue.empty()) {
            Process* process = tempQueue.front();
            tempQueue.pop();

            process->waiting++;
        }

        if (currTime < 10) {
            for (Process &p : processes) {
                if (currTime == p.arrivalTime) {
                    readyQueue.push(&p);
                }
            }
        }

        // Check if a process is ready to start
        if (!CPU_process && !readyQueue.empty()) {
            CPU_process = readyQueue.front();
            CPU_process->totalProcessWaiting += CPU_process->waiting;
            CPU_process->waiting = 0;
            readyQueue.pop();
            cout << "(" << currTime << ") P" << CPU_process->pid << " | ";
        }

        if (CPU_process) {
            cout << "=";
            CPU_process->executionTime++;
            if (CPU_process->executionTime == CPU_process->burstTime) {
                if (currTime != TIME_LIMIT)
                    CPU_process->completionTime = currTime + 1;
                else
                    CPU_process->completionTime = currTime;
                CPU_process->executionTime = 0;
                I_O.push(CPU_process);
                CPU_process = nullptr; // Remove from CPU
                cout <<" | ";
            }
        }

        vector<Process*> ioFinishedProcesses;
        vector<Process*> ioNotFinishedProcesses;

        // Check I/O completion and sort processes by pid if they finished I/O at the same time
        while (!I_O.empty()) {
            Process* process = I_O.top();
            I_O.pop();
            process->IO_time += 1;
            if (process->IO_time >= process->comesBackAfter) {
                ioFinishedProcesses.push_back(process);
            } else {
                ioNotFinishedProcesses.push_back(process);
            }
        }

        // Sort the finished I/O processes by their pid
        sort(ioFinishedProcesses.begin(), ioFinishedProcesses.end(), [](const Process* a, const Process* b) {
            return a->pid < b->pid;
        });

        // Add the sorted processes to the ready queue
        for (Process* process : ioFinishedProcesses) {
            process->IO_time = 0; // Reset IO time
            readyQueue.push(process);
        }

        // Push the processes that are not done with I/O back into the I/O queue
        for (Process* process : ioNotFinishedProcesses) {
            I_O.push(process);
        }

        // Clear the vectors for the next iteration
        ioFinishedProcesses.clear();
        ioNotFinishedProcesses.clear();
    }

    cout << "(" << TIME_LIMIT << ")" << endl;

    double totalTurnAround = 0;
    double totalWaiting = 0;
    for (Process &p : processes) {
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.totalProcessWaiting;

        totalTurnAround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
    }

    double avgTurnAroundTime = totalTurnAround / processes.size();
    double avgWaitingTime = (totalWaiting - (6*(processes.size()-1))) / processes.size();

    cout << endl << "Process-wise Waiting Time and Turnaround Time:" << endl;
    for (const Process &p : processes) {
        cout << "Process " << p.pid
             << ": Waiting Time = " << p.waitingTime
             << ", Turnaround Time = " << p.turnaroundTime << endl;
    }

    cout << endl << "Avg Turnaround Time = " << avgTurnAroundTime << endl;
    cout << "Avg Waiting Time = " << avgWaitingTime << endl;
}

// Define a comparison function for the ready queue to sort processes by their burst time.
auto compareByBurstTime = [](const Process* a, const Process* b) {
    return a->burstTime > b->burstTime;
};


void SJF(vector<Process> &processes) {
    priority_queue<Process*, vector<Process*>, decltype(compareByBurstTime)> readyQueue(compareByBurstTime);
    Process* CPU_process = nullptr;
    priority_queue<Process*, vector<Process*>, decltype(compareProcesses)> I_O(compareProcesses);

    cout << "\nGantt Chart:\n";

    for (int currTime = 0; currTime <= TIME_LIMIT; currTime++) {
        priority_queue<Process*, vector<Process*>, decltype(compareByBurstTime)>  tempQueue = readyQueue;

        while (!tempQueue.empty()) {
            Process* process = tempQueue.top();
            tempQueue.pop();

            process->waiting++;
        }
        if (currTime < 10) {
            // Check for new arrivals and add them to the ready queue
            for (Process &p: processes) {
                if (currTime == p.arrivalTime) {
                    readyQueue.push(&p);
                }
            }
        }

        // Check if a process is ready to start
        if (!CPU_process && !readyQueue.empty()) {
            CPU_process = readyQueue.top();
            CPU_process->totalProcessWaiting += CPU_process->waiting;
            CPU_process->waiting = 0;
            readyQueue.pop();
            cout << "(" << currTime << ") P" << CPU_process->pid << " | ";
        }

        // Process execution
        if (CPU_process) {
            cout << "=";
            CPU_process->executionTime++;
            if (CPU_process->executionTime == CPU_process->burstTime) {
                CPU_process->isTurnedAround = true;
                if (currTime != TIME_LIMIT)
                    CPU_process->completionTime = currTime + 1;
                else
                    CPU_process->completionTime = currTime;
                CPU_process->executionTime = 0;
                I_O.push(CPU_process);
                CPU_process->IO_time = -1;
                CPU_process = nullptr;
                cout << " | ";
            }
        }

        if (currTime == TIME_LIMIT) {
            CPU_process->completionTime = TIME_LIMIT;
        }

        vector<Process*> tempProcesses;

        // Increment IO time for all processes in I_O and move them back to readyQueue if done
        while (!I_O.empty()) {
            Process* process = I_O.top();
            I_O.pop();
            process->IO_time += 1;

            if (process->IO_time >= process->comesBackAfter) {
                // If the process is done with I/O, reset IO_time and push to readyQueue
                process->IO_time = 0;
                readyQueue.push(process);
            } else {
                // If not done with I/O, add to temporary vector to push back later
                tempProcesses.push_back(process);
            }
        }

        // Push all processes that are not done with I/O back into the I_O queue
        for (Process* process : tempProcesses) {
            I_O.push(process);
        }

        tempProcesses.clear(); // C

    }

    cout << " | (" << TIME_LIMIT << ")" << endl;

    double totalTurnAround = 0;
    double totalWaiting = 0;
    int numProcesses = 0;
    for (Process &p : processes) {
        if (!p.isTurnedAround) {
            p.turnaroundTime = 0;
            p.waitingTime = 0;
        }
        else {
            numProcesses++;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.totalProcessWaiting;
        }

        totalTurnAround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
    }

    double avgTurnAroundTime = totalTurnAround / numProcesses;
    double avgWaitingTime = (totalWaiting - (6*(processes.size()-1))) / numProcesses;

    cout << endl << "Process-wise Waiting Time and Turnaround Time:" << endl;
    for (const Process &p : processes) {
        cout << "Process " << p.pid
             << ": Waiting Time = " << p.waitingTime
             << ", Turnaround Time = " << p.turnaroundTime << endl;
    }

    cout << endl << "Avg Turnaround Time = " << avgTurnAroundTime << endl;
    cout << "Avg Waiting Time = " << avgWaitingTime << endl;
}

void SRTF(vector<Process>& processes) {
    auto compareByRemainingTime = [](Process* a, Process* b) {
        return a->remainingTime > b->remainingTime;
    };

    priority_queue<Process*, vector<Process*>, decltype(compareByRemainingTime)> readyQueue(compareByRemainingTime);
    priority_queue<Process*, vector<Process*>, decltype(compareProcesses)> I_O(compareProcesses);
    Process* CPU_process = nullptr;

    cout << "\nGantt Chart:\n";

    for (int currTime = 0; currTime <= TIME_LIMIT; currTime++) {
        priority_queue<Process*, vector<Process*>, decltype(compareByRemainingTime)>  tempQueue = readyQueue;

        while (!tempQueue.empty()) {
            Process* process = tempQueue.top();
            tempQueue.pop();

            process->waiting++;
        }

        if (CPU_process) {
            cout << "=";
            CPU_process->remainingTime--;
        }

        if (currTime < 10) {
            // Check for new arrivals and add them to the ready queue
            for (Process &p: processes) {
                if (currTime == p.arrivalTime) {
                    readyQueue.push(&p);
                }
            }
        }

        vector<Process*> tempProcesses;

        // Increment IO time for all processes in I_O and move them back to readyQueue if done
        while (!I_O.empty()) {
            Process* process = I_O.top();
            I_O.pop();
            process->IO_time += 1;

            if (process->IO_time >= process->comesBackAfter) {
                // If the process is done with I/O, reset IO_time and push to readyQueue
                process->IO_time = 0;
                readyQueue.push(process);
            } else {
                // If not done with I/O, add to temporary vector to push back later
                tempProcesses.push_back(process);
            }
        }

        // Push all processes that are not done with I/O back into the I_O queue
        for (Process* process : tempProcesses) {
            I_O.push(process);
        }

        tempProcesses.clear(); // C

        if (CPU_process && !readyQueue.empty()) {
            CPU_process->totalProcessWaiting += CPU_process->waiting;
            CPU_process->waiting = 0;
            if (CPU_process->remainingTime == 0 || currTime == 200) {
                CPU_process->isTurnedAround = true;
                CPU_process->completionTime = currTime;
                CPU_process->remainingTime = CPU_process->burstTime;
                I_O.push(CPU_process);
                CPU_process = readyQueue.top();
                if (currTime!=200)
                    cout << " | (" << currTime << ") P" << CPU_process->pid << " | ";
                readyQueue.pop();
            }
            if (CPU_process->remainingTime > readyQueue.top()->remainingTime) {
                CPU_process->isTurnedAround = true;
                CPU_process->completionTime = currTime ;
                readyQueue.push(CPU_process);
                CPU_process = readyQueue.top();
                cout << " | (" << currTime << ") P" << CPU_process->pid << " | ";
                readyQueue.pop();
            }
        }

        if (!CPU_process && !readyQueue.empty()){
            CPU_process = readyQueue.top();
            cout << "(" << currTime << ") P" << CPU_process->pid << " | ";
            readyQueue.pop();
        }
    }
    cout << " | (" << TIME_LIMIT << ")" << endl;

    double totalTurnAround = 0;
    double totalWaiting = 0;
    int numProcesses = 0;
    for (Process &p : processes) {
        if (!p.isTurnedAround) {
            p.turnaroundTime = 0;
            p.waitingTime = 0;
        }
        else {
            numProcesses++;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.totalProcessWaiting;
        }

        totalTurnAround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
    }

    double avgTurnAroundTime = totalTurnAround / numProcesses;
    double avgWaitingTime = totalWaiting / numProcesses;

    cout << endl << "Process-wise Waiting Time and Turnaround Time:" << endl;
    for (const Process &p : processes) {
        cout << "Process " << p.pid
             << ": Waiting Time = " << p.waitingTime
             << ", Turnaround Time = " << p.turnaroundTime << endl;
    }

    cout << endl << "Avg Turnaround Time = " << avgTurnAroundTime << endl;
    cout << "Avg Waiting Time = " << avgWaitingTime << endl;

}

void RR (vector<Process>& processes) {
    queue<Process*> readyQueue;
    priority_queue<Process*, vector<Process*>, decltype(compareProcesses)> I_O(compareProcesses);
    Process* CPU_process = nullptr;
    int timeSlice = 0;

    cout << "\nGantt Chart:\n";

    for (int currTime = 0; currTime <= TIME_LIMIT; currTime++) {
        queue<Process*> tempQueue = readyQueue;

        while (!tempQueue.empty()) {
            Process* process = tempQueue.front();
            tempQueue.pop();

            process->waiting++;
        }

        if (CPU_process) {
            cout << "=";
            CPU_process->remainingTime--;
            CPU_process->executionTime++;
        }

        if (currTime < 10) {
            // Check for new arrivals and add them to the ready queue
            for (Process &p: processes) {
                if (currTime == p.arrivalTime) {
                    readyQueue.push(&p);
                }
            }
        }

        vector<Process*> ioFinishedProcesses;
        vector<Process*> ioNotFinishedProcesses;

        // Check I/O completion and sort processes by pid if they finished I/O at the same time
        while (!I_O.empty()) {
            Process* process = I_O.top();
            I_O.pop();
            process->IO_time += 1;

            if (process->IO_time >= process->comesBackAfter) {
                ioFinishedProcesses.push_back(process);
            } else {
                ioNotFinishedProcesses.push_back(process);
            }
        }

        // Sort the finished I/O processes by their pid
        sort(ioFinishedProcesses.begin(), ioFinishedProcesses.end(), [](const Process* a, const Process* b) {
            return a->pid < b->pid;
        });

        // Add the sorted processes to the ready queue
        for (Process* process : ioFinishedProcesses) {
            process->IO_time = 0; // Reset IO time
            readyQueue.push(process);
        }

        // Push the processes that are not done with I/O back into the I/O queue
        for (Process* process : ioNotFinishedProcesses) {
            I_O.push(process);
        }

        // Clear the vectors for the next iteration
        ioFinishedProcesses.clear();
        ioNotFinishedProcesses.clear();

        if (CPU_process && !readyQueue.empty()) {
            CPU_process->totalProcessWaiting += CPU_process->waiting;
            CPU_process->waiting = 0;
            if (CPU_process->remainingTime == 0 || currTime == 200) {
                CPU_process->remainingTime = CPU_process->burstTime;
                CPU_process->completionTime = currTime;
                CPU_process->executionTime = 0;
                I_O.push(CPU_process);
                CPU_process = readyQueue.front();
                if (currTime!=200)
                    cout << " | (" << currTime << ") P" << CPU_process->pid << " | ";
                readyQueue.pop();
            }
            if (CPU_process->executionTime == Q) {
                CPU_process->completionTime = currTime;
                CPU_process->executionTime = 0;
                readyQueue.push(CPU_process);
                CPU_process = readyQueue.front();
                cout << " | (" << currTime << ") P" << CPU_process->pid << " | ";
                readyQueue.pop();
            }
        }

        if (!CPU_process && !readyQueue.empty()){
            CPU_process = readyQueue.front();
            CPU_process->completionTime = currTime;
            cout << "(" << currTime << ") P" << CPU_process->pid << " | ";
            readyQueue.pop();
        }
    }
    cout << " | (" << TIME_LIMIT << ")" << endl;

    double totalTurnAround = 0;
    double totalWaiting = 0;
    int numProcesses = 0;
    for (Process &p : processes) {
        numProcesses++;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.totalProcessWaiting;

        totalTurnAround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
    }

    double avgTurnAroundTime = totalTurnAround / numProcesses;
    double avgWaitingTime = totalWaiting / numProcesses;

    cout << endl << "Process-wise Waiting Time and Turnaround Time:" << endl;
    for (const Process &p : processes) {
        cout << "Process " << p.pid
             << ": Waiting Time = " << p.waitingTime
             << ", Turnaround Time = " << p.turnaroundTime << endl;
    }

    cout << endl << "Avg Turnaround Time = " << avgTurnAroundTime << endl;
    cout << "Avg Waiting Time = " << avgWaitingTime << endl;
}

bool compareProcess(const Process* a, const Process* b) {
    if (a->tempPrio == b->tempPrio) {
        return a->readyArrival < b->readyArrival; // Sort by arrival time if priorities are equal
    }
    return a->tempPrio < b->tempPrio; // Sort by priority
}

queue<Process*> sortReadyQueue(queue<Process*>& readyQueue) {
    vector<Process*> sortedProcesses;

    // Transfer elements from queue to vector
    while (!readyQueue.empty()) {
        sortedProcesses.push_back(readyQueue.front());
        readyQueue.pop();
    }

    // Sort the vector
    sort(sortedProcesses.begin(), sortedProcesses.end(), compareProcess);

    // Transfer back to queue
    for (Process* process : sortedProcesses) {
        readyQueue.push(process);
    }

    return readyQueue;
}

void preemptivePriority(vector<Process>& processes) {
    queue<Process*> readyQueue;
    Process* CPU_process = nullptr;
    priority_queue<Process*, vector<Process*>, decltype(compareProcesses)> I_O(compareProcesses);

    cout << "\nGantt Chart:\n";

    for (int currTime = 0; currTime <= TIME_LIMIT; ++currTime) {
        queue<Process*> tempQueue1 = readyQueue;

        while (!tempQueue1.empty()) {
            Process* process = tempQueue1.front();
            tempQueue1.pop();

            process->waiting++;
        }

        if (CPU_process) {
            cout << "=";
            CPU_process->remainingTime--;
        }

        if (currTime < 10) {
            // Check for new arrivals and add them to the ready queue
            for (Process &p: processes) {
                if (currTime == p.arrivalTime) {
                    p.readyArrival = currTime;
                    readyQueue.push(&p);
                }
            }
        }

        // Make a copy of the readyQueue
        queue<Process*> tempQueue = readyQueue;

        // Traverse the copy without modifying the original queue
        while (!tempQueue.empty()) {
            Process* process = tempQueue.front();
            tempQueue.pop();

            // Process the copy as needed
            process->priorityCounter++;
            if (process->priorityCounter % 5 == 0) {
                if (process->tempPrio > 0)
                    process->tempPrio--;
            }
        }

        vector<Process*> ioFinishedProcesses;
        vector<Process*> ioNotFinishedProcesses;

        // Check I/O completion and sort processes by pid if they finished I/O at the same time
        while (!I_O.empty()) {
            Process* process = I_O.top();
            I_O.pop();
            process->IO_time += 1;

            if (process->IO_time >= process->comesBackAfter) {
                ioFinishedProcesses.push_back(process);
            } else {
                ioNotFinishedProcesses.push_back(process);
            }
        }

        // Sort the finished I/O processes by their pid
        sort(ioFinishedProcesses.begin(), ioFinishedProcesses.end(), [](const Process* a, const Process* b) {
            return a->pid < b->pid;
        });

        // Add the sorted processes to the ready queue
        for (Process* process : ioFinishedProcesses) {
            process->IO_time = 0; // Reset IO time
            process->readyArrival = currTime;
            readyQueue.push(process);
        }

        // Push the processes that are not done with I/O back into the I/O queue
        for (Process* process : ioNotFinishedProcesses) {
            I_O.push(process);
        }

        // Clear the vectors for the next iteration
        ioFinishedProcesses.clear();
        ioNotFinishedProcesses.clear();

        if (CPU_process && !readyQueue.empty()) {
            CPU_process->totalProcessWaiting += CPU_process->waiting;
            CPU_process->waiting = 0;
            if (CPU_process->remainingTime == 0 || currTime == 200) {
                CPU_process->completionTime = currTime;
                CPU_process->remainingTime = CPU_process->burstTime;
                CPU_process->tempPrio = CPU_process->priority;
                I_O.push(CPU_process);
                readyQueue = sortReadyQueue(readyQueue);
                CPU_process = readyQueue.front();
                if (currTime!=200)
                    cout << " | (" << currTime << ") P" << CPU_process->pid << " | ";
                readyQueue.pop();
            }

            readyQueue = sortReadyQueue(readyQueue);
            if (CPU_process->tempPrio > readyQueue.front()->tempPrio) {
                CPU_process->completionTime = currTime;
                Process* p = CPU_process;
                CPU_process = readyQueue.front();
                readyQueue.pop();
                p->tempPrio = p->priority;
                p->readyArrival = currTime;
                readyQueue.push(p);
                cout << " | (" << currTime << ") P" << CPU_process->pid << " | ";
            }

            if (CPU_process->priority == readyQueue.front()->priority) {
                // Do nothing
            }

        }

        if (!CPU_process && !readyQueue.empty()){
            CPU_process = readyQueue.front();
            CPU_process->completionTime = currTime;
            cout << "(" << currTime << ") P" << CPU_process->pid << " | ";
            readyQueue.pop();
        }
    }

    cout << " | (" << TIME_LIMIT << ")" << endl;

    double totalTurnAround = 0;
    double totalWaiting = 0;
    int numProcesses = 0;
    for (Process &p : processes) {
        numProcesses++;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.totalProcessWaiting;
        totalTurnAround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
    }

    double avgTurnAroundTime = totalTurnAround / numProcesses;
    double avgWaitingTime = totalWaiting / numProcesses;

    cout << endl << "Process-wise Waiting Time and Turnaround Time:" << endl;
    for (const Process &p : processes) {
        cout << "Process " << p.pid
             << ": Waiting Time = " << p.waitingTime
             << ", Turnaround Time = " << p.turnaroundTime << endl;
    }

    cout << endl << "Avg Turnaround Time = " << avgTurnAroundTime << endl;
    cout << "Avg Waiting Time = " << avgWaitingTime << endl;
}

void nonPreemptivePriority(vector<Process>& processes) {
    queue<Process*> readyQueue;
    Process* CPU_process = nullptr;
    priority_queue<Process*, vector<Process*>, decltype(compareProcesses)> I_O(compareProcesses);

    cout << "\nGantt Chart:\n";

    for (int currTime = 0; currTime <= TIME_LIMIT; ++currTime) {
        queue<Process*> tempQueue1 = readyQueue;

        while (!tempQueue1.empty()) {
            Process* process = tempQueue1.front();
            tempQueue1.pop();

            process->waiting++;
        }

        if (CPU_process) {
            cout << "=";
            CPU_process->remainingTime--;
        }

        if (currTime < 10) {
            // Check for new arrivals and add them to the ready queue
            for (Process &p: processes) {
                if (currTime == p.arrivalTime) {
                    p.readyArrival = currTime;
                    readyQueue.push(&p);
                }
            }
        }

        // Make a copy of the readyQueue
        queue<Process*> tempQueue = readyQueue;

        // Traverse the copy without modifying the original queue
        while (!tempQueue.empty()) {
            Process* process = tempQueue.front();
            tempQueue.pop();

            // Process the copy as needed
            process->priorityCounter++;
            if (process->priorityCounter % 5 == 0) {
                if (process->tempPrio > 0)
                    process->tempPrio--;
            }
        }

        vector<Process*> ioFinishedProcesses;
        vector<Process*> ioNotFinishedProcesses;

        // Check I/O completion and sort processes by pid if they finished I/O at the same time
        while (!I_O.empty()) {
            Process* process = I_O.top();
            I_O.pop();
            process->IO_time += 1;

            if (process->IO_time >= process->comesBackAfter) {
                ioFinishedProcesses.push_back(process);
            } else {
                ioNotFinishedProcesses.push_back(process);
            }
        }

        // Sort the finished I/O processes by their pid
        sort(ioFinishedProcesses.begin(), ioFinishedProcesses.end(), [](const Process* a, const Process* b) {
            return a->pid < b->pid;
        });

        // Add the sorted processes to the ready queue
        for (Process* process : ioFinishedProcesses) {
            process->IO_time = 0; // Reset IO time
            process->readyArrival = currTime;
            readyQueue.push(process);
        }

        // Push the processes that are not done with I/O back into the I/O queue
        for (Process* process : ioNotFinishedProcesses) {
            I_O.push(process);
        }

        // Clear the vectors for the next iteration
        ioFinishedProcesses.clear();
        ioNotFinishedProcesses.clear();

        if (CPU_process && !readyQueue.empty()) {
            CPU_process->totalProcessWaiting += CPU_process->waiting;
            CPU_process->waiting = 0;
            if (CPU_process->remainingTime == 0 || currTime == 200) {
                CPU_process->completionTime = currTime;
                CPU_process->remainingTime = CPU_process->burstTime;
                CPU_process->tempPrio = CPU_process->priority;
                I_O.push(CPU_process);
                CPU_process = readyQueue.front();
                if (currTime!=200)
                    cout << " | (" << currTime << ") P" << CPU_process->pid << " | ";
                readyQueue.pop();
            }
            readyQueue = sortReadyQueue(readyQueue);
        }

        if (!CPU_process && !readyQueue.empty()){
            CPU_process = readyQueue.front();
            CPU_process->completionTime = currTime;
            cout << "(" << currTime << ") P" << CPU_process->pid << " | ";
            readyQueue.pop();
        }
    }

    cout << " | (" << TIME_LIMIT << ")" << endl;

    double totalTurnAround = 0;
    double totalWaiting = 0;
    int numProcesses = 0;
    for (Process &p : processes) {
        numProcesses++;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.totalProcessWaiting;
        totalTurnAround += p.turnaroundTime;
        totalWaiting += p.waitingTime;
    }

    double avgTurnAroundTime = totalTurnAround / numProcesses;
    double avgWaitingTime = totalWaiting / numProcesses;

    cout << endl << "Process-wise Waiting Time and Turnaround Time:" << endl;
    for (const Process &p : processes) {
        cout << "Process " << p.pid
             << ": Waiting Time = " << p.waitingTime
             << ", Turnaround Time = " << p.turnaroundTime << endl;
    }

    cout << endl << "Avg Turnaround Time = " << avgTurnAroundTime << endl;
    cout << "Avg Waiting Time = " << avgWaitingTime << endl;
}

int main() {

    cout << "Welcome to my Process Scheduling Program...\n\n";

    int choice;

    do {
        cout << endl << "1. FCFS Scheduling." << endl;
        cout << "2. SJF Scheduling." << endl;
        cout << "3. SRTF Scheduling." << endl;
        cout << "4. Round Robin Scheduling." << endl;
        cout << "5. Preemptive Priority Scheduling." << endl;
        cout << "6. Non-preemptive Priorit Scheduling." << endl;
        cout <<"7. Exit." << endl;
        cout << endl << "Enter your choice: ";

        cin >> choice;

        switch (choice) {
            case 1: {
                  vector<Process> processes = createProcesses();
                  FCFS(processes);
                break;
            }
            case 2: {
                vector<Process> processes = createProcesses();
                SJF(processes);
                break;
            }
           case 3: {
               vector<Process> processes = createProcesses();
               SRTF(processes);
               break;
           }
            case 4: {
                vector<Process> processes = createProcesses();
                RR(processes);
                break;
            }
            case 5: {
                vector<Process> processes = createProcesses();
                preemptivePriority(processes);
                break;
            }
            case 6: {
                vector<Process> processes = createProcesses();
                nonPreemptivePriority(processes);
                break;
            }
            case 7: {
                cout << endl << "Exiting Program..." << endl << endl;
                break;
            }
            default:
                cout << endl << "Invalid Choice!" << endl;
        }
    } while (choice != 7);
    return 0;
}
