#ifndef OMSCOMPARE_BOOST_BTREE_STORAGE_H_
#define OMSCOMPARE_BOOST_BTREE_STORAGE_H_

#include <boost/basket.h>
#include <boost/fill.h>
#include <boost/order.h>
#include <boost/route.h>
#include <boost_storage.h>

namespace omscompare {
using BoostBtreeStorage = class containers::BoostStorage<containers::Basket, containers::Order,
                                                         containers::Route, containers::Fill>;
}

#endif
