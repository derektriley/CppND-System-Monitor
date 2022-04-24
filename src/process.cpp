#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid; }

float Process::CpuUtilization() { return cpuUtilization; }

string Process::Command() { return command; }

string Process::Ram() { return ram; }

string Process::User() { return user; }

long int Process::UpTime() { return upTime; }

bool Process::operator<(Process const& a [[maybe_unused]]) const {
  if (cpuUtilization < a.cpuUtilization) {
    return true;
  }
  return false;
}

bool Process::operator>(Process const& a [[maybe_unused]]) const {
  if (cpuUtilization > a.cpuUtilization) {
    return true;
  }
  return false;
}