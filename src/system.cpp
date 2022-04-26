#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  processes_.clear();
  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    std::string user(LinuxParser::User(pid));
    std::string command(LinuxParser::Command(pid));
    long int upTime(LinuxParser::UpTime(pid));
    std::string ram(LinuxParser::Ram(pid));
    float cpuUtilization(LinuxParser::CpuUtilization(pid));
    Process p(pid, user, command, upTime, ram, cpuUtilization);
    processes_.emplace_back(p);
  }
  std::sort(processes_.begin(), processes_.end(), std::greater<>());
  return processes_;
}

std::string System::Kernel() { return this->kernel; }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return this->operatingSystem; }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }

System::System() {
  this->operatingSystem = LinuxParser::OperatingSystem();
  this->kernel = LinuxParser::Kernel();
}