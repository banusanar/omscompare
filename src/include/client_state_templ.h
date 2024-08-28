#ifndef OMSCOMPARE_OMSMODEL_CLIENT_STATE_TEMPLATE_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_STATE_TEMPLATE_H_

#include <tl/expected.hpp>
#include <types/basket.h>
#include <types/error.h>
#include <types/fill.h>
#include <types/idtype.h>
#include <types/order.h>
#include <types/route.h>

#include <boost_btree_storage.h>
#include <boost_hashed_storage.h>

namespace omscompare {
namespace model {

class StateStatistics;

template <typename STORAGE> class ClientState {
public:
  ClientState() : m_dbh() {}
  ~ClientState() {}
  StateStatistics counts() const;
  tl::expected<types::Order, types::Error> findOrder(types::IdType orderid) const;
  tl::expected<types::Order, types::Error> findOrderByClordId(types::FixClOrdIdType clordid) const;
  tl::expected<types::Basket, types::Error> findBasket(types::IdType orderid) const;
  tl::expected<types::Route, types::Error> findRoute(types::IdType orderid) const;
  tl::expected<types::Route, types::Error> findRouteByClordId(types::FixClOrdIdType clordid) const;
  tl::expected<types::Fill, types::Error> findFill(types::IdType orderid) const;

  std::vector<types::Order> findOrdersForBasketId(types::IdType basket_id) const;
  std::vector<types::Route> findRoutesForOrderId(types::IdType order_id,
                                                 types::RouteStatus status_match) const;
  std::vector<types::Fill> findFillsForRouteId(types::IdType route_id,
                                               types::ExecStatus status_match) const;
  std::vector<types::Fill> findFillsForOrderId(types::IdType basket_id,
                                               types::ExecStatus status_match) const;

  tl::expected<types::IdType, types::Error> addBasket(types::Basket &&);
  tl::expected<types::IdType, types::Error> addOrder(types::Order &&);
  tl::expected<types::IdType, types::Error> addRouteForOrder(types::Route &&,
                                                             types::IdType order_id);
  tl::expected<types::IdType, types::Error> addOrderForBasket(types::Order &&,
                                                              types::IdType basket_id);
  tl::expected<types::IdType, types::Error> addFillForRoute(types::Fill &&, types::IdType route_id);
  tl::expected<void, types::Error> updateRouteForOrder(types::Route &&);

private:
  STORAGE m_dbh;
};

template <> class ClientState<BoostBtreeStorage> {
public:
  ClientState() : m_dbh() {}
  ~ClientState() {}
  StateStatistics counts() const;
  tl::expected<types::Order, types::Error> findOrder(types::IdType orderid) const;
  tl::expected<types::Order, types::Error> findOrderByClordId(types::FixClOrdIdType clordid) const;
  tl::expected<types::Basket, types::Error> findBasket(types::IdType orderid) const;
  tl::expected<types::Route, types::Error> findRoute(types::IdType orderid) const;
  tl::expected<types::Route, types::Error> findRouteByClordId(types::FixClOrdIdType clordid) const;
  tl::expected<types::Fill, types::Error> findFill(types::IdType orderid) const;

  std::vector<types::Order> findOrdersForBasketId(types::IdType basket_id) const;
  std::vector<types::Route> findRoutesForOrderId(types::IdType order_id,
                                                 types::RouteStatus status_match) const;
  std::vector<types::Fill> findFillsForRouteId(types::IdType route_id,
                                               types::ExecStatus status_match) const;
  std::vector<types::Fill> findFillsForOrderId(types::IdType basket_id,
                                               types::ExecStatus status_match) const;

  tl::expected<types::IdType, types::Error> addBasket(types::Basket &&);
  tl::expected<types::IdType, types::Error> addOrder(types::Order &&);
  tl::expected<types::IdType, types::Error> addRouteForOrder(types::Route &&,
                                                             types::IdType order_id);
  tl::expected<types::IdType, types::Error> addOrderForBasket(types::Order &&,
                                                              types::IdType basket_id);
  tl::expected<types::IdType, types::Error> addFillForRoute(types::Fill &&, types::IdType route_id);
  tl::expected<void, types::Error> updateRouteForOrder(types::Route &&);

private:
  BoostBtreeStorage m_dbh;
};

using ClientStateBtreeStorage = class ClientState<BoostBtreeStorage>;

template <> class ClientState<BoostHashedStorage> {
public:
  ClientState() : m_dbh() {}
  ~ClientState() {}
  StateStatistics counts() const;
  tl::expected<types::Order, types::Error> findOrder(types::IdType orderid) const;
  tl::expected<types::Order, types::Error> findOrderByClordId(types::FixClOrdIdType clordid) const;
  tl::expected<types::Basket, types::Error> findBasket(types::IdType orderid) const;
  tl::expected<types::Route, types::Error> findRoute(types::IdType orderid) const;
  tl::expected<types::Route, types::Error> findRouteByClordId(types::FixClOrdIdType clordid) const;
  tl::expected<types::Fill, types::Error> findFill(types::IdType orderid) const;

  std::vector<types::Order> findOrdersForBasketId(types::IdType basket_id) const;
  std::vector<types::Route> findRoutesForOrderId(types::IdType order_id,
                                                 types::RouteStatus status_match) const;
  std::vector<types::Fill> findFillsForRouteId(types::IdType route_id,
                                               types::ExecStatus status_match) const;
  std::vector<types::Fill> findFillsForOrderId(types::IdType basket_id,
                                               types::ExecStatus status_match) const;

  tl::expected<types::IdType, types::Error> addBasket(types::Basket &&);
  tl::expected<types::IdType, types::Error> addOrder(types::Order &&);
  tl::expected<types::IdType, types::Error> addRouteForOrder(types::Route &&,
                                                             types::IdType order_id);
  tl::expected<types::IdType, types::Error> addOrderForBasket(types::Order &&,
                                                              types::IdType basket_id);
  tl::expected<types::IdType, types::Error> addFillForRoute(types::Fill &&, types::IdType route_id);
  tl::expected<void, types::Error> updateRouteForOrder(types::Route &&);

private:
  BoostHashedStorage m_dbh;
};

using ClientStateHashedStorage = class ClientState<BoostHashedStorage>;

} // namespace model
} // namespace omscompare

#endif