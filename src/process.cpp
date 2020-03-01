#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <time.h> 
#include <sys/times.h> 
#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
 	_pid = pid; 
}

// TODO: Return this process's ID
int Process::Pid() { return _pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  
    long uptime = LinuxParser::UpTime(_pid);
    long Hertz = sysconf(_SC_CLK_TCK);
    long seconds = uptime ;
    //printf("Hertz %l  Seconds %l\n", Hertz, seconds);
	long total_time = LinuxParser::ActiveJiffies(_pid);   
    long cpu_usage = 0;
  
    if (seconds > 0) {
      cpu_usage = 100 * ((total_time/Hertz) / seconds);   
    }
  
  
  return cpu_usage; }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(_pid); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(_pid); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(_pid); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(_pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return true; }