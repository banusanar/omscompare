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

struct route_by_idx {};
struct route_by_order_idx {};
struct route_by_status_order_idx {};

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
        boost::multi_index::hashed_unique<boost::multi_index::tag<route_by_idx>,
                                          routeIdIdx>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<route_by_order_idx>, routeOrderIdIdx>,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<route_by_status_order_idx>,
            statusRouteIdsForOrderIdIdx>>>;

} // namespace containers
} // namespace omscompare

#endif