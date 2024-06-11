#ifndef OMSCOMPARE_OMSB_BASKET_H_
#define OMSCOMPARE_OMSB_BASKET_H_

#include <boost/container_hash/detail/hash_integral.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index_container_fwd.hpp>

#include <types/basket.h>

namespace omscompare {
namespace containers {

struct basket_by_idx {};
struct basket_by_name_idx {};

using Basket = boost::multi_index_container<
    types::Basket,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<basket_by_idx>,
            boost::multi_index::member<types::Basket,types::IdType,&types::Basket::id>>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<basket_by_name_idx>,
            boost::multi_index::member<types::Basket,std::string,&types::Basket::name>> >>;

using BasketByIdxType = Basket::index<basket_by_idx>::type;
using BasketByNameType = Basket::index<basket_by_name_idx>::type;
} // namespace containers
} // namespace omscompare

#endif