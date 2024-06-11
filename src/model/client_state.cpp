#include <basket.h>
#include <fill.h>
#include <types/idtype.h>
#include <metrics.h>
#include <order.h>
#include <route.h>
#include <bits/ranges_base.h>
#include <client_state.h>
#include <tl/expected.hpp>

namespace omscompare {
namespace model {

using Error = types::Error;
using namespace containers;

ClientState::ClientState()
    : baskets_(), orders_(), routes_(), fills_(), metrics_() {
  metrics_.emplace("basket", Metrics{});
  metrics_.emplace("order", Metrics{});
  metrics_.emplace("route", Metrics{});
  metrics_.emplace("fill", Metrics{});
}

ClientState::Scope::Scope(Metrics &m, Metrics::Operation oper)
    : m(m), oper(oper) {
  m.counter().start_watch();
}

ClientState::Scope::~Scope() {
  auto stop = m.counter().stop_watch();
  if (stop.has_value()) {
    m.add(oper, stop.value().count, stop.value().timetaken);
  }
}

tl::expected<types::Order, Error>
ClientState::findOrder(types::IdType orderid) {
  Scope a{metrics_["order"], Metrics::Operation::FIND};
  auto x = orders_.get<order_by_idx>().find(orderid);
  if (x == orders_.end()) {
    return tl::make_unexpected(Error{.what = "Invalid orderid"});
  }
  return {*x};
}

tl::expected<types::Order, Error>
ClientState::findOrderByClordId(types::FixClOrdIdType clordid) {
  Scope a{metrics_["order"], Metrics::Operation::FIND};
  auto x = orders_.get<order_by_clord_idx>().find(clordid);
  if (x == orders_.get<order_by_clord_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid orderid"});
  }
  return {*x};
}

tl::expected<types::Basket, Error>
ClientState::findBasket(types::IdType basketid) {
  Scope a{metrics_["basket"], Metrics::Operation::FIND};
  auto x = baskets_.get<basket_by_idx>().find(basketid);
  if (x == baskets_.end()) {
    return tl::make_unexpected(Error{.what = "Invalid basketid"});
  }
  return {*x};
}

tl::expected<types::Route, Error>
ClientState::findRoute(types::IdType routeid) {
  Scope a{metrics_["route"], Metrics::Operation::FIND};
  auto x = routes_.get<route_by_idx>().find(routeid);
  if (x == routes_.end()) {
    return tl::make_unexpected(Error{.what = "Invalid routeid"});
  }
  return {*x};
}

tl::expected<types::Route, Error>
ClientState::findRouteByClordId(types::FixClOrdIdType clordid) {
  Scope a{metrics_["route"], Metrics::Operation::FIND};
  auto x = routes_.get<route_by_clord_idx>().find(clordid);
  if (x == routes_.get<route_by_clord_idx>().end()) {
    return tl::make_unexpected(Error{.what = "Invalid routeid"});
  }
  return {*x};
}

tl::expected<types::Fill, Error> ClientState::findFill(types::IdType fillid) {
  Scope a{metrics_["fill"], Metrics::Operation::FIND};
  auto x = fills_.get<fill_by_idx>().find(fillid);
  if (x == fills_.end()) {
    return tl::make_unexpected(Error{.what = "Invalid fillid"});
  }
  return {*x};
}

// empty vector could mean no values or errors??

std::vector<types::Order>
ClientState::findOrdersForBasketId(types::IdType basket_id) {
  Scope a{metrics_["order"], Metrics::Operation::FIND};
  auto [start, end] = orders_.get<order_by_basket_idx>().equal_range(basket_id);
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
ClientState::findRoutesForOrderId(types::IdType order_id,
                                  types::RouteStatus status_match) {
  Scope a{metrics_["route"], Metrics::Operation::FIND};
  auto [start, end] = routes_.get<route_by_status_order_idx>().equal_range(
      std::make_tuple(status_match, order_id));
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
ClientState::findFillsForRouteId(types::IdType route_id,
                                 types::ExecStatus status_match) {
  Scope a{metrics_["fill"], Metrics::Operation::FIND};
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

std::vector<types::Fill>
ClientState::findFillsForOrderId(types::IdType order_id,
                                 types::ExecStatus status_match) {
  Scope a{metrics_["fill"], Metrics::Operation::FIND};
  auto [start, end] = fills_.get<fill_by_status_order_idx>().equal_range(
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
ClientState::addBasket(types::Basket &&basket) {
  Scope a{metrics_["basket"], Metrics::Operation::ADD};
  auto [iter, result] = baskets_.insert(basket);
  if (!result)
    return tl::make_unexpected(Error{.what = "Basket insert failed"});

  return {iter->id};
}

tl::expected<types::IdType, Error> ClientState::addOrder(types::Order &&order) {
  Scope a{metrics_["order"], Metrics::Operation::ADD};
  auto [iter, result] = orders_.insert(order);
  if (!result)
    return tl::make_unexpected(Error{.what = "Order insert failed"});

  return {iter->id};
}

tl::expected<types::IdType, Error>
ClientState::addRouteForOrder(types::Route &&route, types::IdType order_id) {
  Scope a{metrics_["route"], Metrics::Operation::ADD};
  return findOrder(order_id).and_then(
      [&](types::Order) -> tl::expected<types::IdType, Error> {
        auto [iter, result] = routes_.insert(route);
        if (!result)
          return tl::make_unexpected(
              Error{.what = "Found Order. Route insert failed"});
        return {iter->id};
      });
}

tl::expected<types::IdType, Error>
ClientState::addOrderForBasket(types::Order &&order, types::IdType basket_id) {
  Scope a{metrics_["basket"], Metrics::Operation::ADD};
  return findBasket(basket_id).and_then(
      [&](types::Basket) -> tl::expected<types::IdType, Error> {
        auto [iter, result] = orders_.insert(order);
        if (!result)
          return tl::make_unexpected(
              Error{.what = "Found Basket. Order insert failed"});
        return {iter->id};
      });
}

tl::expected<types::IdType, Error>
ClientState::addFillForRoute(types::Fill &&fill, types::IdType route_id) {
  Scope a{metrics_["fill"], Metrics::Operation::ADD};
  return findRoute(route_id).and_then(
      [&](types::Route) -> tl::expected<types::IdType, Error> {
        auto [iter, result] = fills_.insert(fill);
        if (!result)
          return tl::make_unexpected(
              Error{.what = "Found Route. Fill insert failed"});
        return {iter->id};
      });
}

tl::expected<types::IdType, Error>
ClientState::addFillForOrderRoute(types::Fill &&fill, types::IdType route_id,
                                  types::IdType order_id) {
  Scope a{metrics_["fill"], Metrics::Operation::ADD};
  return findOrder(order_id).and_then(
      [&](types::Order) -> tl::expected<types::IdType, Error> {
        return findRoute(route_id).and_then(
            [&](types::Route) -> tl::expected<types::IdType, Error> {
              auto [iter, result] = fills_.insert(fill);
              if (!result)
                return tl::make_unexpected(
                    Error{.what = "Found Route. Fill insert failed"});
              return {iter->id};
            });
      });
}

tl::expected<void, Error> ClientState::updateOrder(types::Order &&order) {
  Scope a{metrics_["order"], Metrics::Operation::UPDATE};
  return findOrder(order.id).and_then(
      [&](types::Order) -> tl::expected<void, Error> {
        auto [iter, result] = orders_.insert(order);
        if (result) // This cannot be a new element. result is false
          return tl::make_unexpected(Error{.what = "Update failed. "});
        return {};
      });
}

tl::expected<void, Error>
ClientState::updateRouteForOrder(types::Route &&route) {
  Scope a{metrics_["route"], Metrics::Operation::UPDATE};
  return findRoute(route.id).and_then(
      [&](types::Route) -> tl::expected<void, Error> {
        auto [iter, result] = routes_.insert(route);
        if (result)
          return tl::make_unexpected(Error{.what = "Update failed. "});
        return {};
      });
}

tl::expected<void, Error> ClientState::updateFillForRoute(types::Fill &&fill) {
  Scope a{metrics_["fill"], Metrics::Operation::UPDATE};
  return findFill(fill.id).and_then(
      [&](types::Fill) -> tl::expected<void, Error> {
        auto [iter, result] = fills_.insert(fill);
        if (result)
          return tl::make_unexpected(Error{.what = "Update failed. "});
        return {};
      });
}

tl::expected<void, Error> ClientState::deleteBasket(types::IdType basket_id) {
  Scope a{metrics_["basket"], Metrics::Operation::DELETE};
  auto x = baskets_.get<basket_by_idx>().find(basket_id);
  if (x == baskets_.end()) {
    return tl::make_unexpected(
        Error{.what = "Invalid basketid. Cannot delete"});
  }
  baskets_.erase(x);
  return {};
}

tl::expected<void, Error> ClientState::deleteOrder(types::IdType order_id) {
  Scope a{metrics_["order"], Metrics::Operation::DELETE};
  auto x = orders_.get<order_by_idx>().find(order_id);
  if (x == orders_.end()) {
    return tl::make_unexpected(Error{.what = "Invalid orderid. Cannot delete"});
  }
  orders_.erase(x);
  return {};
}

tl::expected<void, Error>
ClientState::deleteRouteForOrder(types::IdType route_id) {
  Scope a{metrics_["route"], Metrics::Operation::DELETE};
  auto x = routes_.get<route_by_idx>().find(route_id);
  if (x == routes_.end()) {
    return tl::make_unexpected(Error{.what = "Invalid routeid. Cannot delete"});
  }
  routes_.erase(x);
  return {};
}

tl::expected<void, Error>
ClientState::deleteFillForRoute(types::IdType fill_id) {
  Scope a{metrics_["fill"], Metrics::Operation::DELETE};
  auto x = fills_.get<fill_by_idx>().find(fill_id);
  if (x == fills_.end()) {
    return tl::make_unexpected(Error{.what = "Invalid fill_id. Cannot delete"});
  }
  fills_.erase(x);
  return {};
}

} // namespace model
} // namespace omscompare
