#include <SQLiteCpp/Statement.h>
#include <client_state_sqlite.h>
#include <memory>
#include <sstream>

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
const std::string update_route = ".routes SET status=?1 ,broker=?2, data=?3 WHERE routes.sid==?4;";

} // namespace

void ClientStateSqlite::setup_db_routines() {
  std::stringstream ss;
  auto db_exec = [&, this](const std::string &sql) {
    this->ddlSql(sql);
    ss.str("");
  };
  // Attach this database as a schema name for consistency
  {
    ss << "ATTACH DATABASE main AS " << client_schema;
    db_exec(ss.str());
  }
  // Create the tables before we can start the process for this client.
  // ddSQLite::Transaction transaction(dbh);

  const std::string create_table = "CREATE TABLE IF NOT EXISTS ";
  const std::string create_index = "CREATE INDEX ";

  { // basket table

    ss << create_table << client_schema << ".baskets ("
       << "sid INTEGER PRIMARY KEY, "
       << "name TEXT NOT NULL, "
       << "active INTEGER DEFAULT TRUE CHECK (active >= 0 AND active <= 1)"
       << ");";

    db_exec(ss.str());

    ss << create_index << client_schema << ".basketNameIdx ON baskets (name);";
    db_exec(ss.str());
  }

  { // orders table
    ss << create_table << client_schema << ".orders ("
       << "sid INTEGER PRIMARY KEY, "
       << "clord_id TEXT NOT NULL UNIQUE, "
       << "parent_order_id INTEGER CHECK (parent_order_id <> sid), "
       << "basket_id INTEGER, "
       << "data BLOB, "
       << "FOREIGN KEY (basket_id) REFERENCES baskets(sid) ON DELETE RESTRICT,"
       << "FOREIGN KEY (parent_order_id) REFERENCES orders(sid) ON DELETE RESTRICT"
       << ");";

    db_exec(ss.str());

    ss << create_index << client_schema << ".orderClordIdx ON orders (clord_id);";
    ss << create_index << client_schema << ".orderBasketIdx ON orders (basket_id);";

    db_exec(ss.str());
  }

  { // routes table
    ss << create_table << client_schema << ".routes ("
       << "sid INTEGER PRIMARY KEY, "
       << "clord_id TEXT NOT NULL UNIQUE, "
       << "order_id INTEGER NOT NULL, "
       << "status INTEGER NOT NULL, "
       << "broker TEXT NOT NULL, "
       << "data BLOB, "
       << "FOREIGN KEY (order_id) REFERENCES orders(sid) ON DELETE RESTRICT "
       << ")";

    db_exec(ss.str());

    ss << create_index << client_schema << ".routeClordIdx ON routes (clord_id);";
    ss << create_index << client_schema << ".routeOrderIdx ON routes (order_id);";
    ss << create_index << client_schema << ".routeStatusIDx ON routes (status, order_id);";

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
       << "data BLOB, "
       << "FOREIGN KEY (order_id) REFERENCES orders(sid) ON DELETE RESTRICT, "
       << "FOREIGN KEY (route_id) REFERENCES routes(sid) ON DELETE RESTRICT, "
       << "FOREIGN KEY (original_id) REFERENCES fills(sid) ON DELETE RESTRICT"
       << ")";

    db_exec(ss.str());

    ss << create_index << client_schema << ".fillExecIdx ON fills (exec_id);";
    ss << create_index << client_schema << ".fillRouteIdx ON fills (route_id);";
    ss << create_index << client_schema << ".fillOrderIdx ON fills (order_id);";
    ss << create_index << client_schema << ".fillStatusIDx ON fills (status, order_id);";
    ss << create_index << client_schema << ".fillStatus2IDx ON fills (status, order_id, route_id);";

    db_exec(ss.str());
  }
  // Setup query statements
  {
    ss << select_order << client_schema << ".orders WHERE sid=?1;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_ORDER, query);
    ss.str("");
  }

  {
    ss << select_order << client_schema << ".orders WHERE clord_id=?1;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_ORDER_BY_CLORD_ID, query);
    ss.str("");
  }

  {
    ss << select_order << client_schema << ".orders WHERE basket_id=?1;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_ORDERS_BY_BASKET_ID, query);
    ss.str("");
  }

  {
    ss << select_basket << client_schema << ".baskets WHERE sid=?1;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_BASKET, query);
    ss.str("");
  }

  {
    ss << select_route << client_schema << ".routes WHERE sid=?1;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_ROUTE, query);
    ss.str("");
  }

  {
    ss << select_route << client_schema << ".routes WHERE clord_id=?1;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_ROUTE_BY_CLORD_ID, query);
    ss.str("");
  }

  {
    ss << select_route << client_schema << ".routes WHERE order_id=?1 AND status=?2;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_ROUTES_BY_ORDER_ID, query);
    ss.str("");
  }

  {
    ss << select_fill << client_schema << ".fills WHERE sid=?1;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_FILL, query);
    ss.str("");
  }

  {
    ss << select_fill << client_schema << ".fills WHERE route_id=?1 AND status =?2;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_FILLS_BY_ROUTE_ID, query);
    ss.str("");
  }

  {
    ss << select_fill << client_schema << ".fills WHERE order_id=?1 AND status =?2;";
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::FIND_FILLS_BY_ORDER_ID, query);
    ss.str("");
  }

  {
    ss << "INSERT INTO " << client_schema << insert_basket;
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::ADD_BASKET, query);
    ss.str("");
  }

  {
    ss << "INSERT INTO " << client_schema << insert_order;
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::ADD_ORDER, query);
    ss.str("");
  }

  {
    ss << "INSERT INTO " << client_schema << insert_route;
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::ADD_ROUTE, query);
    ss.str("");
  }

  {
    ss << "INSERT INTO " << client_schema << insert_fill;
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::ADD_FILL, query);
    ss.str("");
  }

  {
    ss << "UPDATE " << client_schema << update_route;
    auto query = std::make_shared<SQLite::Statement>(*dbh, ss.str());
    statements.emplace(OperationId::UPDATE_ROUTE, query);
    ss.str("");
  }
}
} // namespace model
} // namespace omscompare
