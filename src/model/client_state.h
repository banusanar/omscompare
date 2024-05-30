#ifndef OMSCOMPARE_OMSMODEL_CLIENT_STATE_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_STATE_H_

#include "basket.h"
#include "fill.h"
#include "metrics.h"
#include "order.h"
#include "route.h"
#include <map>
#include <string>
#include <tl/expected.hpp>

namespace omscompare {
namespace model {

struct Error {
  int rcode{0};
  std::string what;
};

class ClientState {
public:
  ClientState();
  tl::expected<types::Order, Error> findOrder(types::IdType orderid);
  tl::expected<types::Basket, Error> findBasket(types::IdType orderid);
  tl::expected<types::Route, Error> findRoute(types::IdType orderid);
  tl::expected<types::Fill, Error> findFill(types::IdType orderid);

  // empty vector could mean no values or errors??
  std::vector<types::Order> findOrdersForBasketId(types::IdType basket_id,
                                                  bool active_only);
  std::vector<types::Route>
  findRoutesForOrderId(types::IdType order_id, types::RouteStatus status_match);
  std::vector<types::Fill> findFillsForRouteId(types::IdType route_id,
                                               types::ExecStatus status_match);
  std::vector<types::Fill> findFillsForOrderId(types::IdType basket_id,
                                               types::ExecStatus status_match);

  tl::expected<types::IdType, Error> addOrder(types::Order &&);
  tl::expected<types::IdType, Error> addRouteForOrder(types::Route &&,
                                                      types::IdType order_id);
  tl::expected<types::IdType, Error> addOrderForBasket(types::Order &&,
                                                       types::IdType basket_id);
  tl::expected<types::IdType, Error> addFillForRoute(types::Fill &&,
                                                     types::IdType route_id);
  tl::expected<types::IdType, Error>
  addFillForOrderRoute(types::Fill &&, types::IdType route_id,
                       types::IdType order_id);

  tl::expected<void, Error> updateOrder(types::Order &&);
  tl::expected<void, Error> updateRouteForOrder(types::Route &&);
  tl::expected<void, Error> updateFillForRoute(types::Fill &&);

  tl::expected<void, Error> deleteBasket(types::IdType basket_id);
  tl::expected<void, Error> deleteOrder(types::IdType order_id);
  tl::expected<void, Error> deleteRouteForOrder(types::IdType route_id);
  tl::expected<void, Error> deleteFillForRoute(types::IdType fill_id);

  void status() {
    for (auto &[s, m] : metrics_) {
      m.status();
    }
  }

  struct Scope {
    Scope(Metrics &m, Metrics::Operation oper);
    ~Scope();

    Metrics &m;
    Metrics::Operation oper;
  };

private:
  containers::Basket baskets_;
  containers::Order orders_;
  containers::Route routes_;
  containers::Fill fills_;

  std::map<std::string, Metrics> metrics_;
};

} // namespace model
} // namespace omscompare

#endif