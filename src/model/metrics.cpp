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
  if (count % 100) {
    auto result = Unit{count, total_time_since_count};
    total_time_since_count = 0;
    return {result};
  }
  return {};
}

Metrics::Metrics() {
  units.insert({Operation::ADD, {}});
  units.insert({Operation::UPDATE, {}});
  units.insert({Operation::DELETE, {}});
}

void Metrics::add(Operation &oper, uint64_t count, double timetaken) {
  units[oper].emplace_back(count, timetaken);
}

void Metrics::status() {
  for (auto [op, times] : units) {
    std::cout << OpStr[int(op)] << " : (" << times.front().count << ", "
              << times.front().timetaken << ")" << std::endl;
  }
}

} // namespace model
} // namespace omscompare