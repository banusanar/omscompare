#include "types/error.h"
#include "types/route.h"
#include <argparse/argparse.hpp>
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
  int num_runs = 10;
  try {
    argparse::ArgumentParser program("omscompare");
    program.add_argument("--count")
            .required()
            .help("Number of runs per workflow")
            .scan<'i', int>();

    program.add_argument("-o", "--order_route")
            .help("Creates 1 order and 1 route per workflow")
            .flag();

    program.add_argument("-f", "--order_route_fill")
            .help("Creates 1 order, 1 route, 1 fill per workflow")
            .flag();

    program.add_argument("-rm", "--order_multi_route_fill")
            .help("Creates 1 order, upto [count] routes, 1 fill per route per workflow")
            .flag();

    program.add_argument("-mf", "--order_multi_route_multi_fill")
            .help("Creates 1 order, upto [count] routes, upto [count] fills per route per workflow")
            .flag();

    program.parse_args(argc, argv); 
    num_runs = program.get<int>("count");

    const auto BOOST = app::Client::ContainerType::BOOST; 
    if(program["order_route"] == true)
    {
      std::cout << "Running workflow \'order_route\' for " << num_runs << " times" << std::endl; 
      types::ClientIdType clientid = 1001;
      app::Client client(clientid);
      client.init(BOOST);
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

    if(program["order_route_fill"] == true)
    {
      std::cout << "Running workflow \'order_route_fill\' for " << num_runs << " times" << std::endl; 
      types::ClientIdType clientid = 1024;
      app::Client client(clientid);
      client.init(BOOST);
      app::WorkFlow n1("order_route_fill", client);
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

    if(program["order_multi_route_fill"] == true)
    {
      std::cout << "Running workflow \'order_multi_route_one_fill\' for " << num_runs << " times" << std::endl; 
      types::ClientIdType clientid = 1024;
      app::Client client(clientid);
      client.init(BOOST);
      app::WorkFlow n1("order_mult_route_one_fill", client);
      for (int idx = 0; idx < num_runs; idx++) {
        std::string order_clord_id = "order_" + std::to_string(idx);

        auto create_routes_and_fill =
            [&](types::IdType o) -> tl::expected<void, types::Error> {
          for (int jdx = 0; jdx < std::min(idx,types::DATA_SIZE); jdx++) {
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

    if(program["order_multi_route_multi_fill"] == true)
    {
      std::cout << "Running workflow \'order_multi_route_multi_fill\' for " << num_runs << " times" << std::endl; 
      types::ClientIdType clientid = 1042;
      app::Client client(clientid);
      client.init(BOOST);
      app::WorkFlow n1("order_mult_route_multi_fill", client);
      for (int idx = 0; idx < num_runs; idx++) {
        std::string order_clord_id = "order_" + std::to_string(idx);

        auto create_routes_and_fills =
            [&](types::IdType o) -> tl::expected<void, types::Error> {
          for (int jdx = 0; jdx < std::min(idx,types::DATA_SIZE); jdx++) {
            std::string broker =
                "broker_" + std::to_string(idx) + "_" + std::to_string(jdx);

            auto result = n1.routeOrder(o, broker).and_then(
                [&](types::IdType r) -> tl::expected<void, types::Error> {
                  auto create_fill =
                      [&](void) -> tl::expected<void, types::Error> {
                    return n1.clientRO()->findRoute(r).and_then(
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