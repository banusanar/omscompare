#include "metrics.h"
#include "order.h"
#include <bits/ranges_base.h>
#include <client_state.h>
#include <tl/expected.hpp>

namespace omscompare {
namespace model {

using namespace containers;

ClientState::ClientState()
    : baskets_(), orders_(), routes_(), fills_(), metrics_() {
  metrics_.emplace("basket", Metrics{"basket"});
  metrics_.emplace("order", Metrics{"order"});
  metrics_.emplace("route", Metrics{"route"});
  metrics_.emplace("fill", Metrics{"fill"});
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

tl::expected<types::Basket, Error>
ClientState::findBasket(types::IdType orderid) {
  return {};
}
tl::expected<types::Route, Error>
ClientState::findRoute(types::IdType orderid) {
  return {};
}
tl::expected<types::Fill, Error> ClientState::findFill(types::IdType orderid) {
  return {};
}

// empty vector could mean no values or errors??
std::vector<types::Order>
ClientState::findOrdersForBasketId(types::IdType basket_id, bool active_only) {
  return {};
}
std::vector<types::Route>
ClientState::findRoutesForOrderId(types::IdType order_id,
                                  types::RouteStatus status_match) {
  return {};
}
std::vector<types::Fill>
ClientState::findFillsForRouteId(types::IdType route_id,
                                 types::ExecStatus status_match) {
  return {};
}
std::vector<types::Fill>
ClientState::findFillsForOrderId(types::IdType basket_id,
                                 types::ExecStatus status_match) {
  return {};
}

tl::expected<types::IdType, Error> addOrder(types::Order &&) { return {}; }
tl::expected<types::IdType, Error> addRouteForOrder(types::Route &&,
                                                    types::IdType order_id) {
  return {};
}
tl::expected<types::IdType, Error> addOrderForBasket(types::Order &&,
                                                     types::IdType basket_id) {
  return {};
}
tl::expected<types::IdType, Error> addFillForRoute(types::Fill &&,
                                                   types::IdType route_id) {
  return {};
}
tl::expected<types::IdType, Error>
addFillForOrderRoute(types::Fill &&, types::IdType route_id,
                     types::IdType order_id) {
  return {};
}

tl::expected<void, Error> updateOrder(types::Order &&) { return {}; }
tl::expected<void, Error> updateRouteForOrder(types::Route &&) { return {}; }
tl::expected<void, Error> updateFillForRoute(types::Fill &&) { return {}; }

tl::expected<void, Error> deleteBasket(types::IdType basket_id) { return {}; }
tl::expected<void, Error> deleteOrder(types::IdType order_id) { return {}; }
tl::expected<void, Error> deleteRouteForOrder(types::IdType route_id) {
  return {};
}
tl::expected<void, Error> deleteFillForRoute(types::IdType fill_id) {
  return {};
}

} // namespace model
} // namespace omscompare
