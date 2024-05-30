#ifndef OMSCOMPARE_OMSMODEL_CLIENT_STATE_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_STATE_H_

#include "basket.h"
#include "fill.h"
#include "metrics.h"
#include "order.h"
#include "route.h"
#include <tl/expected.hpp>

namespace omscompare {
namespace model {

struct Error {
  int rcode{0};
  std::string what;
};

class ClientState {
public:
  ClientState() : baskets_(), orders_(), routes_(), fills_() {}

  tl::expected<containers::Order, Error> findOrder(types::IdType orderid);
  tl::expected<containers::Basket, Error> findBasket(types::IdType orderid);
  tl::expected<containers::Route, Error> findRoute(types::IdType orderid);
  tl::expected<containers::Fill, Error> findFill(types::IdType orderid);

  // empty vector could mean no values or errors??
  std::vector<containers::Order> findOrdersForBasketId(types::IdType basket_id,
                                                       bool active_only);
  std::vector<containers::Route>
  findRoutesForOrderId(types::IdType order_id, types::RouteStatus status_match);
  std::vector<containers::Fill>
  findFillsForRouteId(types::IdType route_id, types::ExecStatus status_match);
  std::vector<containers::Fill>
  findFillsForOrderId(types::IdType basket_id, types::ExecStatus status_match);

  tl::expected<types::IdType, Error> addOrder(containers::Order &&);
  tl::expected<types::IdType, Error> addRouteForOrder(containers::Route &&,
                                                      types::IdType order_id);
  tl::expected<types::IdType, Error> addOrderForBasket(containers::Order &&,
                                                       types::IdType basket_id);
  tl::expected<types::IdType, Error> addFillForRoute(containers::Fill &&,
                                                     types::IdType route_id);
  tl::expected<types::IdType, Error>
  addFillForOrderRoute(containers::Fill &&, types::IdType route_id,
                       types::IdType order_id);

  tl::expected<void, Error> updateOrder(containers::Order &&);
  tl::expected<void, Error> updateRouteForOrder(containers::Route &&);
  tl::expected<void, Error> updateFillForRoute(containers::Fill &&);

  tl::expected<void, Error> deleteBasket(types::IdType basket_id);
  tl::expected<void, Error> deleteOrder(types::IdType order_id);
  tl::expected<void, Error> deleteRouteForOrder(types::IdType route_id);
  tl::expected<void, Error> deleteFillForRoute(types::IdType fill_id);

  void status() {
    basket_metrics_.status();
    order_metrics_.status();
    route_metrics_.status();
    fill_metrics_.status();
  }

private:
  containers::Basket baskets_;
  containers::Order orders_;
  containers::Route routes_;
  containers::Fill fills_;

  Metrics basket_metrics_{"basket"};
  Metrics order_metrics_{"order"};
  Metrics route_metrics_{"route"};
  Metrics fill_metrics_{"fill"};
};

} // namespace model
} // namespace omscompare

#endif