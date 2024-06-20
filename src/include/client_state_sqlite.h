#ifndef OMSCOMPARE_OMSMODEL_CLIENT_STATE_SQLITE_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_STATE_SQLITE_H_

#include <client_state_base.h>
#include <memory>
#include <types/idtype.h>

namespace SQLite {
class Database;
}

namespace omscompare {
namespace model {

class ClientStateSqlite : public ClientStateBase {
public:
  ClientStateSqlite(types::ClientIdType client_id);

  virtual tl::expected<types::Order, types::Error>
  findOrder(types::IdType orderid) override;
  virtual tl::expected<types::Order, types::Error>
  findOrderByClordId(types::FixClOrdIdType clordid) override;
  virtual tl::expected<types::Basket, types::Error>
  findBasket(types::IdType orderid) override;
  virtual tl::expected<types::Route, types::Error>
  findRoute(types::IdType orderid) override;
  virtual tl::expected<types::Route, types::Error>
  findRouteByClordId(types::FixClOrdIdType clordid) override;
  virtual tl::expected<types::Fill, types::Error>
  findFill(types::IdType orderid) override;

  virtual std::vector<types::Order>
  findOrdersForBasketId(types::IdType basket_id) override;
  virtual std::vector<types::Route>
  findRoutesForOrderId(types::IdType order_id,
                       types::RouteStatus status_match) override;
  virtual std::vector<types::Fill>
  findFillsForRouteId(types::IdType route_id,
                      types::ExecStatus status_match) override;
  virtual std::vector<types::Fill>
  findFillsForOrderId(types::IdType basket_id,
                      types::ExecStatus status_match) override;

  virtual tl::expected<types::IdType, types::Error>
  addBasket(types::Basket &&) override;
  virtual tl::expected<types::IdType, types::Error>
  addOrder(types::Order &&) override;
  virtual tl::expected<types::IdType, types::Error>
  addRouteForOrder(types::Route &&, types::IdType order_id) override;
  virtual tl::expected<types::IdType, types::Error>
  addOrderForBasket(types::Order &&, types::IdType basket_id) override;
  virtual tl::expected<types::IdType, types::Error>
  addFillForRoute(types::Fill &&, types::IdType route_id) override;
  virtual tl::expected<types::IdType, types::Error>
  addFillForOrderRoute(types::Fill &&, types::IdType route_id,
                       types::IdType order_id) override;

  virtual tl::expected<void, types::Error>
  updateOrder(types::Order &&) override;
  virtual tl::expected<void, types::Error>
  updateRouteForOrder(types::Route &&) override;
  virtual tl::expected<void, types::Error>
  updateFillForRoute(types::Fill &&) override;

  virtual tl::expected<void, types::Error>
  deleteBasket(types::IdType basket_id) override;
  virtual tl::expected<void, types::Error>
  deleteOrder(types::IdType order_id) override;
  virtual tl::expected<void, types::Error>
  deleteRouteForOrder(types::IdType route_id) override;
  virtual tl::expected<void, types::Error>
  deleteFillForRoute(types::IdType fill_id) override;

private:
  std::shared_ptr<SQLite::Database> dbh;

  void create_table(const std::string& sql); //throws instead of returning error
  tl::expected<void, types::Error> select(const std::string& sql);
  tl::expected<void, types::Error> insert(const std::string& sql);
  std::string client_schema;
};

} // namespace model
} // namespace omscompare

#endif