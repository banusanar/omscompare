#include "types/error.h"
#include "types/route.h"
#include <argparse/argparse.hpp>
#include <client.h>
#include <client_state.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <types/idtype.h>
#include <workflows.h>

namespace {

// const std::string ORDER_ROUTE =
// const std::string ORDER_ROUTE_FILL =
// const std::string ORDER_MULT_ROUTE =
// const std::string ORDER_MULT_ROUTE_MULT_FILL = "order_multi_route_multi_fill";

const char *container_opts[] = {"b", "s"};
const std::string container_types[] = {"boost", "sqlite"};
const char *run_opts[] = {"o", "r", "m", "f"};
const std::string run_options[] = {"order_route", "order_route_fill", "order_multi_route_fill",
                                   "order_multi_route_multi_fill"};
const int RUN_SIZE = 256;

using namespace omscompare;
struct WorkFlowWrap {
  WorkFlowWrap(std::string container_type, int inner_loop_count)
      : appc(std::make_shared<app::Client>(1001)), wfname(container_type),
        inner_loop_size(inner_loop_count) {

    if (container_type == "boost") {
      appc->init(app::Client::BOOST);
    } else if (container_type == "sqlite") {
      appc->init(app::Client::SQLite);
    } else {
      throw std::runtime_error("Invalid container type");
    }
    w = std::make_shared<app::WorkFlow>(wfname, appc);
    run_functions.emplace("order_route", &WorkFlowWrap::runOrderRoute);
    run_functions.emplace("order_route_fill", &WorkFlowWrap::runOrderRouteFill);
    run_functions.emplace("order_multi_route_fill", &WorkFlowWrap::runOrderMultiRouteFill);
    run_functions.emplace("order_multi_route_multi_fill",
                          &WorkFlowWrap::runOrderMultiRouteMultiFill);
  }

  bool run(int num_runs, std::string &run_option) {
    auto iter = run_functions.find(run_option);
    if (iter == run_functions.end()) {
      throw std::runtime_error("Run options not found");
    }
    return (this->*iter->second)(num_runs);
  }

  bool runOrderRoute(int num_runs) {
    for (int idx = 0; idx < num_runs; idx++) {
      broker = wfname + "_broker_" + std::to_string(idx);
      order_clord_id = wfname + "_order_" + std::to_string(idx);
      auto res =
          w->createBasket(std::to_string(idx)).and_then([&](auto b) { return createOrder(b); });
      if (!res.has_value()) {
        std::cerr << "Found error in " << idx << " run [" << res.error().what << "]" << std::endl;
        return false;
      }
    }
    return true;
  }

  bool runOrderRouteFill(int num_runs) {
    for (int idx = 0; idx < num_runs; idx++) {
      broker = wfname + "_broker_" + std::to_string(idx);
      order_clord_id = wfname + "_order_" + std::to_string(idx);
      auto res = w->createBasket(std::to_string(idx)).and_then([&](auto b) {
        return createOrderRouteFill(b);
      });
      if (!res.has_value()) {
        std::cerr << "Found error in " << idx << " run [" << res.error().what << "]" << std::endl;
        return false;
      }
    }
    return true;
  }

  bool runOrderMultiRouteFill(int num_runs) {
    for (int idx = 0; idx < num_runs; idx++) {
      broker = wfname + "_broker_" + std::to_string(idx);
      order_clord_id = wfname + "_order_" + std::to_string(idx);
      auto res = w->createBasket(std::to_string(idx)).and_then([&](auto b) {
        return createOrderRoutesAndFill(b);
      });
      if (!res.has_value()) {
        std::cerr << "Found error in " << idx << " run [" << res.error().what << "]" << std::endl;
        return false;
      }
    }
    return true;
  }

  bool runOrderMultiRouteMultiFill(int num_runs) {
    for (int idx = 0; idx < num_runs; idx++) {
      broker = wfname + "_broker_" + std::to_string(idx);
      order_clord_id = wfname + "_order_" + std::to_string(idx);
      auto res = w->createBasket(std::to_string(idx)).and_then([&](auto b) {
        return createOrderRoutesAndMultiFills(b);
      });
      if (!res.has_value()) {
        std::cerr << "Found error in " << idx << " run [" << res.error().what << "]" << std::endl;
        return false;
      }
    }
    return true;
  }

  typedef bool (WorkFlowWrap::*run_function_type)(int);

private:
  std::shared_ptr<app::Client> appc;
  std::shared_ptr<app::WorkFlow> w;
  std::string wfname;
  std::string broker;
  std::string order_clord_id;
  int inner_loop_size{0};

  std::map<std::string, run_function_type> run_functions;

  tl::expected<void, types::Error> createRoute(types::IdType o) {
    return w->routeOrder(o, broker).and_then(
        [&](types::IdType r) -> tl::expected<void, types::Error> { return w->ackRoute(r); });
  }

