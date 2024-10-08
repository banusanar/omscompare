#ifndef OMSCOMPARE_OMSMODEL_CLIENT_STATE_BOOST_HASHED_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_STATE_BOOST_HASHED_H_

#include "types/idtype.h"
#include <boost_hashed/basket.h>
#include <boost_hashed/fill.h>
#include <boost_hashed/order.h>
#include <boost_hashed/route.h>
#include <client_state_base.h>
#include <map>
#include <metrics.h>

namespace omscompare {
namespace model {

class ClientStateBoostHashed : public ClientStateBase {
public:
  ClientStateBoostHashed(types::ClientIdType);
  ~ClientStateBoostHashed();
  virtual StateStatistics counts() const override;
  virtual tl::expected<types::Order, types::Error> findOrder(types::IdType orderid) const override;
  virtual tl::expected<types::Order, types::Error>
  findOrderByClordId(types::FixClOrdIdType clordid) const override;
  virtual tl::expected<types::Basket, types::Error>
  findBasket(types::IdType orderid) const override;
  virtual tl::expected<types::Route, types::Error> findRoute(types::IdType orderid) const override;
  virtual tl::expected<types::Route, types::Error>
  findRouteByClordId(types::FixClOrdIdType clordid) const override;
  virtual tl::expected<types::Fill, types::Error> findFill(types::IdType orderid) const override;

  virtual std::vector<types::Order> findOrdersForBasketId(types::IdType basket_id) const override;
  virtual std::vector<types::Route>
  findRoutesForOrderId(types::IdType order_id, types::RouteStatus status_match) const override;
  virtual std::vector<types::Fill>
  findFillsForRouteId(types::IdType route_id, types::ExecStatus status_match) const override;
  virtual std::vector<types::Fill>
  findFillsForOrderId(types::IdType basket_id, types::ExecStatus status_match) const override;

  virtual tl::expected<types::IdType, types::Error> addBasket(types::Basket &&) override;
  virtual tl::expected<types::IdType, types::Error> addOrder(types::Order &&) override;
  virtual tl::expected<types::IdType, types::Error>
  addRouteForOrder(types::Route &&, types::IdType order_id) override;
  virtual tl::expected<types::IdType, types::Error>
  addOrderForBasket(types::Order &&, types::IdType basket_id) override;
  virtual tl::expected<types::IdType, types::Error>
  addFillForRoute(types::Fill &&, types::IdType route_id) override;
  // virtual tl::expected<types::IdType, types::Error>
  // addFillForOrderRoute(types::Fill &&, types::IdType route_id, types::IdType order_id) override;

  // virtual tl::expected<void, types::Error> updateOrder(types::Order &&) override;
  virtual tl::expected<void, types::Error> updateRouteForOrder(types::Route &&) override;
  // virtual tl::expected<void, types::Error> updateFillForRoute(types::Fill &&) override;

  // virtual tl::expected<void, types::Error> deleteBasket(types::IdType basket_id) override;
  // virtual tl::expected<void, types::Error> deleteOrder(types::IdType order_id) override;
  // virtual tl::expected<void, types::Error> deleteRouteForOrder(types::IdType route_id) override;
  // virtual tl::expected<void, types::Error> deleteFillForRoute(types::IdType fill_id) override;

  // void status() {
  //   for (auto &[s, m] : metrics_) {
  //     m.status();
  //   }
  // }

  // struct Scope {
  //   Scope(Metrics &m, Metrics::Operation oper);
  //   ~Scope();

  //   Metrics &m;
  //   Metrics::Operation oper;
  // };

private:
  containers::hashed::Basket baskets_;
  containers::hashed::Order orders_;
  containers::hashed::Route routes_;
  containers::hashed::Fill fills_;

  // std::map<std::string, Metrics> metrics_;
};

} // namespace model
} // namespace omscompare

#endif