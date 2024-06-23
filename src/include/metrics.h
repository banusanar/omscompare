#ifndef OMSCOMPARE_MODEL_METRICS_H_
#define OMSCOMPARE_MODEL_METRICS_H_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace omscompare {
namespace model {

struct Unit {
  uint64_t count{0};
  double timetaken{0.0};
};

class Counter {
public:
  Counter() : count(0), total_time_since_count(0.0), begin_start_of_operation() {}

  void start_watch();
  void stop_watch();

  constexpr uint64_t getCount() const { return count; }
  constexpr double getTimeTaken() const { return total_time_since_count; }
  constexpr double getAverageTimeTaken() const { return average_time; }
  constexpr double getBadTimeTaken() const { return bad_avg_time; }
  constexpr double getWorstTimeTaken() const { return worst_avg_time; }
  constexpr double getWorstTime() const { return worst_time; }
  constexpr uint64_t getBadEventsAboveAverage() const { return events_above_average; }
  constexpr uint64_t getWorseEventsAboveAverage() const { return worst_events_above_average; }

private:
  uint64_t count{0};
  uint64_t events_above_average{0};
  uint64_t worst_events_above_average{0};
  double total_time_since_count{0.0};
  double worst_time{0.0};
  double average_time{0.0};
  double bad_avg_time{0.0};
  double worst_avg_time{0.0};
  std::chrono::steady_clock::time_point begin_start_of_operation;
};

class Metrics {
public:
  Metrics() {}

  Counter &readCounter() { return ro_counter_; }
  Counter &writeCounter() { return wo_counter_; }

private:
  Counter ro_counter_;
  Counter wo_counter_;
};

} // namespace model
} // namespace omscompare
#endif