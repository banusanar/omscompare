#ifndef OMSCOMPARE_OMSB_ROUTE_H_
#define OMSCOMPARE_OMSB_ROUTE_H_

#include <boost/container_hash/detail/hash_integral.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index_container_fwd.hpp>

#include <types/route.h>

namespace omscompare {
namespace containers {

struct routeIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Route &entry) const {
    return boost::hash_value(entry.id);
  }
};

struct routeOrderIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Route &entry) const {
    return boost::hash_value(entry.order_id);
  }
};

struct statusRouteIdsForOrderIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Route &entry) const {
    result_type seed = 0;
    boost::hash_combine(seed, entry.status);
    boost::hash_combine(seed, entry.order_id);
    boost::hash_combine(seed, entry.id);
    return seed;
  }
};

using Route = boost::multi_index_container<
    types::Route,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<routeIdIdx>,
        boost::multi_index::hashed_non_unique<routeOrderIdIdx>,
        boost::multi_index::hashed_unique<statusRouteIdsForOrderIdIdx>>>;

} // namespace containers
} // namespace omscompare

#endif