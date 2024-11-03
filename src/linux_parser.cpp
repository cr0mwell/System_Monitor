#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::istringstream;
using std::ifstream;
using std::getline;

// OS name (f.e. Ubuntu 18.04.2 LTS)
string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::replace(line.begin(), line.end(), ' ', '_');
        std::replace(line.begin(), line.end(), '=', ' ');
        std::replace(line.begin(), line.end(), '"', ' ');
        istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == "PRETTY_NAME") {
            std::replace(value.begin(), value.end(), '_', ' ');
            return value;
          }
        }
      }
    }
    return value;
}

// Kernel version (f.e. 4.15.0-55-generic)
string LinuxParser::Kernel() {
    string os, version, kernel;
    string line;
    ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      istringstream linestream(line);
      linestream >> os >> version >> kernel;
    }
    return kernel;
}

// PIDs list extraction
vector<int> LinuxParser::Pids() {
    vector<int> pids;
    DIR* directory = opendir(kProcDirectory.c_str());
    struct dirent* file;
    while ((file = readdir(directory)) != nullptr) {
      // Is this a directory?
      if (file->d_type == DT_DIR) {
        // Is every character of the name a digit?
        string filename(file->d_name);
        if (std::all_of(filename.begin(), filename.end(), isdigit)) {
          int pid = stoi(filename);
          pids.push_back(pid);
        }
      }
    }
    closedir(directory);
    return pids;
}

// System memory utilization
float LinuxParser::MemoryUtilization() { 
	float total {}, free {};
  	vector<float *> memory {&total, &free};
  	string line {}, label {};
	ifstream fs(kProcDirectory + kMeminfoFilename);
    if(fs.is_open()) {
    	for(auto &m: memory)
        	if(getline(fs, line)) {
            	istringstream iss {line};
              	iss >> label >> *m;
        	}
	}
  	//std::cout << "Used memory: " << total - free << std::endl;
  	return (total - free) / total;
}

// System uptime
long LinuxParser::UpTime() {
	long uptime {};
  	string line {};
  	ifstream fs {kProcDirectory + kUptimeFilename};
    if(fs.is_open()) {
      	if(getline(fs, line)) {
        	istringstream iss {line};
          	iss >> uptime;
    	}
	}  	
  	//std::cout << "System uptime: " << uptime << std::endl;
  	return uptime;
}

// Number of jiffies for the system
long LinuxParser::Jiffies() {
  	// sysconf(_SC_CLK_TCK) gives the number of ticks(jiffies) per second
	return sysconf(_SC_CLK_TCK) * UpTime();
}

// Number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  	long jiffies {};
  	string line {}, value {};
  	ifstream fs {kProcDirectory + std::to_string(pid) + kStatFilename};
    if(fs.is_open()) {
      	while(getline(fs, line)) {
          	istringstream iss {line};
          	// Need to extract 14-17 tokens only
          	for(size_t i{}; i<17; ++i) {
            	iss >> value;
              	if(i > 12)
                	jiffies += std::stol(value);
            }
        }
	}
	return jiffies;
}
              
// CPU utilization
vector<string> LinuxParser::CpuUtilization() {
	vector<string> cpuUtil {};
    string line {}, value {}, name {};
    std::ifstream fs {kProcDirectory + kStatFilename};
    if(fs.is_open()) {
    	std::getline(fs, line);
		// Collecting all jiffies tokens
		// Format: <CPU_name> user nice system idle iowait irq softirq steal guest guest_nice
		istringstream iss {line};
		// Skipping the first token
		iss >> name;
		while(iss >> value)
			cpuUtil.push_back(value);
	}
	return cpuUtil;
}

