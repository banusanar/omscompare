#include "client_state_base.h"
#include "types/error.h"
#include "types/fill.h"
#include "types/order.h"
#include "types/route.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Statement.h>
#include <client_state_sqlite.h>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <types/idtype.h>
#include <vector>

namespace omscompare {
namespace model {

namespace {
const std::string select_basket = "SELECT sid, name, active FROM ";
const std::string select_order = "SELECT sid, clord_id, parent_order_id, basket_id, data FROM ";
const std::string select_route = "SELECT sid, clord_id, order_id, status, broker, data FROM ";
const std::string select_fill =
    "SELECT sid, exec_id, order_id, route_id, status, original_id, data FROM ";

types::Order &&getOrderFromSql(SQLite::Statement &query) {
  types::Order o = {.id = static_cast<types::IdType>(query.getColumn(0).getInt64()),
                    .clord_id = query.getColumn(1).getString(),
                    .parent_order_id = static_cast<types::IdType>(query.getColumn(2).getInt64())};
  if (query.getColumn(3).getInt64() > 0)
    o.basket_id = std::make_optional(static_cast<types::IdType>(query.getColumn(3).getInt64()));

  memcpy(o.data, query.getColumn(4).getBlob(), query.getColumn(4).getBytes());
  return std::move(o);
}

types::Basket &&getBasketFromSql(SQLite::Statement &query) {
  return std::move(types::Basket{.id = static_cast<types::IdType>(query.getColumn(0).getInt64()),
                                 .name = query.getColumn(1).getString(),
                                 .is_active = (query.getColumn(2).getUInt() == 1) ? true : false});
}

types::Route &&getRouteFromSql(SQLite::Statement &query) {
  types::Route r{.id = static_cast<types::IdType>(query.getColumn(0).getInt64()),
                 .order_id = static_cast<types::IdType>(query.getColumn(2).getInt64()),
                 .clord_id = query.getColumn(1).getString(),
                 .status = types::RouteStatus(query.getColumn(3).getInt()),
                 .broker = query.getColumn(4).getString()};
  memcpy(r.data, query.getColumn(5).getBlob(), query.getColumn(5).getBytes());
  return std::move(r);
}

types::Fill &&getFillFromSql(SQLite::Statement &query) {
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
  return std::move(r);
}
} // namespace

void ClientStateSqlite::create_table(const std::string &sql) {
  if (dbh->exec(sql) != SQLite::OK) {
    std::cerr << "[" << sql << "] failed. " << std::endl;
    throw std::runtime_error(dbh->getErrorMsg());
  }
}

// tl::expected<void, types::Error> ClientStateSqlite::select(const std::string&
// sql) {
//    SQLite::Statement query(*dbh, sql);
//    if(!query.executeStep()) {

//    }
// }

// bool insert(const std::string& sql)
ClientStateSqlite::ClientStateSqlite(types::ClientIdType client_id)
    : dbh(std::make_shared<SQLite::Database>(":memory:",
                                             SQLite::OPEN_MEMORY | SQLite::OPEN_NOMUTEX)),
      client_schema(std::to_string(client_id)) {
  // Create the tables before we can start the process for this client.
  // ddSQLite::Transaction transaction(dbh);

  const std::string create_table = "CREATE TABLE IF NOT EXISTS ";
  const std::string create_index = "CREATE INDEX ";
  std::stringstream ss;
  auto db_exec = [&, this](const std::string &sql) {
    this->create_table(sql);
    ss.str("");
  };

  { // basket table
    ss << create_table << client_schema << ".baskets ("
       << "sid INTEGER PRIMARY KEY, "
       << "name TEXT NOT NULL, "
       << "active INTEGER DEFAULT TRUE CHECK (active >= 0 AND active <= 1)"
       << ");";

    db_exec(ss.str());

    ss << create_index << "basketNameIdx ON " << client_schema << ".baskets (name);";
    db_exec(ss.str());
  }

  { // orders table
    ss << create_table << client_schema << ".orders ("
       << "sid INTEGER PRIMARY KEY, "
       << "clord_id TEXT NOT NULL UNIQUE, "
       << "parent_order_id INTEGER CHECK (parent_order_id <> sid), "
       << "basket_id INTEGER, "
       << "data BLOB "
       << "FOREIGN KEY (basket_id) REFERENCES " << client_schema
       << ".baskets(sid) ON DELETE RESTRICT"
       << "FOREIGN KEY (parent_order_id) REFERENCES " << client_schema
       << ".orders(sid) ON DELETE RESTRICT"
       << ")";

    db_exec(ss.str());

    ss << create_index << "orderClordIdx ON " << client_schema << ".orders (clordid);";
    ss << create_index << "orderBasketIdx ON " << client_schema << ".orders (basket_id);";

    db_exec(ss.str());
  }

  { // routes table
    ss << create_table << client_schema << ".routes ("
       << "sid INTEGER PRIMARY KEY, "
       << "clord_id TEXT NOT NULL UNIQUE, "
       << "order_id INTEGER NOT NULL, "
       << "status INTEGER NOT NULL, "
       << "broker TEXT NOT NULL, "
       << "data BLOB "
       << "FOREIGN KEY (order_id) REFERENCES " << client_schema << ".orders(sid) ON DELETE RESTRICT"
       << ")";

    db_exec(ss.str());

    ss << create_index << "routeClordIdx ON " << client_schema << ".routes (clordid);";
    ss << create_index << "routeOrderIdx ON " << client_schema << ".routes (order_id);";
    ss << create_index << "routeStatusIDx ON " << client_schema << ".routes (status, order_id);";

    db_exec(ss.str());
  }

  { // fills table
    ss << create_table << client_schema << ".fills ("
       << "sid INTEGER PRIMARY KEY, "
       << "exec_id TEXT NOT NULL UNIQUE, "
       << "order_id INTEGER NOT NULL, "
       << "route_id INTEGER NOT NULL, "
       << "status INTEGER NOT NULL, "
       << "original_id INTEGER CHECK (original_id <> sid), "
       << "data BLOB "
       << "FOREIGN KEY (order_id) REFERENCES " << client_schema << ".orders(sid) ON DELETE RESTRICT"
       << "FOREIGN KEY (route_id) REFERENCES " << client_schema << ".routes(sid) ON DELETE RESTRICT"
       << "FOREIGN KEY (original_id) REFERENCES " << client_schema
       << ".fills(sid) ON DELETE RESTRICT"
       << ")";

    db_exec(ss.str());

    ss << create_index << "fillExecIdx ON " << client_schema << ".fills (exec_id);";
    ss << create_index << "fillRouteIdx ON " << client_schema << ".fills (route_id);";
    ss << create_index << "fillOrderIdx ON " << client_schema << ".fills (order_id);";
    ss << create_index << "fillStatusIDx ON " << client_schema << ".fills (status, order_id);";
    ss << create_index << "fillStatus2IDx ON " << client_schema
       << ".fills (status, order_id, route_id);";

    db_exec(ss.str());
  }
}

tl::expected<types::Order, types::Error> ClientStateSqlite::findOrder(types::IdType orderid) {
  std::stringstream ss;
  ss << select_order << client_schema << ".orders WHERE "
     << " sid = " << orderid << ";";
  SQLite::Statement query(*dbh, ss.str());
  if (!query.executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid orderid"});
  }

  return {getOrderFromSql(query)};
}

tl::expected<types::Order, types::Error>
ClientStateSqlite::findOrderByClordId(types::FixClOrdIdType clordid) {
  std::stringstream ss;
  ss << select_order << client_schema << ".orders WHERE "
     << " clord_id = " << clordid << ";";
  SQLite::Statement query(*dbh, ss.str());
  if (!query.executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid clordid"});
  }

  return {getOrderFromSql(query)};
}

std::vector<types::Order> ClientStateSqlite::findOrdersForBasketId(types::IdType basket_id) {
  std::stringstream ss;
  ss << select_order << client_schema << ".orders WHERE "
     << " basket_id = " << basket_id << ";";
  SQLite::Statement query(*dbh, ss.str());
  std::vector<types::Order> orders;
  while (query.executeStep()) {
    orders.emplace_back(getOrderFromSql(query));
  }
  return orders;
}

tl::expected<types::Basket, types::Error> ClientStateSqlite::findBasket(types::IdType basketid) {
  std::stringstream ss;
  ss << select_basket << client_schema << ".baskets WHERE "
     << " sid = " << basketid << ";";
  SQLite::Statement query(*dbh, ss.str());
  if (!query.executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid basketid"});
  }

  return {getBasketFromSql(query)};
}

tl::expected<types::Route, types::Error> ClientStateSqlite::findRoute(types::IdType routeid) {
  std::stringstream ss;
  ss << select_route << client_schema << ".routes WHERE "
     << " sid = " << routeid << ";";
  SQLite::Statement query(*dbh, ss.str());
  if (!query.executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid routeid"});
  }

  return {getRouteFromSql(query)};
}

tl::expected<types::Route, types::Error>
ClientStateSqlite::findRouteByClordId(types::FixClOrdIdType clordid) {
  std::stringstream ss;
  ss << select_route << client_schema << ".routes WHERE "
     << " clord_id = " << clordid << ";";
  SQLite::Statement query(*dbh, ss.str());
  if (!query.executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid route clordid"});
  }

  return {getRouteFromSql(query)};
}

std::vector<types::Route> ClientStateSqlite::findRoutesForOrderId(types::IdType order_id,
                                                                  types::RouteStatus status_match) {
  std::stringstream ss;
  ss << select_route << client_schema << ".routes WHERE "
     << "order_id = " << order_id << " AND "
     << "status = " << int(status_match) << ";";
  SQLite::Statement query(*dbh, ss.str());
  std::vector<types::Route> routes;
  while (query.executeStep()) {
    routes.emplace_back(getRouteFromSql(query));
  }
  return routes;
}

tl::expected<types::Fill, types::Error> ClientStateSqlite::findFill(types::IdType fill_id) {
  std::stringstream ss;
  ss << select_fill << client_schema << ".fills WHERE "
     << " sid = " << fill_id << ";";
  SQLite::Statement query(*dbh, ss.str());
  if (!query.executeStep()) {
    return tl::make_unexpected(types::Error{.what = "Invalid fill_id"});
  }

  return {getFillFromSql(query)};
}

std::vector<types::Fill> ClientStateSqlite::findFillsForRouteId(types::IdType route_id,
                                                                types::ExecStatus status_match) {
  std::stringstream ss;
  ss << select_route << client_schema << ".fills WHERE "
     << "route_id = " << route_id << " AND "
     << "status = " << int(status_match) << ";";
  SQLite::Statement query(*dbh, ss.str());
  std::vector<types::Fill> fills;
  while (query.executeStep()) {
    fills.emplace_back(getFillFromSql(query));
  }
  return fills;
}

std::vector<types::Fill> ClientStateSqlite::findFillsForOrderId(types::IdType order_id,
                                                                types::ExecStatus status_match) {
  std::stringstream ss;
  ss << select_route << client_schema << ".fills WHERE "
     << "order_id = " << order_id << " AND "
     << "status = " << int(status_match) << ";";
  SQLite::Statement query(*dbh, ss.str());
  std::vector<types::Fill> fills;
  while (query.executeStep()) {
    fills.emplace_back(getFillFromSql(query));
  }
  return fills;
}

tl::expected<types::IdType, types::Error> addBasket(types::Basket &&);
tl::expected<types::IdType, types::Error> addOrder(types::Order &&);
tl::expected<types::IdType, types::Error> addRouteForOrder(types::Route &&, types::IdType order_id);
tl::expected<types::IdType, types::Error> addOrderForBasket(types::Order &&,
                                                            types::IdType basket_id);
tl::expected<types::IdType, types::Error> addFillForRoute(types::Fill &&, types::IdType route_id);
tl::expected<types::IdType, types::Error>
addFillForOrderRoute(types::Fill &&, types::IdType route_id, types::IdType order_id);

tl::expected<void, types::Error> updateOrder(types::Order &&);
tl::expected<void, types::Error> updateRouteForOrder(types::Route &&);
tl::expected<void, types::Error> updateFillForRoute(types::Fill &&);

tl::expected<void, types::Error> deleteBasket(types::IdType basket_id);
tl::expected<void, types::Error> deleteOrder(types::IdType order_id);
tl::expected<void, types::Error> deleteRouteForOrder(types::IdType route_id);
tl::expected<void, types::Error> deleteFillForRoute(types::IdType fill_id);

} // namespace model
} // namespace omscompare
