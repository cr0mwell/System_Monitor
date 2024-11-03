#include <vector>
#include <string>

#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
	std::vector<std::string> cpu = LinuxParser::CpuUtilization();
	long totalCpuUtilization {}, activeCpuUtilization {};
	
  	for(size_t i{}; i<cpu.size(); i++) {
    	if(i != LinuxParser::CPUStates::kIdle_ && i != LinuxParser::CPUStates::kIOwait_)
			activeCpuUtilization += std::stol(cpu[i]);
		totalCpuUtilization += std::stol(cpu[i]);
	}

	// As the CPU utilization number of jiffies grows in time, the measurement will not show a real CPU usage
	// Measuring the utilization for the current time frame only therefore
  	float cpuUtilization = (float)(activeCpuUtilization - activeCpuUtil) / (totalCpuUtilization - totalCpuUtil);
	// Saving the current measurements for future calculations
	activeCpuUtil = activeCpuUtilization;
	totalCpuUtil = totalCpuUtilization;
	
	return cpuUtilization;
}