// Number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
	long jiffies {};
    vector<string> cpu = CpuUtilization();
    // Tokens to be considered as active:
    // user nice system irq softirq steal guest guest_nice
    vector<long> activeIdx {CPUStates::kUser_,
  							CPUStates::kNice_,
  							CPUStates::kSystem_,
  							CPUStates::kIRQ_,
  							CPUStates::kSoftIRQ_,
  							CPUStates::kSteal_,
  							CPUStates::kGuest_,
  							CPUStates::kGuestNice_};
    // Summing up all the active tokens from the cpu
    for(const auto &idx: activeIdx)
    	jiffies += std::stol(cpu[idx]);
    return jiffies;
}

// Number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
	long jiffies {};
    vector<string> cpu = CpuUtilization();
    // Summing up 3rd and 4th tokens from the cpu
    vector<long> idleIdx {CPUStates::kIdle_,
  						  CPUStates::kIOwait_};
    for(const auto &idx: idleIdx)
    	jiffies += std::stol(cpu[idx]);
    return jiffies;
}

// Total number of processes
int LinuxParser::TotalProcesses() {
	int processes {};
  	string line {}, key{}, value {};
  	ifstream fs {kProcDirectory + kStatFilename};
    if(fs.is_open()) {
      	while(getline(fs, line)) {
          	istringstream iss {line};
          	iss >> key >> value;
          	if(key == "processes") {
              	processes= std::stol(value);
              	break;
            }
        }
	}
	return processes;
}

// Number of running processes
int LinuxParser::RunningProcesses() {
	int runningProcesses {};
  	string line {}, key{}, value {};
  	ifstream fs {kProcDirectory + kStatFilename};
    if(fs.is_open()) {
      	while(getline(fs, line)) {
          	istringstream iss {line};
          	iss >> key >> value;
          	if(key == "procs_running") {
              	runningProcesses = std::stol(value);
              	break;
            }
        }
	}
	return runningProcesses;
}

// Process CMD
string LinuxParser::Command(int pid) {
	string cmdLine {};
  	ifstream fs {kProcDirectory + std::to_string(pid) + kCmdlineFilename};
    if(fs.is_open())
      	getline(fs, cmdLine);
    return cmdLine;
}

// Process memory used
string LinuxParser::Ram(int pid) {
	string usedRAM {}, line {}, key{}, value {};
  	ifstream fs {kProcDirectory + std::to_string(pid) + kStatusFilename};
    if(fs.is_open()) {
      	while(getline(fs, line)) {
          	istringstream iss {line};
          	iss >> key >> value;
          	if(key == "VmSize:") {
              	usedRAM = std::to_string(std::stol(value) / 1024);	// In MB
              	break;
            }
        }
	}
	return usedRAM;
}

// Process UID
string LinuxParser::Uid(int pid) {
	string uid {}, line {}, key{}, value {};
  	ifstream fs {kProcDirectory + std::to_string(pid) + kStatusFilename};
    if(fs.is_open()) {
      	while(getline(fs, line)) {
          	istringstream iss {line};
          	iss >> key >> value;
          	if(key == "Uid:") {
              	uid = value;
              	break;
            }
        }
	}
	return uid;
}

// Process username
string LinuxParser::User(int pid) {
	string username {}, line {}, x {}, uid {};
    string searchedUid = Uid(pid);
    ifstream fs {kPasswordPath};
    if(fs.is_open()) {
      	while(getline(fs, line)) {
          	std::replace(line.begin(), line.end(), ':', ' ');
          	istringstream iss {line};
          	iss >> username >> x >> uid;
          	if(uid == searchedUid) {
              	break;
            }
        }
	}
	return username;
}

// Process uptime
long LinuxParser::UpTime(int pid) {
	long startTime {}, uptime {UpTime()};
    string line {}, value {};
    ifstream fs {kProcDirectory + std::to_string(pid) + kStatFilename};
    if(fs.is_open()) {
      	if(getline(fs, line)) {
          	istringstream iss {line};
          	for (size_t i{}; i<22; ++i) {
            	iss >> value;
              	if (i == 21)
                	startTime = std::stol(value);
            }
        }
	}
    return uptime - startTime / sysconf(_SC_CLK_TCK);
}
