#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "format.h"
#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;


#define printVariableNameAndValue(x) std::cout<< "The name of variable **" << (#x) << "** and the value of variable is => " << x << "\n";


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value = "";
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, dummy, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> dummy >> kernel;
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
  printf("Pids %ld", pids.size());  
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string token, token_num, end_token;
  int MemTotal;
  int MemFree;
  //int MemAvail;
  int MemUsed;

  //printf("MemoryUtilization()\n");
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> token >> token_num >> end_token;
    MemTotal = std::stoi(token_num);
    linestream >> token >> token_num >> end_token;
    MemFree = std::stoi(token_num);
    linestream >> token >> token_num >> end_token;
    //MemAvail = std::stoi(token_num);
  }

  MemUsed = MemTotal - MemFree;

  return 1.0 * MemUsed / MemTotal;
}

// TODO: Read and return the system uptime (in sec)
float LinuxParser::UpTime() {
  string line;
  string token, end_token;
  
  long uptime = 0;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> token >> end_token;
    uptime = stof(token);
  }
  //printf("debug uptime %s\n", token.c_str());
  

  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line;
  string cpu, user, nice, system = 0;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system;
  }
//  printf("sys jiffies %s\n", system);
  return std::stol(system);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string token;
  vector<string> r;
  int token_num = 1;

  long utime;      // #14 token (tokens count from 1)
  long stime;      // #15 token
  long cutime;     // #16 children in user mode
  long cstime;     // #17 children in kernel mode
  //long starttime;  // #22 time when process started in ticks
  long total_time = 0;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
  

    while (linestream >> token) {
 //     printf("active jiffies %d\n", token_num);
      switch (token_num) {
        case 14:
          utime = stol(token);
          break;
        case 15:
          stime = stol(token);
          break;
        case 16:
          cutime = stol(token);
          break;
        case 17:
          cstime = stol(token);
          break;
        //case 22:
        //  starttime = stol(token);
        //  break;
        default:
          break;
      }

      token_num++;
      
      if (token_num > 22) {
//        printf("active jiffies see last token %d\n", token_num);    
        break;
      }
    }

//    printf("active jiffies out of loop %d\n", token_num);    
    total_time = utime + stime + cutime + cstime;
  }
  return total_time;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> util_stats = LinuxParser::CpuUtilization();

//  printf("active jiffies 2 %s\n", util_stats[2]);
  
  if (util_stats.size() > 2)
  	return std::stol(util_stats[2]);
  else 
    return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> util_stats = LinuxParser::CpuUtilization();
//  printf("idle jiffies %s\n", util_stats[3]);
  
  if (util_stats.size() > 3)
  	return std::stol(util_stats[3]);
  else
    return 0;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string token;
  vector<string> r;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> token;  // throw away first token cpu

    while (linestream >> token) {
      r.push_back(token);
    }
  }

  return r;
}

// TODO: Read and return the total number of processes
// Under meminfo
// "procs running 15"
int LinuxParser::TotalProcesses() {
  string key, value, line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
        if (key == "processes") {        
		  break;

        }
      
    }
  }
  
  return stoi(value);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value, line;

 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
        if (key == "procs_running") {
          //return stoi(value);
          break;
        }
      
    }

  }
  
  return stoi(value);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;

  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }

  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key, value, line;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return value;
        }
      }
    }
  }
  
  return "1000";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string name, x, u2, u3, name2, dir, dir2;

  std::ifstream stream(kPasswordPath);

  name = "BEEF";
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');     
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> u2 >> u3 >> name2 >> dir >> dir2 ;
      if (u2 == uid) {

          break;
      }
    } // while
  } // if

  return name;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key, value, line;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
          //return value;
          break;
      }
      
    }
  }

  return value;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string token;
  long sec = 0;
  long sys_uptime = 0;
  
  sys_uptime = LinuxParser::UpTime();
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> token;
      //printf("uptime stat token %d", i);
    }
  	sec = sys_uptime - std::stol(token.c_str()) / sysconf(_SC_CLK_TCK);    
  }

  

  //printf("Uptime         pid %s\n", token.c_str());
  return sec;
}

float LinuxParser::getCpuPercent(int pid)
{
    std::string path =  kProcDirectory + to_string(pid) + kStatFilename;
    std::ifstream stream(path);
    
    string line;
    std::getline(stream, line); // file contains only one line
    std::istringstream buffer(line);
    std::istream_iterator<string> beginning(buffer), end;
    std::vector<string> line_content(beginning, end);
    //float utime = LinuxParser::UpTime(pid);
    float utime = stof(line_content[13]);
    float stime = stof(line_content[14]);
    float cutime = stof(line_content[15]);
    float cstime = stof(line_content[16]);
    float starttime = stof(line_content[21]);
    float uptime = LinuxParser::UpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime / freq);
    float result = 1.0 * ((total_time / freq) / seconds);
  
    //if (pid == 1) {
    //printVariableNameAndValue(utime)
    //printVariableNameAndValue(total_time) 
    //printVariableNameAndValue(uptime)
    //}
    return (result);
}