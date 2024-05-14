#include <boost/container_hash/detail/hash_integral.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index_container_fwd.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include "../omstypes/order.h"

namespace omscompare {
    namespace omsb {

struct orderIdIdx
{
    typedef std::size_t result_type;
    result_type operator() (const omstypes::Order &entry) const
    {
        return boost::hash_value(entry.id);
    }
};

struct parentOrderIdIdx
{
    typedef std::size_t result_type;
    result_type operator() (const omstypes::Order &entry) const
    {
        return boost::hash_value(entry.parent_order_id);
    }
};

struct basketIdIdx
{
    typedef std::size_t result_type;
    result_type operator() (const omstypes::Order &entry) const
    {
        return (entry.basket_id.has_value()) ? boost::hash_value(entry.id) 
            : boost::hash_value(0u);
    }
};

//each client gets thier own boost::multi_index_containter for orders/baskets

using Order = boost::multi_index_container<omstypes::Order, 
                        boost::multi_index::indexed_by<
                            boost::multi_index::hashed_unique<orderIdIdx>,
                            boost::multi_index::hashed_non_unique<parentOrderIdIdx>,
                            boost::multi_index::hashed_non_unique<basketIdIdx>
                            >>;


    }
}