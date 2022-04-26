#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;
  bool operator>(Process const& a) const;

  Process(int pid, std::string user, std::string command, long int upTime,
          std::string ram, float cpuUtilization)
      : pid(pid),
        user(user),
        command(command),
        upTime(upTime),
        ram(ram),
        cpuUtilization(cpuUtilization) {}

 private:
  int pid;
  std::string user;
  std::string command;
  long int upTime;
  std::string ram;
  float cpuUtilization;
};

#endif