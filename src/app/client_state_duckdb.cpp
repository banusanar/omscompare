#include <client_state_duckdb.h>
#include <duckdb.h>
#include <metrics.h>
#include <types/error.h>
#include <types/fill.h>
#include <types/idtype.h>
#include <types/order.h>
#include <types/route.h>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Exception.h>
#include <SQLiteCpp/Statement.h>

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <string>
#include <tl/expected.hpp>
#include <vector>

namespace omscompare {
namespace model {

namespace {
const std::string select_basket = "SELECT sid, name, active FROM ";
const std::string select_order = "SELECT sid, clord_id, parent_order_id, basket_id, data FROM ";
const std::string select_route = "SELECT sid, clord_id, order_id, status, broker, data FROM ";
const std::string select_fill =
    "SELECT sid, exec_id, order_id, route_id, status, original_id, data FROM ";

const std::string insert_basket = ".baskets (sid, name, active) VALUES (?1,?2,?3);";
const std::string insert_order = ".orders (sid, clord_id, parent_order_id, basket_id, data) VALUES "
                                 "(?1,?2,?3,?4,?5);";
const std::string insert_route = ".routes (sid, clord_id, order_id, status, broker, data) VALUES "
                                 "(?1,?2,?3,?4,?5,?6);";
const std::string insert_fill = ".fills (sid, exec_id, order_id, route_id, status, data) VALUES "
                                "(?1,?2,?3,?4,?5,?6);";
const std::string insert_corrected_fill =
    ".fills (sid, exec_id, order_id, route_id, status, original_id, data) VALUES "
    "(?1,?2,?3,?4,?5,?6,?7);";
const std::string update_route = ".routes SET status=?1 ,broker=?2, data=?3 WHERE ";

std::vector<types::Order> getOrderFromSql(std::unique_ptr<duckdb::MaterializedQueryResult> result) {
  std::vector<types::Order> orders;
  for(auto& row : *result) {
    types::Order o= {.id = static_cast<types::IdType>(row.GetValue<types::IdType>(0)),
                    .clord_id = row.GetValue<std::string>(1),
                    .parent_order_id = static_cast<types::IdType>(row.GetValue<types::IdType>(2))};
    if (auto b = row.GetValue<types::IdType>(3) > 0)
      o.basket_id = b;

   // memcpy(o.data, row.GetValue<std::byte>(idx_t col_idx)
  }
  return (orders);
}

types::Basket getBasketFromSql(SQLite::Statement &query) {
  return (types::Basket{.id = static_cast<types::IdType>(query.getColumn(0).getInt64()),
                                 .name = query.getColumn(1).getString(),
                                 .is_active = (query.getColumn(2).getUInt() == 1) ? true : false});
}

types::Route getRouteFromSql(SQLite::Statement &query) {
  types::Route r{.id = static_cast<types::IdType>(query.getColumn(0).getInt64()),
                 .order_id = static_cast<types::IdType>(query.getColumn(2).getInt64()),
                 .clord_id = query.getColumn(1).getString(),
                 .status = types::RouteStatus(query.getColumn(3).getInt()),
                 .broker = query.getColumn(4).getString()};
  memcpy(r.data, query.getColumn(5).getBlob(), query.getColumn(5).getBytes());
  return (r);
}

types::Fill getFillFromSql(SQLite::Statement &query) {
  types::Fill r{
      .id = static_cast<types::IdType>(query.getColumn(0).getInt64()),
      .route_id = static_cast<types::IdType>(query.getColumn(3).getInt64()),
      .order_id = static_cast<types::IdType>(query.getColumn(2).getInt64()),
      .exec_id = query.getColumn(1).getString(),
      .status = types::ExecStatus(query.getColumn(4).getInt()),
  };
  if (query.getColumn(5).getInt64() > 0)
    r.original_id = std::make_optional(static_cast<types::IdType>(query.getColumn(5).getInt64()));
  memcpy(r.data, query.getColumn(6).getBlob(), query.getColumn(6).getBytes());
  return (r);
}

} // namespace

void ClientStateDuckdb::ddlSql(const std::string &sql) {
  auto res = dbh->Query(sql);
  if(res->HasError()) {
    std::stringstream os;
    os << "[" << sql << "] failed. " << res->GetError();
    throw std::runtime_error(os.str());
  }
}

// tl::expected<void, types::Error> ClientStateDuckdb::dmlSql(std::shared_ptr<duckdb::PreparedStatement> query)
//   try {
//     query->Execute();
//   } catch (const SQLite::Exception &ae) {
//     return tl::make_unexpected(
//         types::Error{.what = std::string(dbh->getErrorMsg() + query.getExpandedSQL())});
//   } catch (const std::exception &ue) {
//     return tl::make_unexpected(
//         types::Error{.what = std::string(dbh->getErrorMsg() + query.getExpandedSQL())});
//   }

//   return {};
// }

ClientStateDuckdb::ClientStateDuckdb(types::ClientIdType client_id)
    : ClientStateBase(client_id), client_schema("client_" + std::to_string(client_id)),
      db(), 
      dbh() {
  if (duckdb_open(NULL, &(*db)) == DuckDBError) {
    throw std::runtime_error("Cannot open database");
  }
  if (duckdb_connect(*db, &(*dbh)) == DuckDBError) {
    throw std::runtime_error("Cannot create database connection");
  }
  setup_db_routines();
}

ClientStateDuckdb::~ClientStateDuckdb() {
  for(auto [key, stmt] : statements) {
    duckdb_destroy_prepare(&(*stmt));
  }
  // cleanup
  duckdb_disconnect(&(*dbh));
  duckdb_close(&(*db));
}

StateStatistics ClientStateDuckdb::counts() const {
  // TODO
  return StateStatistics{0, 0, 0, 0};
}

tl::expected<types::Order, types::Error> ClientStateDuckdb::findOrder(types::IdType orderid) const {
  auto query = *statements.find(OperationId::FIND_ORDER)->second;
  duckdb_clear_bindings(query);
  duckdb_bind_int64(query, 1, 42);
  duckdb_result result;
  auto rc = duckdb_execute_prepared(query, &result);
  if(rc != DuckDBSuccess) {
    return tl::make_unexpected(types::Error{.what = duckdb_result_error(&result)});
  }

  int rows = duckdb_row_count(&result);
  if( rows != 1) { //There should be only 1 result
    if(rows == 0)
      return tl::make_unexpected(types::Error{.what = "No order found"});
    else 
      return tl::make_unexpected(types::Error{.what = "Duplicate result found"});
  }

  types::Order o = getOrderFromSql(&result, 1);
  duckdb_destroy_result(&result);
  return {o};
}

tl::expected<types::Order, types::Error>
ClientStateSqlite::findOrderByClordId(types::FixClOrdIdType clordid) const {
  auto query = statements.find(OperationId::FIND_ORDER_BY_CLORD_ID)->second;
  query->reset();
  query->bind(1, clordid);
  if (!query->executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid clordid"});
  }

  return {getOrderFromSql(*query)};
}

std::vector<types::Order> ClientStateSqlite::findOrdersForBasketId(types::IdType basket_id) const {
  auto query = statements.find(OperationId::FIND_ORDERS_BY_BASKET_ID)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(basket_id));
  std::vector<types::Order> orders;
  while (query->executeStep()) {
    orders.emplace_back(getOrderFromSql(*query));
  }
  return orders;
}

tl::expected<types::Basket, types::Error>
ClientStateSqlite::findBasket(types::IdType basketid) const {
  auto query = statements.find(OperationId::FIND_BASKET)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(basketid));
  if (!query->executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid basketid"});
  }

