#include "boost_btree_storage.h"
#include "boost_hashed_storage.h"
#include "client_templ.h"
#include "types/error.h"
#include "types/route.h"
#include "workflows.h"
#include "workflows_templ.h"
#include <argparse/argparse.hpp>
#include <client.h>
#include <client_state_boost.h>
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

const char *container_opts[] = {"b", "d"};                               //, "s"};
const std::string container_types[] = {"boost_ordered", "boost_hashed"}; //, "sqlite"};
// const char *run_opts[] = {"o", "r", "m", "f"};
// const std::string run_options[] = {"order_route", "order_route_fill", "order_multi_route_fill",
//                                    "order_multi_route_multi_fill"};
const int RUN_SIZE = 64;

using namespace omscompare;

using StandardSharedWf = std::shared_ptr<app::WorkFlow>;
using BoostBTreeWorkflows = std::shared_ptr<app::WorkFlowTempl<BoostBtreeStorage>>;
using BoostHashedWorkflows = std::shared_ptr<app::WorkFlowTempl<BoostHashedStorage>>;
using BoostBTreeClientType = app::ClientTempl<BoostBtreeStorage>;
using BoostHashedClientType = app::ClientTempl<BoostHashedStorage>;

template <typename WFTYPE>
tl::expected<void, types::Error> create_fills(std::shared_ptr<WFTYPE> &w, types::IdType route_id,
                                              int idx) {
  return w->clientRO().findRoute(route_id).and_then(
      [&](types::Route route) -> tl::expected<void, types::Error> {
        for (int zdx = 0; zdx < std::min(idx, RUN_SIZE); zdx++) {
          auto y = w->addFillForRoute(route.clord_id, route.broker + "fill_" + std::to_string(zdx));
          if (!y.has_value()) {
            return tl::make_unexpected(y.error());
          }
        }
        return {};
      });
}

template <typename WFTYPE>
tl::expected<types::IdType, types::Error> create_b_o_rs_fills(const std::string &wfname,
                                                              std::shared_ptr<WFTYPE> &w, int idx) {
  auto broker = wfname + "_broker_" + std::to_string(idx);
  auto order_clord_id = wfname + "_order_" + std::to_string(idx);
  return w->createBasket(std::to_string(idx))
      .and_then([&](auto b) -> tl::expected<types::IdType, types::Error> {
        return w->createOrder(order_clord_id, {b})
            .and_then([&](auto o) -> tl::expected<types::IdType, types::Error> {
              for (int jdx = 0; jdx < std::min(idx, RUN_SIZE); jdx++) {
                std::string fill_broker = broker + "_" + std::to_string(jdx);
                auto result =
                    w->routeOrder(o, fill_broker)
                        .and_then([&](types::IdType r) -> tl::expected<void, types::Error> {
                          return w->ackRoute(r).and_then(
                              [&](void) -> tl::expected<void, types::Error> {
                                return create_fills(w, r, idx);
                              });
                        });
                if (!result.has_value()) {
                  return tl::make_unexpected(result.error());
                }
              }
              return {o};
            });
      });
}

struct WorkFlowWrap {
  WorkFlowWrap(std::string container_type, int inner_loop_count = 0)
      : appc(std::make_shared<app::Client>(1001)), btreewf(nullptr), bhashwf(nullptr),
        btreecl(1002), bhashcl(1003), wfname(container_type), inner_loop_size(inner_loop_count) {

    if (container_type == "boost_ordered") {
      appc->init(app::Client::BOOST_ORDERED_INDEX);
      btreewf = std::make_shared<app::WorkFlowTempl<BoostBtreeStorage>>(container_type + "_templ",
                                                                        btreecl);
    } else if (container_type == "boost_hashed") {
      appc->init(app::Client::BOOST_HASHED_INDEX);
      bhashwf = std::make_shared<app::WorkFlowTempl<BoostHashedStorage>>(container_type + "_templ",
                                                                         bhashcl);
    }
    // else if (container_type == "sqlite") {
    //   appc->init(app::Client::SQLite);
    // }
    else {
      throw std::runtime_error("Invalid container type");
    }
    w = std::make_shared<app::WorkFlow>(wfname, appc);
  }

  bool runOrderMultiRouteMultiFill(int num_runs) {
    for (int idx = 0; idx < num_runs; idx++) {
      auto res = create_b_o_rs_fills<app::WorkFlow>(wfname, w, idx)
                     .and_then([&](auto o) {
                       if (btreewf) {
                         return create_b_o_rs_fills<app::WorkFlowTempl<BoostBtreeStorage>>(
                             wfname, btreewf, idx);
                       } else
                         return tl::expected<types::IdType, types::Error>{o};
                     })
                     .and_then([&](auto o) {
                       if (bhashwf) {
                         return create_b_o_rs_fills<app::WorkFlowTempl<BoostHashedStorage>>(
                             wfname, bhashwf, idx);
                       } else
                         return tl::expected<types::IdType, types::Error>{o};
                     });
      if (!res.has_value()) {
        std::cerr << "Found error in " << idx << " run [" << res.error().what << "]" << std::endl;
        return false;
      }
    }
    return true;
  }

private:
  std::shared_ptr<app::Client> appc;
  std::shared_ptr<app::WorkFlow> w;
  BoostBTreeClientType btreecl;
  BoostHashedClientType bhashcl;
  BoostBTreeWorkflows btreewf;
  BoostHashedWorkflows bhashwf;
  std::string wfname;
  std::string broker;
  std::string order_clord_id;
  int inner_loop_size{0};
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

    // program.add_argument("-i", "--inner_count")
    //     .default_value(RUN_SIZE)
    //     .help("Number of routes or fills per each iteration per workflow")
    //     .scan<'i', int>();

    // for (int idx = 0; idx < 4; idx++) {
    //   std::string sopt = "-";
    //   sopt.append(run_opts[idx]);
    //   std::string bigopt = "--";
    //   bigopt.append(run_options[idx]);
    //   std::string help_text = "Creates " + run_options[idx] + " per workflow ";
    //   program.add_argument(sopt, bigopt).help(help_text).flag();
    // }

    program.parse_args(argc, argv);
    num_runs = program.get<int>("count");

    // auto loop_count = program.get<int>("inner_count");

    if (program["boost_ordered"] == false && program["boost_hashed"] == false) {
      std::cout << "Both Storage options cannot be turned off" << std::endl;
      return -1;
    }

    if (num_runs > 20000) {
      std::cout << "The max limits are reached at above 20000 for these options" << std::endl;
      return -1;
    }

    std::cout << "Running workflow for " << num_runs << " times" << std::endl;
    for (auto st : container_types) {
      if (program[st] == true) {
        std::cout << "\tStart run for \'" << st << "\' container storage type" << std::endl;
        WorkFlowWrap wp(st);
        wp.runOrderMultiRouteMultiFill(num_runs);
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