#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
      std::istringstream linestream(line);
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

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

float LinuxParser::MemoryUtilization() { 
  std::filesystem::path path(LinuxParser::kProcDirectory + LinuxParser::kMeminfoFilename);
  std::ifstream stream(path);
  std::string memTotalLine, memFreeLine, key, value;
  int memTotal, memFree;
  if (stream.is_open()) {
    std::getline(stream, memTotalLine);
    std::getline(stream, memFreeLine);
    std::istringstream lineStreamMemTotal(memTotalLine);
    std::istringstream lineStreamMemFree(memFreeLine);
    lineStreamMemTotal >> key >> value;
    std::istringstream(value) >> memTotal;
    lineStreamMemFree >> key >> value;
    std::istringstream(value) >> memFree;
  }
  return ((memTotal - memFree) / (float)memTotal);
}

long int LinuxParser::UpTime() { 
  std::filesystem::path path(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
  std::ifstream stream(path);
  std::string line;
  long int uptime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::replace(line.begin(), line.end(), '.', ' ');
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

float LinuxParser::CpuUtilization(int pid) {
  long uptime = LinuxParser::UpTime();
  long Hertz = sysconf(_SC_CLK_TCK);

  std::filesystem::path path(LinuxParser::kProcDirectory + to_string(pid) +
                             LinuxParser::kStatFilename);
  std::ifstream stream(path);
  std::string line, value;
  long utime, stime, cutime, cstime, starttime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::stringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      value.clear();
      linestream >> value;
      if (i == 13) { // utime
        utime = std::stol(value);
      } else if (i == 14) { //stime
        stime = std::stol(value);
      } else if (i == 15) { //cutime
        cutime = std::stol(value);
      } else if (i == 16) { //cstime
        cstime = std::stol(value);
      } else if (i == 21) { //starttime
        starttime = std::stol(value);
      }
    }
  }
  
  long total_time = utime + stime;
  total_time = total_time + cutime + cstime;
  float seconds = (float) uptime - (starttime / (float)Hertz);
  return 100.0 * (total_time / Hertz) / (float) seconds;
}

float LinuxParser::CpuUtilization() { 
  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::filesystem::path path(LinuxParser::kProcDirectory +
                             LinuxParser::kStatFilename);
  std::ifstream stream(path);
  std::string line, cpu;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  }

  int idleTime = idle + iowait;
  int nonIdleTime = user + nice + system + irq + softirq + steal;
  int totalTime = idleTime + nonIdleTime;

  return (totalTime - idleTime) / (float) totalTime;
}

int LinuxParser::TotalProcesses() {
  std::filesystem::path path(LinuxParser::kProcDirectory +
                             LinuxParser::kStatFilename);
  std::ifstream stream(path);
  std::string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        int i;
        std::istringstream(value) >> i;
        return i;
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  std::filesystem::path path(LinuxParser::kProcDirectory +
                             LinuxParser::kStatFilename);
  std::ifstream stream(path);
  std::string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        int i;
        std::istringstream(value) >> i;
        return i;
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) { 
  std::filesystem::path path(LinuxParser::kProcDirectory + to_string(pid) +
                             LinuxParser::kCmdlineFilename);
  std::ifstream stream(path);
  std::string line;
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) {
  std::filesystem::path path(LinuxParser::kProcDirectory + to_string(pid) +
                             LinuxParser::kStatusFilename);
  std::ifstream stream(path);
  std::string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        int vmSizeMb = std::stoi(value) / 1000;
        return std::to_string(vmSizeMb);
      }
    }
  }
  return string();
}


string LinuxParser::Uid(int pid) {
  std::filesystem::path path(LinuxParser::kProcDirectory + to_string(pid) +
                             LinuxParser::kStatusFilename);
  std::ifstream stream(path);
  std::string line, key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }
  return line;
}

string LinuxParser::User(int pid) { 
  std::filesystem::path path(LinuxParser::kPasswordPath);
  std::ifstream stream(path);
  std::string line;
  std::string user, passwd, uid;

  string uidString = LinuxParser::Uid(pid);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> passwd >> uid;
      if (uid.compare(uidString) == 0) {
          return user;
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) { 
  std::filesystem::path path(LinuxParser::kProcDirectory + to_string(pid) +
                             LinuxParser::kStatFilename);
  std::ifstream stream(path);
  std::string line, value;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::stringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      value.clear();
      linestream >> value;
    }
  }
  return std::stol(value) / sysconf(_SC_CLK_TCK);
}
