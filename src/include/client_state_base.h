#ifndef OMSCOMPARE_OMSMODEL_CLIENT_STATE_BASE_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_STATE_BASE_H_

#include <tl/expected.hpp>
#include <types/basket.h>
#include <types/error.h>
#include <types/fill.h>
#include <types/idtype.h>
#include <types/order.h>
#include <types/route.h>

namespace omscompare {
namespace model {

class StateStatistics;

class ClientStateBase {
public:
  ClientStateBase() = default;
  virtual ~ClientStateBase(){};
  virtual StateStatistics counts() const = 0;
  virtual tl::expected<types::Order, types::Error> findOrder(types::IdType orderid) const = 0;
  virtual tl::expected<types::Order, types::Error>
  findOrderByClordId(types::FixClOrdIdType clordid) const = 0;
  virtual tl::expected<types::Basket, types::Error> findBasket(types::IdType orderid) const = 0;
  virtual tl::expected<types::Route, types::Error> findRoute(types::IdType orderid) const = 0;
  virtual tl::expected<types::Route, types::Error>
  findRouteByClordId(types::FixClOrdIdType clordid) const = 0;
  virtual tl::expected<types::Fill, types::Error> findFill(types::IdType orderid) const = 0;

  virtual std::vector<types::Order> findOrdersForBasketId(types::IdType basket_id) const = 0;
  virtual std::vector<types::Route> findRoutesForOrderId(types::IdType order_id,
                                                         types::RouteStatus status_match) const = 0;
  virtual std::vector<types::Fill> findFillsForRouteId(types::IdType route_id,
                                                       types::ExecStatus status_match) const = 0;
  virtual std::vector<types::Fill> findFillsForOrderId(types::IdType basket_id,
                                                       types::ExecStatus status_match) const = 0;

  virtual tl::expected<types::IdType, types::Error> addBasket(types::Basket &&) = 0;
  virtual tl::expected<types::IdType, types::Error> addOrder(types::Order &&) = 0;
  virtual tl::expected<types::IdType, types::Error> addRouteForOrder(types::Route &&,
                                                                     types::IdType order_id) = 0;
  virtual tl::expected<types::IdType, types::Error> addOrderForBasket(types::Order &&,
                                                                      types::IdType basket_id) = 0;
  virtual tl::expected<types::IdType, types::Error> addFillForRoute(types::Fill &&,
                                                                    types::IdType route_id) = 0;
  // virtual tl::expected<types::IdType, types::Error>
  // addFillForOrderRoute(types::Fill &&, types::IdType route_id, types::IdType order_id) = 0;

  // virtual tl::expected<void, types::Error> updateOrder(types::Order &&) = 0;
  virtual tl::expected<void, types::Error> updateRouteForOrder(types::Route &&) = 0;
  // virtual tl::expected<void, types::Error> updateFillForRoute(types::Fill &&) = 0;

  // virtual tl::expected<void, types::Error> deleteBasket(types::IdType basket_id) = 0;
  // virtual tl::expected<void, types::Error> deleteOrder(types::IdType order_id) = 0;
  // virtual tl::expected<void, types::Error> deleteRouteForOrder(types::IdType route_id) = 0;
  // virtual tl::expected<void, types::Error> deleteFillForRoute(types::IdType fill_id) = 0;
};

} // namespace model
} // namespace omscompare

#endif