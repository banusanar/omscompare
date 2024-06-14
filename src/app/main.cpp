#include "types/error.h"
#include "types/route.h"
#include <client.h>
#include <client_state.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <types/idtype.h>
#include <workflows.h>

int main(int argc, char **argv) {
  using namespace omscompare;
  int64_t num_runs = 10;
  try {
    if (argc == 2) {
    }

    {
      types::ClientIdType clientid = 1001;
      app::Client client(clientid);
      app::WorkFlow n1("order_route", client);
      for (int idx = 0; idx < num_runs; idx++) {
        std::string broker = "broker_" + std::to_string(idx);
        std::string order_clord_id = "order_" + std::to_string(idx);

        auto create_route =
            [&](types::IdType o) -> tl::expected<void, types::Error> {
          return n1.routeOrder(o, broker).and_then(
              [&](types::IdType r) -> tl::expected<void, types::Error> {
                return n1.ackRoute(r);
              });
        };

        auto create_order =
            [&](types::IdType b) -> tl::expected<void, types::Error> {
          std::optional<types::IdType> basket_id{b};
          return n1.createOrder(order_clord_id, basket_id)
              .and_then(create_route);
        };
        auto result =
            n1.createBasket(std::to_string(idx)).and_then(create_order);

        if (!result.has_value()) {
          std::cerr << "Found error in " << idx << " run ["
                    << result.error().what << "]" << std::endl;
          break;
        }
      }
    }

    {
      types::ClientIdType clientid = 1024;
      app::Client client(clientid);
      app::WorkFlow n1("order_route_one_fill", client);
      for (int idx = 0; idx < num_runs; idx++) {
        std::string broker = "broker_" + std::to_string(idx);
        std::string order_clord_id = "order_" + std::to_string(idx);

        auto create_route_and_fill =
            [&](types::IdType o) -> tl::expected<void, types::Error> {
          return n1.routeOrder(o, broker).and_then(
              [&](types::IdType r) -> tl::expected<void, types::Error> {
                return n1.ackRoute(r).and_then(
                    [&](void) -> tl::expected<void, types::Error> {
                      auto x = n1.createNewManualFillForRoute(r);
                      if (x.has_value()) {
                        return {};
                      }
                      return tl::make_unexpected(x.error());
                    });
              });
        };

        auto create_order =
            [&](types::IdType b) -> tl::expected<void, types::Error> {
          std::optional<types::IdType> basket_id{b};
          return n1.createOrder(order_clord_id, basket_id)
              .and_then(create_route_and_fill);
        };
        auto result =
            n1.createBasket(std::to_string(idx)).and_then(create_order);

        if (!result.has_value()) {
          std::cerr << "Found error in " << idx << " run ["
                    << result.error().what << "]" << std::endl;
          break;
        }
      }
    }

    {
      types::ClientIdType clientid = 1024;
      app::Client client(clientid);
      app::WorkFlow n1("order_mult_route_one_fill", client);
      for (int idx = 0; idx < num_runs; idx++) {
        std::string order_clord_id = "order_" + std::to_string(idx);

        auto create_routes_and_fill =
            [&](types::IdType o) -> tl::expected<void, types::Error> {
          for (int jdx = 0; jdx < idx; jdx++) {
            std::string broker =
                "broker_" + std::to_string(idx) + "_" + std::to_string(jdx);
            auto result = n1.routeOrder(o, broker).and_then(
                [&](types::IdType r) -> tl::expected<void, types::Error> {
                  return n1.ackRoute(r).and_then(
                      [&](void) -> tl::expected<void, types::Error> {
                        auto x = n1.createNewManualFillForRoute(r);
                        if (x.has_value()) {
                          return {};
                        }
                        return tl::make_unexpected(x.error());
                      });
                });
            if (!result.has_value()) {
              return tl::make_unexpected(result.error());
            }
          }
          return {};
        };

        auto create_order =
            [&](types::IdType b) -> tl::expected<void, types::Error> {
          std::optional<types::IdType> basket_id{b};
          return n1.createOrder(order_clord_id, basket_id)
              .and_then(create_routes_and_fill);
        };
        auto result =
            n1.createBasket(std::to_string(idx)).and_then(create_order);

        if (!result.has_value()) {
          std::cerr << "Found error in " << idx << " run ["
                    << result.error().what << "]" << std::endl;
          break;
        }
      }
    }

    {
      types::ClientIdType clientid = 1042;
      app::Client client(clientid);
      app::WorkFlow n1("order_mult_route_multi_fill", client);
      for (int idx = 0; idx < num_runs; idx++) {
        std::string order_clord_id = "order_" + std::to_string(idx);

        auto create_routes_and_fills =
            [&](types::IdType o) -> tl::expected<void, types::Error> {
          for (int jdx = 0; jdx < idx; jdx++) {
            std::string broker =
                "broker_" + std::to_string(idx) + "_" + std::to_string(jdx);

            auto result = n1.routeOrder(o, broker).and_then(
                [&](types::IdType r) -> tl::expected<void, types::Error> {
                  auto create_fill =
                      [&](void) -> tl::expected<void, types::Error> {
                    return n1.clientRO().findRoute(r).and_then(
                        [&](types::Route route)
                            -> tl::expected<void, types::Error> {
                          for (int zdx = 0; zdx < (10 * jdx); zdx++) {
                            std::string exec_id =
                                "fill_" + std::to_string(idx) + "_" +
                                std::to_string(jdx) + "_" + std::to_string(zdx);
                            auto y =
                                n1.addFillForRoute(route.clord_id, exec_id);
                            if (!y.has_value()) {
                              return tl::make_unexpected(y.error());
                            }
                          }
                          return {};
                        });
                  };
                  return n1.ackRoute(r).and_then(create_fill);
                });
            if (!result.has_value()) {
              return tl::make_unexpected(result.error());
            }
          }
          return {};
        };

        auto create_order =
            [&](types::IdType b) -> tl::expected<void, types::Error> {
          std::optional<types::IdType> basket_id{b};
          return n1.createOrder(order_clord_id, basket_id)
              .and_then(create_routes_and_fills);
        };
        auto result =
            n1.createBasket(std::to_string(idx)).and_then(create_order);

        if (!result.has_value()) {
          std::cerr << "Found error in " << idx << " run ["
                    << result.error().what << "]" << std::endl;
          break;
        }
      }
    }

  } catch (const std::exception &ae) {
    std::cerr << "Caught exception [" << ae.what() << "]" << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;
    return -1;
  }

  return 0;
}