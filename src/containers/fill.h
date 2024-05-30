#ifndef OMSCOMPARE_OMSB_FILL_H_
#define OMSCOMPARE_OMSB_FILL_H_

#include <boost/container_hash/detail/hash_integral.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index_container_fwd.hpp>

#include <types/fill.h>

namespace omscompare {
namespace containers {

struct fillIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Fill &entry) const {
    return boost::hash_value(entry.id);
  }
};

struct fillRouteIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Fill &entry) const {
    return boost::hash_value(entry.route_id);
  }
};

struct fillOrderIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Fill &entry) const {
    return boost::hash_value(entry.order_id);
  }
};

struct statusFillIdsForOrderIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Fill &entry) const {
    result_type seed = 0;
    boost::hash_combine(seed, entry.status);
    boost::hash_combine(seed, entry.order_id);
    boost::hash_combine(seed, entry.id);
    return seed;
  }
};

struct statusFillIdsForRouteIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Fill &entry) const {
    result_type seed = 0;
    boost::hash_combine(seed, entry.status);
    boost::hash_combine(seed, entry.order_id);
    boost::hash_combine(seed, entry.route_id);
    boost::hash_combine(seed, entry.id);
    return seed;
  }
};

using Fill = boost::multi_index_container<
    types::Fill,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<fillIdIdx>,
        boost::multi_index::hashed_non_unique<fillRouteIdIdx>,
        boost::multi_index::hashed_non_unique<fillOrderIdIdx>,
        boost::multi_index::hashed_unique<statusFillIdsForRouteIdIdx>,
        boost::multi_index::hashed_unique<statusFillIdsForOrderIdIdx>>>;

} // namespace containers
} // namespace omscompare

#endif