
#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  
  vector<string> stats = LinuxParser::CpuUtilization();

  float user = stof(stats[0]);
  float nice = stof(stats[1]);
  float system = stof(stats[2]);
  float idle = stof(stats[3]);
  float iowait = stof(stats[4]);
  float irq = stof(stats[5]);
  float softirq = stof(stats[6]);
  float steal = stof(stats[7]);
  float Idle = idle + iowait;
  float nonidle = user + nice + system + irq + softirq + steal;

  float total = Idle + nonidle;
  float totald = total - prev_total_;
  float idled = Idle - prev_idle_;

  float util = (totald - idled) / totald;

  prev_total_ = total;
  prev_idle_ = Idle;


  return util;
}