  return {getBasketFromSql(*query)};
}

tl::expected<types::Route, types::Error> ClientStateSqlite::findRoute(types::IdType routeid) const {
  auto query = statements.find(OperationId::FIND_ROUTE)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(routeid));
  if (!query->executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid routeid"});
  }

  return {getRouteFromSql(*query)};
}

tl::expected<types::Route, types::Error>
ClientStateSqlite::findRouteByClordId(types::FixClOrdIdType clordid) const {
  auto query = statements.find(OperationId::FIND_ROUTE_BY_CLORD_ID)->second;
  query->reset();
  query->bind(1, clordid);
  if (!query->executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid route clordid"});
  }

  return {getRouteFromSql(*query)};
}

std::vector<types::Route>
ClientStateSqlite::findRoutesForOrderId(types::IdType order_id,
                                        types::RouteStatus status_match) const {
  auto query = statements.find(OperationId::FIND_ROUTES_BY_ORDER_ID)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(order_id));
  query->bind(2, (int)status_match);
  std::vector<types::Route> routes;
  while (query->executeStep()) {
    routes.emplace_back(getRouteFromSql(*query));
  }
  return routes;
}

tl::expected<types::Fill, types::Error> ClientStateSqlite::findFill(types::IdType fill_id) const {
  auto query = statements.find(OperationId::FIND_FILL)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(fill_id));
  if (!query->executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid fill_id"});
  }

  return {getFillFromSql(*query)};
}

