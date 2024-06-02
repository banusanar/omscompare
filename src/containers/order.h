#ifndef OMSCOMPARE_OMSB_ORDER_H_
#define OMSCOMPARE_OMSB_ORDER_H_

#include <boost/container_hash/detail/hash_integral.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index_container_fwd.hpp>

#include <types/order.h>

namespace omscompare {
namespace containers {

struct order_by_idx {};
struct order_by_clord_idx {};
struct order_by_parent_idx {};
struct order_by_basket_idx {};

struct orderIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Order &entry) const {
    return boost::hash_value(entry.id);
  }
};

struct globalClordIdIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Order &entry) const {
    return boost::hash_value(entry.clord_id);
  }
};

struct parentIdOrderIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Order &entry) const {
    return boost::hash_value(entry.parent_order_id);
  }
};

struct basketIdOrderIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Order &entry) const {
    return (entry.basket_id.has_value()) ? boost::hash_value(entry.id)
                                         : boost::hash_value(0u);
  }
};

// each client gets thier own boost::multi_index_containter for orders/baskets

using Order = boost::multi_index_container<
    types::Order,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<boost::multi_index::tag<order_by_idx>,
                                          orderIdIdx>,
        boost::multi_index::hashed_unique<boost::multi_index::tag<order_by_clord_idx>,
                                          globalClordIdIdx>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<order_by_parent_idx>, parentIdOrderIdx>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<order_by_basket_idx>, basketIdOrderIdx>>>;

} // namespace containers
} // namespace omscompare

#endif