  tl::expected<void, types::Error> createRouteAndFill(types::IdType o) {
    return w->routeOrder(o, broker).and_then(
        [&](types::IdType r) -> tl::expected<void, types::Error> {
          return w->ackRoute(r).and_then([&](void) -> tl::expected<void, types::Error> {
            auto x = w->createNewManualFillForRoute(r);
            if (x.has_value()) {
              return {};
            }
            return tl::make_unexpected(x.error());
          });
        });
  }

  tl::expected<void, types::Error> createRoutesAndFill(types::IdType o) {
    for (int jdx = 0; jdx < inner_loop_size; jdx++) {
      std::string fill_broker = broker + "_" + std::to_string(jdx);
      auto result =
          w->routeOrder(o, fill_broker)
              .and_then([&](types::IdType r) -> tl::expected<void, types::Error> {
                return w->ackRoute(r).and_then([&](void) -> tl::expected<void, types::Error> {
                  auto x = w->createNewManualFillForRoute(r);
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
  }

  tl::expected<void, types::Error> createRoutesAndFills(types::IdType o) {
    for (int jdx = 0; jdx < inner_loop_size; jdx++) {
      std::string fill_broker = broker + "_" + std::to_string(jdx);
      auto result =
          w->routeOrder(o, fill_broker)
              .and_then([&](types::IdType r) -> tl::expected<void, types::Error> {
                return w->ackRoute(r).and_then(
                    [&](void) -> tl::expected<void, types::Error> { return createFills(r); });
              });
      if (!result.has_value()) {
        return tl::make_unexpected(result.error());
      }
    }
    return {};
  }

  tl::expected<void, types::Error> createFills(types::IdType route_id) {
    return w->clientRO()->findRoute(route_id).and_then(
        [&](types::Route route) -> tl::expected<void, types::Error> {
          for (int zdx = 0; zdx < inner_loop_size; zdx++) {
            auto y =
                w->addFillForRoute(route.clord_id, route.broker + "fill_" + std::to_string(zdx));
            if (!y.has_value()) {
              return tl::make_unexpected(y.error());
            }
          }
          return {};
        });
  }

  tl::expected<void, types::Error> createOrder(types::IdType b) {
    return w->createOrder(order_clord_id, {b}).and_then([&](auto o) { return createRoute(o); });
  }

  tl::expected<void, types::Error> createOrderRouteFill(types::IdType b) {
    return w->createOrder(order_clord_id, {b}).and_then([&](auto o) {
      return createRouteAndFill(o);
    });
  }

  tl::expected<void, types::Error> createOrderRoutesAndFill(types::IdType b) {
    return w->createOrder(order_clord_id, {b}).and_then([&](auto o) {
      return createRoutesAndFill(o);
    });
  }

  tl::expected<void, types::Error> createOrderRoutesAndMultiFills(types::IdType b) {
    return w->createOrder(order_clord_id, {b}).and_then([&](auto o) {
      return createRoutesAndFills(o);
    });
  }
};
} // namespace

int main(int argc, char **argv) {
  using namespace omscompare;
  int num_runs = 10;
  try {
    argparse::ArgumentParser program("omscompare");
    for (int idx = 0; idx < 2; idx++) {
      std::string sopt = "-";
      sopt.append(container_opts[idx]);
      std::string bigopt = "--";
      bigopt.append(container_types[idx]);
      std::string help_text = "Use " + container_types[idx] + " for storage";
      program.add_argument(sopt, bigopt).help(help_text).flag();
    }

    program.add_argument("-c", "--count")
        .required()
        .help("Number of runs per workflow")
        .scan<'i', int>();

    program.add_argument("-i", "--inner_count")
        .default_value(RUN_SIZE)
        .help("Number of routes or fills per each iteration per workflow")
        .scan<'i', int>();

    for (int idx = 0; idx < 4; idx++) {
      std::string sopt = "-";
      sopt.append(run_opts[idx]);
      std::string bigopt = "--";
      bigopt.append(run_options[idx]);
      std::string help_text = "Creates " + run_options[idx] + " per workflow ";
      program.add_argument(sopt, bigopt).help(help_text).flag();
    }

    program.parse_args(argc, argv);
    num_runs = program.get<int>("count");

    auto loop_count = program.get<int>("inner_count");

    if (program["boost"] == false && program["sqlite"] == false) {
      std::cout << "Both Boost and Sqlite cannot be turned off for storage" << std::endl;
      return -1;
    }

    if (program[run_options[2]] == true && num_runs > 20000) {
      std::cout << "The max limits are reached at above 20000 for these options" << std::endl;
      return -1;
    }

    for (auto ropt : run_options) {
      if (program[ropt] == true) {
        std::cout << "Running workflow \'" << ropt << "\' for " << num_runs << " times"
                  << std::endl;
        for (auto st : container_types) {
          if (program[st] == true) {
            std::cout << "\tStart run for \'" << st << "\' container storage type" << std::endl;
            WorkFlowWrap wp(st, loop_count);
            wp.run(num_runs, ropt);
          }
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