std::vector<types::Fill>
ClientStateSqlite::findFillsForRouteId(types::IdType route_id,
                                       types::ExecStatus status_match) const {
  auto query = statements.find(OperationId::FIND_FILLS_BY_ROUTE_ID)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(route_id));
  query->bind(2, (int)status_match);
  std::vector<types::Fill> fills;
  while (query->executeStep()) {
    fills.emplace_back(getFillFromSql(*query));
  }
  return fills;
}

std::vector<types::Fill>
ClientStateSqlite::findFillsForOrderId(types::IdType order_id,
                                       types::ExecStatus status_match) const {
  auto query = statements.find(OperationId::FIND_FILLS_BY_ORDER_ID)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(order_id));
  query->bind(2, (int)status_match);
  std::vector<types::Fill> fills;
  while (query->executeStep()) {
    fills.emplace_back(getFillFromSql(*query));
  }
  return fills;
}

tl::expected<types::IdType, types::Error> ClientStateSqlite::addBasket(types::Basket &&basket) {
  auto query = statements.find(OperationId::ADD_BASKET)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(basket.id));
  query->bind(2, basket.name);
  query->bind(3, (int)(basket.is_active ? 1 : 0));
  auto r = dmlSql(*query);
  if (r.has_value()) {
    return {basket.id};
  }
  return tl::make_unexpected(r.error());
}

tl::expected<types::IdType, types::Error> ClientStateSqlite::addOrder(types::Order &&order) {
  auto query = statements.find(OperationId::ADD_ORDER)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(order.id));
  query->bind(2, order.clord_id);
  if (order.parent_order_id != 0) {
    query->bind(3, static_cast<int64_t>(order.parent_order_id));
  } else {
    query->bind(3);
  }
  if (order.basket_id.has_value()) {
    query->bind(4, static_cast<int64_t>(order.basket_id.value()));
  } else {
    query->bind(4);
  }
  query->bindNoCopy(5, order.data, sizeof(order.data));
  auto r = dmlSql(*query);
  if (r.has_value()) {
    return {order.id};
  }
  return tl::make_unexpected(r.error());
}

tl::expected<types::IdType, types::Error>
ClientStateSqlite::addRouteForOrder(types::Route &&route, types::IdType order_id) {
  auto query = statements.find(OperationId::ADD_ROUTE)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(route.id));
  query->bind(2, route.clord_id);
  query->bind(3, static_cast<int64_t>(order_id));
  query->bind(4, static_cast<int>(route.status));
  query->bind(5, route.broker);
  query->bindNoCopy(6, route.data, sizeof(route.data));
  auto r = dmlSql(*query);
  if (r.has_value()) {
    return {route.id};
  }
  return tl::make_unexpected(r.error());
}

tl::expected<types::IdType, types::Error>
ClientStateSqlite::addOrderForBasket(types::Order &&order, types::IdType basket_id) {
  auto query = statements.find(OperationId::ADD_ORDER)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(order.id));
  query->bind(2, order.clord_id);
  if (order.parent_order_id == 0) {
    query->bind(3);
  } else {
    query->bind(3, static_cast<int64_t>(order.parent_order_id));
  }
  query->bind(4, static_cast<int64_t>(basket_id));
  query->bindNoCopy(5, order.data, sizeof(order.data));
  auto r = dmlSql(*query);
  if (r.has_value()) {
    return {order.id};
  }
  return tl::make_unexpected(r.error());
}

tl::expected<types::IdType, types::Error>
ClientStateSqlite::addFillForRoute(types::Fill &&fill, types::IdType route_id) {
  auto query = statements.find(OperationId::ADD_FILL)->second;
  query->reset();
  query->bind(1, static_cast<int64_t>(fill.id));
  query->bind(2, fill.exec_id);
  query->bind(3, static_cast<int64_t>(fill.order_id));
  query->bind(4, static_cast<int64_t>(route_id));
  query->bind(5, static_cast<int>(fill.status));
  query->bindNoCopy(6, fill.data, sizeof(fill.data));
  auto r = dmlSql(*query);
  if (r.has_value()) {
    return {fill.id};
  }
  return tl::make_unexpected(r.error());
}

tl::expected<void, types::Error> ClientStateSqlite::updateRouteForOrder(types::Route &&route) {
  auto query = statements.find(OperationId::UPDATE_ROUTE)->second;
  query->reset();
  query->bind(1, static_cast<int>(route.status));
  query->bind(2, route.broker);
  query->bindNoCopy(3, route.data, sizeof(route.data));
  query->bind(4, static_cast<int64_t>(route.id));
  return dmlSql(*query);
}

} // namespace model
} // namespace omscompare
