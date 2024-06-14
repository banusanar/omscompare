#include <metrics.h>

namespace omscompare {
namespace model {

void Counter::start_watch() {
  begin_start_of_operation = std::chrono::steady_clock::now();
}

std::optional<Unit> Counter::stop_watch() {
  total_time_since_count +=
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::steady_clock::now() - begin_start_of_operation)
          .count();
  count++;
  if (count % 100 == 0) {
    auto result = Unit{.count = count, .timetaken = total_time_since_count};
    total_time_since_count = 0;
    return {result};
  }
  return {};
}

Metrics::Metrics() {
  units_.insert({Operation::ADD, {}});
  units_.insert({Operation::UPDATE, {}});
  units_.insert({Operation::DELETE, {}});
}

void Metrics::add(Operation &oper, uint64_t count, double timetaken) {
  units_[oper].emplace_back(Unit{.count = count, .timetaken = timetaken});
}

void Metrics::status() {
  for (auto [op, times] : units_) {
    std::cout << OpStr[int(op)] << " : (" << times.front().count << ", "
              << times.front().timetaken << ")" << std::endl;
  }
}

} // namespace model
} // namespace omscompare
