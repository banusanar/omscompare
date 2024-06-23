#include <metrics.h>

namespace omscompare {
namespace model {

void Counter::start_watch() { begin_start_of_operation = std::chrono::steady_clock::now(); }

void Counter::stop_watch() {
  auto time_for_run = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now() - begin_start_of_operation)
                          .count();
  total_time_since_count += time_for_run;
  count++;
  // https://en.wikipedia.org/wiki/Moving_average#Cumulative_average
  const auto prev = average_time_per_instance;
  average_time_per_instance = prev + ((time_for_run - prev) / count);
  if (time_for_run > average_time_per_instance * 2) { // twice the existing avg
    events_above_average++;
    std::cerr << "prev avg " << prev << " vs curr run " << time_for_run << std::endl;
  }
}

} // namespace model
} // namespace omscompare
