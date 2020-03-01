#include "format.h"
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hrs = seconds / 3600;
  long min = (seconds % 3600) / 60;
  long sec = min % 60;
  std::ostringstream out;


  
  //out << s_hrs << ":" << s_min << ":" << s_sec << "\n";
  
  out << std::setw(2) << std::setfill('0') << to_string(hrs) << ":" << std::setw(2) << to_string(min) << ":" << std::setw(2) << to_string(sec);
  return out.str();
}