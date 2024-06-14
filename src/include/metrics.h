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
  Counter()
      : count(0), total_time_since_count(0.0), begin_start_of_operation() {}

  void start_watch();
  std::optional<Unit> stop_watch();

  constexpr uint64_t getCount() const { return count; }
  constexpr double getTimeTaken() const { return total_time_since_count; }

private:
  uint64_t count{0};
  double total_time_since_count{0.0};
  std::chrono::steady_clock::time_point begin_start_of_operation;
};

class Metrics {
public:
  enum class Operation : int { UNKNOWN, FIND, ADD, UPDATE, DELETE };
  const std::vector<std::string> OpStr{"UNKNOWN", "FIND", "ADD", "UPDATE",
                                       "DELETE"};

  Metrics();

  void add(Operation &oper, uint64_t count, double timetaken);
  void status();

  Counter &counter() { return counter_; }

private:
  std::map<Operation, std::list<Unit>> units_;
  Counter counter_;
};

} // namespace model
} // namespace omscompare
#endif