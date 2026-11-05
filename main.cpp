#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
#include <limits>
using namespace std;

// CPU Scheduling Algorithm Simulator
// Priority convention: smaller number means higher priority.
struct Process {
    int id, arrival, burst, priority;
    int start = -1, completion = 0, waiting = 0, turnaround = 0, response = 0;
    int remaining = 0;
};
struct Segment { int pid, start, end; }; // pid 0 denotes CPU idle

void printGantt(const vector<Segment>& chart) {
    if (chart.empty()) return;
    cout << "\nGantt Chart:\n";
    for (const auto& s : chart) cout << "+--------";
    cout << "+\n";
    for (const auto& s : chart) {
        string label = s.pid == 0 ? "IDLE" : "P" + to_string(s.pid);
        cout << "|" << setw(8) << left << label;
    }
    cout << "|\n";
    for (const auto& s : chart) cout << "+--------";
    cout << "+\n" << chart.front().start;
    for (const auto& s : chart) cout << setw(9) << right << s.end;
    cout << "\n";
}
void finishMetrics(vector<Process>& p) {
    for (auto& x : p) { x.turnaround=x.completion-x.arrival; x.waiting=x.turnaround-x.burst; }
}
void printResults(const string& name, const vector<Process>& p, const vector<Segment>& chart) {
    double wt=0,tat=0,rt=0;
    cout << "\n========== " << name << " RESULTS ==========\n";
    cout << left << setw(10)<<"Process"<<setw(8)<<"AT"<<setw(8)<<"BT"<<setw(10)<<"Priority"<<setw(8)<<"CT"<<setw(8)<<"TAT"<<setw(8)<<"WT"<<setw(8)<<"RT"<<"\n";
    for(const auto& x:p){
        cout<<left<<setw(10)<<("P"+to_string(x.id))<<setw(8)<<x.arrival<<setw(8)<<x.burst<<setw(10)<<x.priority<<setw(8)<<x.completion<<setw(8)<<x.turnaround<<setw(8)<<x.waiting<<setw(8)<<x.response<<"\n";
        wt+=x.waiting; tat+=x.turnaround; rt+=x.response;
    }
    cout<<fixed<<setprecision(2)<<"\nAverage waiting time    : "<<wt/p.size()
        <<"\nAverage turnaround time : "<<tat/p.size()
        <<"\nAverage response time   : "<<rt/p.size()<<"\n";
    printGantt(chart);
}
vector<Process> readProcesses(){
    int n; cout<<"Enter number of processes (1-20): ";
    while(!(cin>>n)||n<1||n>20){cout<<"Enter a whole number from 1 to 20: ";cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
    vector<Process> p(n);
    for(int i=0;i<n;i++){
        p[i].id=i+1; cout<<"\nProcess P"<<p[i].id<<"\nArrival time: ";
        while(!(cin>>p[i].arrival)||p[i].arrival<0){cout<<"Enter a non-negative integer: ";cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
        cout<<"Burst time (> 0): ";
        while(!(cin>>p[i].burst)||p[i].burst<=0){cout<<"Enter a positive integer: ";cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
        cout<<"Priority (smaller number = higher priority): ";
        while(!(cin>>p[i].priority)||p[i].priority<0){cout<<"Enter a non-negative integer: ";cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
        p[i].remaining=p[i].burst;
    }
    return p;
}
void fcfs(vector<Process> p){
    stable_sort(p.begin(),p.end(),[](const Process&a,const Process&b){return a.arrival<b.arrival;});
    vector<Segment> chart; int time=0;
    for(auto&x:p){if(time<x.arrival){chart.push_back({0,time,x.arrival});time=x.arrival;}
        x.start=time;x.response=time-x.arrival;chart.push_back({x.id,time,time+x.burst});time+=x.burst;x.completion=time;}
    finishMetrics(p);printResults("FIRST COME FIRST SERVED (FCFS)",p,chart);
}
void sjf(vector<Process> p){
    vector<Segment> chart;int time=0,done=0,n=p.size();
    while(done<n){int idx=-1;for(int i=0;i<n;i++)if(!p[i].completion&&p[i].arrival<=time&&(idx==-1||p[i].burst<p[idx].burst||(p[i].burst==p[idx].burst&&p[i].arrival<p[idx].arrival)))idx=i;
        if(idx==-1){int next=numeric_limits<int>::max();for(auto&x:p)if(!x.completion)next=min(next,x.arrival);chart.push_back({0,time,next});time=next;continue;}
        p[idx].start=time;p[idx].response=time-p[idx].arrival;chart.push_back({p[idx].id,time,time+p[idx].burst});time+=p[idx].burst;p[idx].completion=time;done++;}
    finishMetrics(p);printResults("SHORTEST JOB FIRST (NON-PREEMPTIVE)",p,chart);
}
void priorityScheduling(vector<Process> p){
    vector<Segment> chart;int time=0,done=0,n=p.size();
    while(done<n){int idx=-1;for(int i=0;i<n;i++)if(!p[i].completion&&p[i].arrival<=time&&(idx==-1||p[i].priority<p[idx].priority||(p[i].priority==p[idx].priority&&p[i].arrival<p[idx].arrival)))idx=i;
        if(idx==-1){int next=numeric_limits<int>::max();for(auto&x:p)if(!x.completion)next=min(next,x.arrival);chart.push_back({0,time,next});time=next;continue;}
        p[idx].start=time;p[idx].response=time-p[idx].arrival;chart.push_back({p[idx].id,time,time+p[idx].burst});time+=p[idx].burst;p[idx].completion=time;done++;}
    finishMetrics(p);printResults("PRIORITY (NON-PREEMPTIVE)",p,chart);
}
void roundRobin(vector<Process> p,int q){
    int n=p.size(),time=0,done=0;vector<Segment> chart;vector<bool> added(n,false);vector<int> ready;
    auto addArrivals=[&](){for(int i=0;i<n;i++)if(!added[i]&&p[i].arrival<=time){ready.push_back(i);added[i]=true;}};
    while(done<n){addArrivals();if(ready.empty()){int next=numeric_limits<int>::max();for(int i=0;i<n;i++)if(!added[i])next=min(next,p[i].arrival);chart.push_back({0,time,next});time=next;addArrivals();}
        int idx=ready.front();ready.erase(ready.begin());if(p[idx].start==-1){p[idx].start=time;p[idx].response=time-p[idx].arrival;}
        int run=min(q,p[idx].remaining);chart.push_back({p[idx].id,time,time+run});time+=run;p[idx].remaining-=run;addArrivals();
        if(p[idx].remaining>0)ready.push_back(idx);else{p[idx].completion=time;done++;}}
    finishMetrics(p);printResults("ROUND ROBIN (QUANTUM = "+to_string(q)+")",p,chart);
}

// Strategy hierarchy demonstrates inheritance and runtime polymorphism.
class SchedulingStrategy {
public:
    virtual void execute(const vector<Process>& processes, int quantum = 0) const = 0;
    virtual string name() const = 0;
    virtual ~SchedulingStrategy() = default;
};
class FCFSScheduler : public SchedulingStrategy {
public:
    void execute(const vector<Process>& p, int = 0) const override { fcfs(p); }
    string name() const override { return "FCFS"; }
};
class SJFScheduler : public SchedulingStrategy {
public:
    void execute(const vector<Process>& p, int = 0) const override { sjf(p); }
    string name() const override { return "SJF"; }
};
class PriorityScheduler : public SchedulingStrategy {
public:
    void execute(const vector<Process>& p, int = 0) const override { priorityScheduling(p); }
    string name() const override { return "Priority"; }
};
class RoundRobinScheduler : public SchedulingStrategy {
public:
    void execute(const vector<Process>& p, int q = 0) const override { roundRobin(p, q); }
    string name() const override { return "Round Robin"; }
};

int main(){
    cout<<"=============================================\n       CPU SCHEDULING ALGORITHM SIMULATOR\n=============================================\n";
    vector<Process> p=readProcesses();int choice;
    do{cout<<"\n1. FCFS\n2. SJF (Non-preemptive)\n3. Priority (Non-preemptive)\n4. Round Robin\n5. Run all algorithms\n0. Exit\nEnter choice: ";
        if(!(cin>>choice)){cout<<"Invalid input. Exiting safely.\n";break;}
        switch(choice){case 1: { FCFSScheduler scheduler; const SchedulingStrategy* strategy=&scheduler; strategy->execute(p); break; }
        case 2: { SJFScheduler scheduler; const SchedulingStrategy* strategy=&scheduler; strategy->execute(p); break; }
        case 3: { PriorityScheduler scheduler; const SchedulingStrategy* strategy=&scheduler; strategy->execute(p); break; }
        case 4:case 5:{
            if(choice==5){FCFSScheduler a;SJFScheduler b;PriorityScheduler c;
                const SchedulingStrategy* strategies[]={&a,&b,&c};
                for(const auto* strategy:strategies) strategy->execute(p);
            }
            int q; cout<<"Enter time quantum (> 0): ";
            while(!(cin>>q)||q<=0){cout<<"Enter a positive integer: ";cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
            RoundRobinScheduler scheduler; const SchedulingStrategy* strategy=&scheduler;
            strategy->execute(p,q);break;
        }
        case 0:cout<<"Thank you for using the simulator.\n";break;default:cout<<"Invalid menu choice. Try again.\n";}
    }while(choice!=0);return 0;
}
