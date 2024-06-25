#ifndef OMSCOMPARE_OMSMODEL_CLIENT_STATE_SQLITE_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_STATE_SQLITE_H_

#include <SQLiteCpp/Statement.h>
#include <client_state_base.h>
#include <memory>
#include <types/idtype.h>

namespace SQLite {
class Database;
class Statement;
} // namespace SQLite

namespace omscompare {
namespace model {

class ClientStateSqlite : public ClientStateBase {
public:
  ClientStateSqlite(types::ClientIdType client_id);
  ~ClientStateSqlite() {}
  virtual StateStatistics counts() const override;
  virtual tl::expected<types::Order, types::Error> findOrder(types::IdType orderid) const override;
  virtual tl::expected<types::Order, types::Error>
  findOrderByClordId(types::FixClOrdIdType clordid) const override;
  virtual tl::expected<types::Basket, types::Error>
  findBasket(types::IdType basket_id) const override;
  virtual tl::expected<types::Route, types::Error> findRoute(types::IdType route_id) const override;
  virtual tl::expected<types::Route, types::Error>
  findRouteByClordId(types::FixClOrdIdType clordid) const override;
  virtual tl::expected<types::Fill, types::Error> findFill(types::IdType fill_id) const override;

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

  enum OperationId : int {
    ADD_BASKET,
    ADD_ORDER,
    ADD_ROUTE,
    ADD_FILL,
    FIND_ORDER,
    FIND_ORDER_BY_CLORD_ID,
    FIND_ORDERS_BY_BASKET_ID,
    FIND_ROUTE,
    FIND_ROUTE_BY_CLORD_ID,
    FIND_ROUTES_BY_ORDER_ID,
    FIND_BASKET,
    FIND_FILL,
    FIND_FILLS_BY_ROUTE_ID,
    FIND_FILLS_BY_ORDER_ID,
    UPDATE_ROUTE,
  };

private:
  std::string client_schema;
  std::shared_ptr<SQLite::Database> dbh;
  std::map<OperationId, std::shared_ptr<SQLite::Statement>> statements;

  void setup_db_routines();            // throws as part of constructor
  void ddlSql(const std::string &sql); // throws instead of returning error
  tl::expected<void, types::Error> dmlSql(SQLite::Statement &query);
};

} // namespace model
} // namespace omscompare

#endif