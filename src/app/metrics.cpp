#include <algorithm>
#include <metrics.h>

namespace omscompare {
namespace model {

std::ostream &operator<<(std::ostream &os, const StateStatistics &lhs) {
  os << "[ O: " << lhs.orders << ", R: " << lhs.routes << ", F: " << lhs.fills
     << ", B: " << lhs.baskets << "]";
  return os;
}

void Counter::start_watch() { begin_start_of_operation = std::chrono::steady_clock::now(); }

double Counter::stop_watch() {
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() -
                                                               begin_start_of_operation)
      .count();
}

void Metrics::accum(double time_for_run, const StateStatistics &state,
                    const std::string &funcname) {
  total_time_since_count += time_for_run;
  count++;
  // https://en.wikipedia.org/wiki/Moving_average#Cumulative_average
  const auto prev = average_time;
  average_time = prev + ((time_for_run - prev) / count);
  if (time_for_run > average_time * state.bad_multiplier) { // 5 times the existing avg
    if (time_for_run > std::max(average_time * state.worse_multiplier, 40.0)) {
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
  // std::cerr << state << " time: " << time_for_run << " msecs for " << funcname << std::endl;
}

} // namespace model
} // namespace omscompare
