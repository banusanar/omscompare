#ifndef OMSCOMPARE_BOOST_STORAGE_H_
#define OMSCOMPARE_BOOST_STORAGE_H_

namespace omscompare {
namespace containers {

template <typename Basket, typename Order, typename Route, typename Fill> struct BoostStorage {
  Basket baskets_;
  Order orders_;
  Route routes_;
  Fill fills_;

  BoostStorage() : baskets_(), orders_(), routes_(), fills_() {}
};

} // namespace containers
} // namespace omscompare
#endif
