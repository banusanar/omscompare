#ifndef OMSCOMPARE_BOOST_HASHED_STORAGE_H_
#define OMSCOMPARE_BOOST_HASHED_STORAGE_H_

#include <boost_hashed/basket.h>
#include <boost_hashed/fill.h>
#include <boost_hashed/order.h>
#include <boost_hashed/route.h>
#include <boost_storage.h>

namespace omscompare {
using BoostHashedStorage =
    class containers::BoostStorage<containers::hashed::Basket, containers::hashed::Order,
                                   containers::hashed::Route, containers::hashed::Fill>;
}

#endif
