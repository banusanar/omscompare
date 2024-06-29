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
  for (int bidx = Bucket::MSECS_0_TO_10; bidx < Bucket::MAX_BUCKET_VALUES; bidx++) {
    if (time_for_run >= start_buckets_compare[bidx] &&
        time_for_run < start_buckets_compare[bidx + 1]) {
      counts_per_bucket_[bidx]++;
      auto prev = average_per_bucket_[bidx];
      average_per_bucket_[bidx] = prev + ((time_for_run - prev) / counts_per_bucket_[bidx]);
    }
  }
  worst_time = std::max(double(time_for_run), worst_time);
  // std::cerr << state << " time: " << time_for_run << " msecs for " << funcname << std::endl;
}

} // namespace model
} // namespace omscompare
