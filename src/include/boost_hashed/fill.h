#ifndef OMSCOMPARE_BOOST_HASHED_FILL_H_
#define OMSCOMPARE_BOOST_HASHED_FILL_H_

#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>

#include <types/fill.h>

namespace omscompare {
namespace containers {
namespace hashed {

struct fill_by_idx {};
struct fill_by_exec_idx {};
struct fill_by_route_idx {};
struct fill_by_order_idx {};
struct fill_by_status_order_idx {};
struct fill_by_status_order_route_idx {};

using Fill = boost::multi_index_container<
    types::Fill,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<fill_by_idx>,
            boost::multi_index::member<types::Fill, types::IdType, &types::Fill::id>>,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<fill_by_exec_idx>,
            boost::multi_index::member<types::Fill, types::FixClOrdIdType, &types::Fill::exec_id>>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<fill_by_route_idx>,
            boost::multi_index::member<types::Fill, types::IdType, &types::Fill::route_id>>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<fill_by_order_idx>,
            boost::multi_index::member<types::Fill, types::IdType, &types::Fill::order_id>>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<fill_by_status_order_idx>,
            boost::multi_index::composite_key<
                types::Fill,
                boost::multi_index::member<types::Fill, types::ExecStatus, &types::Fill::status>,
                boost::multi_index::member<types::Fill, types::IdType, &types::Fill::order_id>>>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<fill_by_status_order_route_idx>,
            boost::multi_index::composite_key<
                types::Fill,
                boost::multi_index::member<types::Fill, types::ExecStatus, &types::Fill::status>,
                boost::multi_index::member<types::Fill, types::IdType, &types::Fill::order_id>,
                boost::multi_index::member<types::Fill, types::IdType, &types::Fill::route_id>>>>>;

using FillByIdxType = Fill::index<fill_by_idx>::type;
using FillByExecIdType = Fill::index<fill_by_exec_idx>::type;
using FillByOrderIdType = Fill::index<fill_by_order_idx>::type;
using FillByRouteIdType = Fill::index<fill_by_route_idx>::type;
} // namespace hashed
} // namespace containers
} // namespace omscompare

#endif