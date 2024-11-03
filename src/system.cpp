#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// System's CPU
Processor& System::Cpu() {
	return cpu_;
}

// System's processes
vector<Process>& System::Processes() {
  	vector<int> processes = LinuxParser::Pids();
  	processes_.erase(processes_.begin(), processes_.end());
  	for(auto pid: processes) {
      	Process process = Process(pid);
    	processes_.push_back(process);
    }
  	
  	// Sorting the processes by the CPU utilization time
  	std::sort(processes_.begin(), processes_.end(),
	  		  [](const Process &a, const Process &b){return a > b;});
  	
  	return processes_;
}

// The system's kernel identifier (string)
std::string System::Kernel() {
	return LinuxParser::Kernel();
}

// The system's memory utilization
float System::MemoryUtilization() {
	return LinuxParser::MemoryUtilization();
}

// The operating system name
std::string System::OperatingSystem() {
	return LinuxParser::OperatingSystem();
}

// The number of processes actively running on the system
int System::RunningProcesses() {
	return LinuxParser::RunningProcesses();
}

// The total number of processes on the system
int System::TotalProcesses() {
	return LinuxParser::TotalProcesses();
}

// Number of seconds since the system started running
long int System::UpTime() {
	return LinuxParser::UpTime();
}