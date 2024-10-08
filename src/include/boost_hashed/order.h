#ifndef OMSCOMPARE_BOOST_HASHED_ORDER_H_
#define OMSCOMPARE_BOOST_HASHED_ORDER_H_

#include <boost/container_hash/detail/hash_integral.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>

#include <types/order.h>

namespace omscompare {
namespace containers {
namespace hashed {

struct order_by_idx {};
struct order_by_clord_idx {};
struct order_by_parent_idx {};
struct order_by_basket_idx {};

struct basketIdOrderIdx {
  typedef std::size_t result_type;
  result_type operator()(const types::Order &entry) const {
    return (entry.basket_id.has_value()) ? boost::hash_value(entry.id) : boost::hash_value(0u);
  }
};

// each client gets thier own boost::multi_index_containter for orders/baskets

using Order = boost::multi_index_container<
    types::Order,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<order_by_idx>,
            boost::multi_index::member<types::Order, types::IdType, &types::Order::id>>,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<order_by_clord_idx>,
            boost::multi_index::member<types::Order, types::FixClOrdIdType,
                                       &types::Order::clord_id>>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<order_by_parent_idx>,
            boost::multi_index::member<types::Order, types::IdType,
                                       &types::Order::parent_order_id>>,
        boost::multi_index::hashed_non_unique<boost::multi_index::tag<order_by_basket_idx>,
                                              basketIdOrderIdx>>>;

using OrderByIdxType = Order::index<order_by_idx>::type;
using OrderByClordIdType = Order::index<order_by_clord_idx>::type;
using OrderByBasketIdType = Order::index<order_by_basket_idx>::type;
} // namespace hashed
} // namespace containers
} // namespace omscompare

#endif