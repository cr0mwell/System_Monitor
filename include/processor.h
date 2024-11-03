#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long activeCpuUtil {};
  long totalCpuUtil {};
};

#endif