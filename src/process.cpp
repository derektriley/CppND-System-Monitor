#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid; }

float Process::CpuUtilization() { return cpuUtilization; }

string Process::Command() { return command; }

string Process::Ram() { return ram; }

string Process::User() { return user; }

long int Process::UpTime() { return LinuxParser::UpTime() - upTime; }

bool Process::operator<(Process const& a) const {
  return cpuUtilization < a.cpuUtilization;
}

bool Process::operator>(Process const& a) const {
  return cpuUtilization > a.cpuUtilization;
}