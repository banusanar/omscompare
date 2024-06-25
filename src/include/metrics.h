#ifndef OMSCOMPARE_MODEL_METRICS_H_
#define OMSCOMPARE_MODEL_METRICS_H_

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

namespace omscompare {
namespace model {

struct StateStatistics {
  uint64_t baskets;
  uint64_t orders;
  uint64_t routes;
  uint64_t fills;
};

enum Bucket : int {
  MSECS_0_TO_10 = 0,
  MSECS_10_TO_20,
  MSECS_20_TO_40,
  MSECS_40_TO_100,
  MSECS_100_TO_200,
  MSECS_ABOVE_200,
  MAX_BUCKET_VALUES // some huge number
};
const int start_buckets_compare[] = {0, 10, 20, 40, 100, 200, std::numeric_limits<int>::max()};

std::ostream &operator<<(std::ostream &os, const StateStatistics &lhs);

class Counter {
public:
  Counter() : begin_start_of_operation() {}

  void start_watch();
  double stop_watch();

private:
  std::chrono::steady_clock::time_point begin_start_of_operation;
};

class Metrics {
public:
  Metrics() {}

  Counter &counter() { return counter_; }
  // Counter &writeCounter() { return wo_counter_; }

  void accum(double time_taken, const StateStatistics &, const std::string &);

  constexpr uint64_t getCount() const {
    return std::accumulate(counts_per_bucket_.begin(), counts_per_bucket_.end(), 0);
  }
  constexpr double getTimeTaken() const { return total_time_since_count; }
  constexpr double getWorstTime() const { return worst_time; }
  constexpr std::array<int, Bucket::MAX_BUCKET_VALUES> bucketCounts() const {
    return counts_per_bucket_;
  }
  constexpr std::array<double, Bucket::MAX_BUCKET_VALUES> bucketAverages() const {
    return average_per_bucket_;
  }

private:
  Counter counter_;
  std::array<int, Bucket::MAX_BUCKET_VALUES> counts_per_bucket_{};
  std::array<double, Bucket::MAX_BUCKET_VALUES> average_per_bucket_{};
  double total_time_since_count{0.0};
  double worst_time{0.0};
  // Counter wo_counter_;
};

} // namespace model
} // namespace omscompare
#endif