#include <metrics.h>

namespace omscompare {
namespace model {

void Counter::start_watch() { begin_start_of_operation = std::chrono::steady_clock::now(); }

void Counter::stop_watch() {
  total_time_since_count += std::chrono::duration_cast<std::chrono::microseconds>(
                                std::chrono::steady_clock::now() - begin_start_of_operation)
                                .count();
  count++;
}

} // namespace model
} // namespace omscompare
