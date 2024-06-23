#include <algorithm>
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
  const auto prev = average_time;
  average_time = prev + ((time_for_run - prev) / count);
  if (time_for_run > average_time * 2) { // 2 times the existing avg
    if (time_for_run > average_time * 10) {
      worst_events_above_average++;
      const auto prev_worst = worst_avg_time;
      worst_avg_time = prev_worst + ((time_for_run - prev_worst) / worst_events_above_average);
    } else {
      events_above_average++;
      const auto prev_bad = bad_avg_time;
      bad_avg_time = prev_bad + ((time_for_run - prev_bad) / events_above_average);
    }
    average_time = prev; // reverse the average time
  }
  worst_time = std::max(double(time_for_run), worst_time);
}

} // namespace model
} // namespace omscompare
