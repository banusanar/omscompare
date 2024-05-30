#include "order.h"
#include <client_state.h>

namespace omscompare {
namespace model {

using namespace containers;

tl::expected<containers::Order, Error>
ClientState::findOrder(types::IdType orderid) {
  auto x = orders_.get<order_by_idx>().find(orderid);
  return {};
}

tl::expected<containers::Basket, Error>
ClientState::findBasket(types::IdType orderid) {
  return {};
}
tl::expected<containers::Route, Error>
ClientState::findRoute(types::IdType orderid) {
  return {};
}
tl::expected<containers::Fill, Error>
ClientState::findFill(types::IdType orderid) {
  return {};
}

// empty vector could mean no values or errors??
std::vector<containers::Order>
ClientState::findOrdersForBasketId(types::IdType basket_id, bool active_only) {
  return {};
}
std::vector<containers::Route>
ClientState::findRoutesForOrderId(types::IdType order_id,
                                  types::RouteStatus status_match) {
  return {};
}
std::vector<containers::Fill>
ClientState::findFillsForRouteId(types::IdType route_id,
                                 types::ExecStatus status_match) {
  return {};
}
std::vector<containers::Fill>
ClientState::findFillsForOrderId(types::IdType basket_id,
                                 types::ExecStatus status_match) {
  return {};
}

tl::expected<types::IdType, Error> addOrder(containers::Order &&) { return {}; }
tl::expected<types::IdType, Error> addRouteForOrder(containers::Route &&,
                                                    types::IdType order_id) {
  return {};
}
tl::expected<types::IdType, Error> addOrderForBasket(containers::Order &&,
                                                     types::IdType basket_id) {
  return {};
}
tl::expected<types::IdType, Error> addFillForRoute(containers::Fill &&,
                                                   types::IdType route_id) {
  return {};
}
tl::expected<types::IdType, Error>
addFillForOrderRoute(containers::Fill &&, types::IdType route_id,
                     types::IdType order_id) {
  return {};
}

tl::expected<void, Error> updateOrder(containers::Order &&) { return {}; }
tl::expected<void, Error> updateRouteForOrder(containers::Route &&) {
  return {};
}
tl::expected<void, Error> updateFillForRoute(containers::Fill &&) { return {}; }

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
