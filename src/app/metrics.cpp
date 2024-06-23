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
  if (average_time_per_instance == 0) {
    average_time_per_instance = time_for_run;
  } else { // https://en.wikipedia.org/wiki/Moving_average#Cumulative_average
    average_time_per_instance =
        average_time_per_instance + ((time_for_run - average_time_per_instance) / count);
  }
  if (time_for_run > average_time_per_instance * 0.05) {
    events_above_average++;
  }
}

} // namespace model
} // namespace omscompare
