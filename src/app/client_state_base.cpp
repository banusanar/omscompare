#include <client_state_base.h>
#include <tl/expected.hpp>
#include <types/idtype.h>

namespace omscompare {
namespace model {

using Error = types::Error;

tl::expected<types::Order, Error>
ClientStateBase::findOrder(types::IdType orderid) const {
  return findOrder(orderid);
}

tl::expected<types::Order, Error>
ClientStateBase::findOrderByClordId(types::FixClOrdIdType clordid) const {
  return findOrderByClordId(clordid);
}

tl::expected<types::Basket, Error>
ClientStateBase::findBasket(types::IdType basketid) const {
  return findBasket(basketid);
}

tl::expected<types::Route, Error>
ClientStateBase::findRoute(types::IdType routeid) const {
  return findRoute(routeid);
}

tl::expected<types::Route, Error>
ClientStateBase::findRouteByClordId(types::FixClOrdIdType clordid) const {
  return findRouteByClordId(clordid);
}

tl::expected<types::Fill, Error>
ClientStateBase::findFill(types::IdType fillid) const {
  return findFill(fillid);
}

std::vector<types::Order>
ClientStateBase::findOrdersForBasketId(types::IdType basket_id) const {
  return findOrdersForBasketId(basket_id);
}

std::vector<types::Route>
ClientStateBase::findRoutesForOrderId(types::IdType order_id,
                                  types::RouteStatus status_match) const {
  return findRoutesForOrderId(order_id, status_match);
}

std::vector<types::Fill>
ClientStateBase::findFillsForRouteId(types::IdType route_id,
                                 types::ExecStatus status_match) const {
  return findFillsForRouteId(route_id, status_match);
}

std::vector<types::Fill>
ClientStateBase::findFillsForOrderId(types::IdType order_id,
                                 types::ExecStatus status_match) const {
  return findFillsForOrderId(order_id, status_match);
}

} // namespace model
} // namespace omscompare
