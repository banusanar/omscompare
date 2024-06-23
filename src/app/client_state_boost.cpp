#include <basket.h>
#include <bits/ranges_base.h>
#include <client_state_boost.h>
#include <fill.h>
#include <metrics.h>
#include <order.h>
#include <route.h>
#include <tl/expected.hpp>
#include <types/idtype.h>

namespace omscompare {
namespace model {

const int BENCHMARK_SIZE = 100000;
using Error = types::Error;
using namespace containers;

ClientState::ClientState() : baskets_(), orders_(), routes_(), fills_() {}

//   // metrics_.emplace("basket", Metrics{});
//   // metrics_.emplace("order", Metrics{});
//   // metrics_.emplace("route", Metrics{});
//   // metrics_.emplace("fill", Metrics{});
// }

// ClientState::Scope::Scope(Metrics &m, Metrics::Operation oper)
//     : m(m), oper(oper) {
//   //m.counter().start_watch();
// }

// ClientState::Scope::~Scope() {
//   // auto stop = m.counter().stop_watch();
//   // if (stop.has_value()) {
//   //   m.add(oper, stop.value().count, stop.value().timetaken);
//   // }
// }

tl::expected<types::Order, Error> ClientState::findOrder(types::IdType orderid) const {
  auto x = orders_.get<order_by_idx>().find(orderid);
  if (x == orders_.get<order_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid orderid"});
  }
  return {*x};
}

tl::expected<types::Order, Error>
ClientState::findOrderByClordId(types::FixClOrdIdType clordid) const {
  OrderByClordIdType::iterator x = orders_.get<order_by_clord_idx>().find(clordid);
  if (x == orders_.get<order_by_clord_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid orderid"});
  }
  return {*x};
}

tl::expected<types::Basket, Error> ClientState::findBasket(types::IdType basketid) const {
  BasketByIdxType::iterator x = baskets_.get<basket_by_idx>().find(basketid);
  if (x == baskets_.get<basket_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid basketid"});
  }
  return {*x};
}

tl::expected<types::Route, Error> ClientState::findRoute(types::IdType routeid) const {
  RouteByIdxType::iterator x = routes_.get<route_by_idx>().find(routeid);
  if (x == routes_.get<route_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid routeid"});
  }
  return {*x};
}

tl::expected<types::Route, Error>
ClientState::findRouteByClordId(types::FixClOrdIdType clordid) const {
  RouteByClordIdType::iterator x = routes_.get<route_by_clord_idx>().find(clordid);
  if (x == routes_.get<route_by_clord_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid routeid"});
  }
  return {*x};
}

tl::expected<types::Fill, Error> ClientState::findFill(types::IdType fillid) const {
  FillByIdxType::iterator x = fills_.get<fill_by_idx>().find(fillid);
  if (x == fills_.get<fill_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid fillid"});
  }
  return {*x};
}

std::vector<types::Order> ClientState::findOrdersForBasketId(types::IdType basket_id) const {
  std::pair<OrderByBasketIdType::iterator, OrderByBasketIdType::iterator> x =
      orders_.get<order_by_basket_idx>().equal_range(basket_id);
  auto start = std::get<0>(x);
  auto end = std::get<1>(x);
  if (start == end) {
    return {};
  }
  std::vector<types::Order> result{};
  result.reserve(std::distance(start, end));
  for (; start != end; start++) {
    result.emplace_back(*start);
  }
  return result;
}

std::vector<types::Route> ClientState::findRoutesForOrderId(types::IdType order_id,
                                                            types::RouteStatus status_match) const {
  using RouteStatusPairType = std::pair<Route::index<route_by_status_order_idx>::type::iterator,
                                        Route::index<route_by_status_order_idx>::type::iterator>;

  RouteStatusPairType x =
      routes_.get<route_by_status_order_idx>().equal_range(std::make_tuple(status_match, order_id));
  auto start = std::get<0>(x);
  auto end = std::get<1>(x);
  if (start == end) {
    return {};
  }
  std::vector<types::Route> result{};
  result.reserve(std::distance(start, end));
  for (; start != end; start++) {
    result.emplace_back(*start);
  }
  return result;
}

std::vector<types::Fill> ClientState::findFillsForRouteId(types::IdType route_id,
                                                          types::ExecStatus status_match) const {
  auto [start, end] = fills_.get<fill_by_route_idx>().equal_range(route_id);
  if (start == end) {
    return {};
  }
  std::vector<types::Fill> result{};
  result.reserve(std::distance(start, end));
  for (; start != end; start++) {
    if (start->status == status_match)
      result.emplace_back(*start);
  }
  return result;
}

std::vector<types::Fill> ClientState::findFillsForOrderId(types::IdType order_id,
                                                          types::ExecStatus status_match) const {
  auto [start, end] =
      fills_.get<fill_by_status_order_idx>().equal_range(std::make_tuple(status_match, order_id));
  if (start == end) {
    return {};
  }
  std::vector<types::Fill> result{};
  result.reserve(std::distance(start, end));
  for (; start != end; start++) {
    result.emplace_back(*start);
  }
  return result;
}

// There is no more error checking here. All error checking happens before this
// API

tl::expected<types::IdType, types::Error> ClientState::addBasket(types::Basket &&basket) {
  auto [iter, result] = baskets_.insert(basket);
  if (!result)
    return tl::make_unexpected(Error{.what = "Basket insert failed"});
  if ((baskets_.size() % BENCHMARK_SIZE) == 0) {
    std::cerr << "Baskets at " << baskets_.size() << std::endl;
  }
  return {iter->id};
}

tl::expected<types::IdType, Error> ClientState::addOrder(types::Order &&order) {
  auto [iter, result] = orders_.insert(order);
  if (!result)
    return tl::make_unexpected(Error{.what = "Order insert failed"});

  if ((orders_.size() % BENCHMARK_SIZE) == 0) {
    std::cerr << "orders_ at " << orders_.size() << std::endl;
  }
  return {iter->id};
}

tl::expected<types::IdType, Error> ClientState::addRouteForOrder(types::Route &&route,
                                                                 types::IdType order_id) {
  return findOrder(order_id).and_then([&](types::Order) -> tl::expected<types::IdType, Error> {
    auto [iter, result] = routes_.insert(route);
    if (!result)
      return tl::make_unexpected(Error{.what = "Found Order. Route insert failed"});
    if ((routes_.size() % BENCHMARK_SIZE) == 0) {
      std::cerr << "routes_ at " << routes_.size() << std::endl;
    }
    return {iter->id};
  });
}

tl::expected<types::IdType, Error> ClientState::addOrderForBasket(types::Order &&order,
                                                                  types::IdType basket_id) {
  return findBasket(basket_id).and_then([&](types::Basket) -> tl::expected<types::IdType, Error> {
    auto [iter, result] = orders_.insert(order);
    if (!result)
      return tl::make_unexpected(Error{.what = "Found Basket. Order insert failed"});
    if ((orders_.size() % BENCHMARK_SIZE) == 0) {
      std::cerr << "orders_ at " << orders_.size() << std::endl;
    }
    return {iter->id};
  });
}

tl::expected<types::IdType, Error> ClientState::addFillForRoute(types::Fill &&fill,
                                                                types::IdType route_id) {
  return findRoute(route_id).and_then([&](types::Route) -> tl::expected<types::IdType, Error> {
    auto [iter, result] = fills_.insert(fill);
    if (!result)
      return tl::make_unexpected(Error{.what = "Found Route. Fill insert failed"});
    if ((fills_.size() % BENCHMARK_SIZE) == 0) {
      std::cerr << "fills_ at " << fills_.size() << std::endl;
    }
    return {iter->id};
  });
}

// tl::expected<types::IdType, Error> ClientState::addFillForOrderRoute(types::Fill &&fill,
//                                                                      types::IdType route_id,
//                                                                      types::IdType order_id) {
//   return findOrder(order_id).and_then([&](types::Order) -> tl::expected<types::IdType, Error> {
//     return findRoute(route_id).and_then([&](types::Route) -> tl::expected<types::IdType, Error> {
//       auto [iter, result] = fills_.insert(fill);
//       if (!result)
//         return tl::make_unexpected(Error{.what = "Found Route. Fill insert failed"});
//       if ((fills_.size() % BENCHMARK_SIZE) == 0) {
//         std::cerr << "fills_ at " << fills_.size() << std::endl;
//       }
//       return {iter->id};
//     });
//   });
// }

// tl::expected<void, Error> ClientState::updateOrder(types::Order &&order) {
//   return findOrder(order.id).and_then([&](types::Order) -> tl::expected<void, Error> {
//     auto [iter, result] = orders_.insert(order);
//     if (result) // This cannot be a new element. result is false
//       return tl::make_unexpected(Error{.what = "Update failed. "});
//     return {};
//   });
// }

tl::expected<void, Error> ClientState::updateRouteForOrder(types::Route &&route) {
  return findRoute(route.id).and_then([&](types::Route) -> tl::expected<void, Error> {
    auto [iter, result] = routes_.insert(route);
    if (result)
      return tl::make_unexpected(Error{.what = "Update failed. "});
    return {};
  });
}

// tl::expected<void, Error> ClientState::updateFillForRoute(types::Fill &&fill) {
//   return findFill(fill.id).and_then([&](types::Fill) -> tl::expected<void, Error> {
//     auto [iter, result] = fills_.insert(fill);
//     if (result)
//       return tl::make_unexpected(Error{.what = "Update failed. "});
//     return {};
//   });
// }

// tl::expected<void, Error> ClientState::deleteBasket(types::IdType basket_id) {
//   auto x = baskets_.get<basket_by_idx>().find(basket_id);
//   if (x == baskets_.end()) {
//     return tl::make_unexpected(Error{.what = "Invalid basketid. Cannot delete"});
//   }
//   baskets_.erase(x);
//   return {};
// }

// tl::expected<void, Error> ClientState::deleteOrder(types::IdType order_id) {
//   auto x = orders_.get<order_by_idx>().find(order_id);
//   if (x == orders_.end()) {
//     return tl::make_unexpected(Error{.what = "Invalid orderid. Cannot delete"});
//   }
//   orders_.erase(x);
//   return {};
// }

// tl::expected<void, Error> ClientState::deleteRouteForOrder(types::IdType route_id) {
//   auto x = routes_.get<route_by_idx>().find(route_id);
//   if (x == routes_.end()) {
//     return tl::make_unexpected(Error{.what = "Invalid routeid. Cannot delete"});
//   }
//   routes_.erase(x);
//   return {};
// }

// tl::expected<void, Error> ClientState::deleteFillForRoute(types::IdType fill_id) {
//   auto x = fills_.get<fill_by_idx>().find(fill_id);
//   if (x == fills_.end()) {
//     return tl::make_unexpected(Error{.what = "Invalid fill_id. Cannot delete"});
//   }
//   fills_.erase(x);
//   return {};
// }

} // namespace model
} // namespace omscompare
