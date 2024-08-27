#include "client_state_templ.h"
#include <bits/ranges_base.h>
#include <boost/basket.h>
#include <boost/fill.h>
#include <boost/order.h>
#include <boost/route.h>
#include <client_state_boost.h>
#include <iomanip>
#include <metrics.h>
#include <tl/expected.hpp>
#include <types/idtype.h>

namespace omscompare {
namespace model {

const int BENCHMARK_SIZE = 100000;
using Error = types::Error;
using namespace containers;

StateStatistics ClientStateHashedStorage::counts() const {
  return StateStatistics{.baskets = m_dbh.baskets_.size(),
                         .orders = m_dbh.orders_.size(),
                         .routes = m_dbh.routes_.size(),
                         .fills = m_dbh.fills_.size()};
}
//   // metrics_.emplace("basket", Metrics{});
//   // metrics_.emplace("order", Metrics{});
//   // metrics_.emplace("route", Metrics{});
//   // metrics_.emplace("fill", Metrics{});
// }

// ClientStateHashedStorage::Scope::Scope(Metrics &m, Metrics::Operation oper)
//     : m(m), oper(oper) {
//   //m.counter().start_watch();
// }

// ClientStateHashedStorage::Scope::~Scope() {
//   // auto stop = m.counter().stop_watch();
//   // if (stop.has_value()) {
//   //   m.add(oper, stop.value().count, stop.value().timetaken);
//   // }
// }

tl::expected<types::Order, Error> ClientStateHashedStorage::findOrder(types::IdType orderid) const {
  auto x = m_dbh.orders_.get<order_by_idx>().find(orderid);
  if (x == m_dbh.orders_.get<order_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid orderid"});
  }
  return {*x};
}

tl::expected<types::Order, Error>
ClientStateHashedStorage::findOrderByClordId(types::FixClOrdIdType clordid) const {
  OrderByClordIdType::iterator x = m_dbh.orders_.get<order_by_clord_idx>().find(clordid);
  if (x == m_dbh.orders_.get<order_by_clord_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid orderid"});
  }
  return {*x};
}

tl::expected<types::Basket, Error>
ClientStateHashedStorage::findBasket(types::IdType basketid) const {
  BasketByIdxType::iterator x = m_dbh.baskets_.get<basket_by_idx>().find(basketid);
  if (x == m_dbh.baskets_.get<basket_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid basketid"});
  }
  return {*x};
}

tl::expected<types::Route, Error> ClientStateHashedStorage::findRoute(types::IdType routeid) const {
  RouteByIdxType::iterator x = m_dbh.routes_.get<route_by_idx>().find(routeid);
  if (x == m_dbh.routes_.get<route_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid routeid"});
  }
  return {*x};
}

tl::expected<types::Route, Error>
ClientStateHashedStorage::findRouteByClordId(types::FixClOrdIdType clordid) const {
  RouteByClordIdType::iterator x = m_dbh.routes_.get<route_by_clord_idx>().find(clordid);
  if (x == m_dbh.routes_.get<route_by_clord_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid routeid"});
  }
  return {*x};
}

tl::expected<types::Fill, Error> ClientStateHashedStorage::findFill(types::IdType fillid) const {
  FillByIdxType::iterator x = m_dbh.fills_.get<fill_by_idx>().find(fillid);
  if (x == m_dbh.fills_.get<fill_by_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid fillid"});
  }
  return {*x};
}

std::vector<types::Order>
ClientStateHashedStorage::findOrdersForBasketId(types::IdType basket_id) const {
  std::pair<OrderByBasketIdType::iterator, OrderByBasketIdType::iterator> x =
      m_dbh.orders_.get<order_by_basket_idx>().equal_range(basket_id);
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

std::vector<types::Route>
ClientStateHashedStorage::findRoutesForOrderId(types::IdType order_id,
                                               types::RouteStatus status_match) const {
  using RouteStatusPairType = std::pair<Route::index<route_by_status_order_idx>::type::iterator,
                                        Route::index<route_by_status_order_idx>::type::iterator>;

  RouteStatusPairType x = m_dbh.routes_.get<route_by_status_order_idx>().equal_range(
      std::make_tuple(status_match, order_id));
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

std::vector<types::Fill>
ClientStateHashedStorage::findFillsForRouteId(types::IdType route_id,
                                              types::ExecStatus status_match) const {
  auto [start, end] = m_dbh.fills_.get<fill_by_route_idx>().equal_range(route_id);
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

std::vector<types::Fill>
ClientStateHashedStorage::findFillsForOrderId(types::IdType order_id,
                                              types::ExecStatus status_match) const {
  auto [start, end] = m_dbh.fills_.get<fill_by_status_order_idx>().equal_range(
      std::make_tuple(status_match, order_id));
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

tl::expected<types::IdType, types::Error>
ClientStateHashedStorage::addBasket(types::Basket &&basket) {
  auto [iter, result] = m_dbh.baskets_.insert(basket);
  if (!result)
    return tl::make_unexpected(Error{.what = "Basket insert failed"});
  // if ((baskets_.size() % BENCHMARK_SIZE) == 0) {
  //   std::cerr << "Baskets at " << baskets_.size() << std::endl;
  // }
  return {iter->id};
}

tl::expected<types::IdType, Error> ClientStateHashedStorage::addOrder(types::Order &&order) {
  auto [iter, result] = m_dbh.orders_.insert(order);
  if (!result)
    return tl::make_unexpected(Error{.what = "Order insert failed"});

  // if ((orders_.size() % BENCHMARK_SIZE) == 0) {
  //   std::cerr << "orders_ at " << orders_.size() << std::endl;
  // }
  return {iter->id};
}

tl::expected<types::IdType, Error>
ClientStateHashedStorage::addRouteForOrder(types::Route &&route, types::IdType order_id) {
  return findOrder(order_id).and_then([&](types::Order) -> tl::expected<types::IdType, Error> {
    auto [iter, result] = m_dbh.routes_.insert(route);
    if (!result)
      return tl::make_unexpected(Error{.what = "Found Order. Route insert failed"});
    // if ((routes_.size() % BENCHMARK_SIZE) == 0) {
    //   std::cerr << "routes_ at " << routes_.size() << std::endl;
    // }
    return {iter->id};
  });
}

tl::expected<types::IdType, Error>
ClientStateHashedStorage::addOrderForBasket(types::Order &&order, types::IdType basket_id) {
  return findBasket(basket_id).and_then([&](types::Basket) -> tl::expected<types::IdType, Error> {
    auto [iter, result] = m_dbh.orders_.insert(order);
    if (!result)
      return tl::make_unexpected(Error{.what = "Found Basket. Order insert failed"});
    // if ((orders_.size() % BENCHMARK_SIZE) == 0) {
    //   std::cerr << "orders_ at " << orders_.size() << std::endl;
    // }
    return {iter->id};
  });
}

tl::expected<types::IdType, Error>
ClientStateHashedStorage::addFillForRoute(types::Fill &&fill, types::IdType route_id) {
  return findRoute(route_id).and_then([&](types::Route) -> tl::expected<types::IdType, Error> {
    auto [iter, result] = m_dbh.fills_.insert(fill);
    if (!result)
      return tl::make_unexpected(Error{.what = "Found Route. Fill insert failed"});
    // if ((fills_.size() % BENCHMARK_SIZE) == 0) {
    //   std::cerr << "fills_ at " << fills_.size() << std::endl;
    // }
    return {iter->id};
  });
}

tl::expected<void, Error> ClientStateHashedStorage::updateRouteForOrder(types::Route &&route) {
  return findRoute(route.id).and_then([&](types::Route) -> tl::expected<void, Error> {
    auto [iter, result] = m_dbh.routes_.insert(route);
    if (result)
      return tl::make_unexpected(Error{.what = "Update failed. "});
    return {};
  });
}

} // namespace model
} // namespace omscompare
