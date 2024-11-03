#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
	// Setting the process's start time
	string line {}, value {};
  	std::ifstream fs {LinuxParser::kProcDirectory + std::to_string(pid_) + LinuxParser::kStatFilename};
    if(fs.is_open()) {
      	while(getline(fs, line)) {
          	std::istringstream iss {line};
          	// Start time is located in 22d line
          	for(size_t i{}; i<22; ++i) {
            	iss >> value;
              	if(i == 21)
                	startTime = std::stol(value);
			}
		}
	}
}

// Return this process's ID
int Process::Pid() {
	return pid_;
}

// Return this process's CPU utilization
float Process::CpuUtilization() const {
	long curProcessCpuUtilization = LinuxParser::ActiveJiffies(pid_);
	long processUpTime = LinuxParser::UpTime() - startTime / sysconf(_SC_CLK_TCK);
	float processCpuUtilization = (float)(curProcessCpuUtilization / sysconf(_SC_CLK_TCK)) / (processUpTime + 1e-7);
	return processCpuUtilization;
}

// Return the command that generated this process
string Process::Command() {
	return LinuxParser::Command(pid_);
}

// Return this process's memory utilization
string Process::Ram() {
	return LinuxParser::Ram(pid_);
}

// Return the user (name) that generated this process
string Process::User() {
	return LinuxParser::User(pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
	return LinuxParser::UpTime(pid_);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator>(Process const& a) const {
	return CpuUtilization() > a.CpuUtilization();
}