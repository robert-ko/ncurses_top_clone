#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float prev_total_{0};
  float prev_idle_{0};
};

#endif