#include "workflows.h"
#include "metrics.h"
#include <algorithm>
#include <basket.h>
#include <client_state.h>
#include <climits>
#include <fill.h>
#include <functional>
#include <iomanip>
#include <ios>
#include <memory>
#include <order.h>
#include <random>
#include <route.h>
#include <sys/types.h>
#include <tl/expected.hpp>
#include <types/idtype.h>

namespace omscompare {
namespace app {

namespace {

void generateRandomObj(std::byte *data, int size) {
  using random_bytes_engine =
      std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;
  random_bytes_engine rbe;
  std::generate_n(data, size, [rbe]() mutable { return std::byte(rbe()); });
}

auto print_stats = [](const model::Counter &lhs, std::string oper) {
  if (lhs.getCount() > 0) {
    std::cout << lhs.getCount() << " operations took " << std::setprecision(6)
              << std::ios_base::fixed;
    if (lhs.getTimeTaken() < 100000) {
      std::cout << lhs.getTimeTaken() << " msecs." << std::endl;
    } else {
      std::cout << lhs.getTimeTaken() / 100000 << " secs." << std::endl;
    }
    std::cout << "Each operation took an avg of " << std::setprecision(6) << std::ios_base::fixed
              << lhs.getAverageTimeTaken() << " msecs." << std::endl;
    std::cout << lhs.getEventsAboveAverage() << " events took twice the previous average time"
              << std::endl;
  }
};

} // namespace

WorkFlow::WorkFlow(std::string wf_name, std::shared_ptr<Client> client)
    : workflow_name(wf_name), client_(client), metric_() {
  if (!client_->is_ready()) {
    throw std::runtime_error("Cannot initiate workflow for this client. Not ready");
  }
}

WorkFlow::~WorkFlow() {
  // const auto totaltime =
  //     metric_.readCounter().getTimeTaken() + metric_.writeCounter().getTimeTaken();
  // const auto totalcount = metric_.readCounter().getCount() + metric_.writeCounter().getCount();
  // if (totaltime < 100000) {
  //   std::cout << totalcount << " operations took " << std::setprecision(6) <<
  //   std::ios_base::fixed
  //             << totaltime << " msecs." << std::endl;
  // } else {
  //   std::cout << totalcount << " operations took " << std::setprecision(6) <<
  //   std::ios_base::fixed
  //             << totaltime / 100000 << " secs." << std::endl;
  // }

  print_stats(metric_.readCounter(), "read");
  // print_stats(metric_.writeCounter(), "write");
}

WorkFlow::Scope::Scope(model::Metrics &m) : m(m) { m.readCounter().start_watch(); }

WorkFlow::Scope::~Scope() { m.readCounter().stop_watch(); }

tl::expected<types::IdType, types::Error>
WorkFlow::createOrder(std::string clord_id, std::optional<types::IdType> &&basket_id) {
  Scope s(metric_);
  types::Order order{.id = types::getNewOrderIdForClient(client_->client_id_),
                     .clord_id = types::getNewClordIdForClient(client_->client_id_),
                     .parent_order_id = 0,
                     .basket_id = basket_id};
  generateRandomObj(order.data, types::DATA_SIZE);
  return client_->state_->addOrder(std::move(order));
}

tl::expected<types::IdType, types::Error>
WorkFlow::createChildOrder(std::string clord_id, std::string &&parent_clord_id,
                           std::optional<types::IdType> &&basket_id) {
  Scope s(metric_);
  return client_->state_->findOrderByClordId(parent_clord_id)
      .and_then([&](types::Order parent_order) -> tl::expected<types::IdType, types::Error> {
        types::Order order{.id = types::getNewOrderIdForClient(client_->client_id_),
                           .clord_id = types::getNewClordIdForClient(client_->client_id_),
                           .parent_order_id = parent_order.id,
                           .basket_id = basket_id};
        generateRandomObj(order.data, types::DATA_SIZE);
        return client_->state_->addOrder(std::move(order));
      });
}

tl::expected<types::IdType, types::Error> WorkFlow::createBasket(std::string &&basket_name) {
  Scope s(metric_);
  types::Basket basket{.id = types::getNewBasketIdForClient(client_->client_id_),
                       .name = basket_name,
                       .is_active = true};
  return client_->state_->addBasket(std::move(basket));
}

tl::expected<types::IdType, types::Error> WorkFlow::routeOrder(types::IdType order_id,
                                                               std::string broker) {
  Scope s(metric_);
  types::Route route{.id = types::getNewRouteIdForClient(client_->client_id_),
                     .order_id = order_id,
                     .clord_id = types::getNewRouteClordIdForClient(client_->client_id_),
                     .status = types::RouteStatus::SENT_TO_BROKER,
                     .broker = broker};
  generateRandomObj(route.data, types::DATA_SIZE);
  return client_->state_->addRouteForOrder(std::move(route), order_id);
}

tl::expected<void, types::Error> WorkFlow::ackRoute(types::IdType route_id) {
  Scope s(metric_);
  return client_->state_->findRoute(route_id).and_then(
      [&](types::Route route) -> tl::expected<void, types::Error> {
        types::Route updroute{route};
        updroute.status = types::RouteStatus::BROKER_ACKNOWLEDGE;
        return client_->state_->updateRouteForOrder(std::move(updroute));
      });
}

tl::expected<types::IdType, types::Error>
WorkFlow::createNewManualFillForRoute(types::IdType route_id) {
  Scope s(metric_);
  return client_->state_->findRoute(route_id).and_then(
      [&](types::Route route) -> tl::expected<types::IdType, types::Error> {
        types::Fill fill{.id = types::getNewFillIdForClient(client_->client_id_),
                         .route_id = route.id,
                         .order_id = route.order_id,
                         .exec_id = types::getNewExecIdForClient(client_->client_id_, route.id),
                         .status = types::ExecStatus::NEW};
        generateRandomObj(fill.data, types::DATA_SIZE);
        return client_->state_->addFillForRoute(std::move(fill), route_id);
      });
}

tl::expected<types::IdType, types::Error>
WorkFlow::addFillForRoute(types::FixClOrdIdType &&route_clordid, types::FixClOrdIdType &&exec_id) {
  Scope s(metric_);
  return client_->state_->findRouteByClordId(route_clordid)
      .and_then([&](types::Route route) -> tl::expected<types::IdType, types::Error> {
        types::Fill fill{.id = types::getNewFillIdForClient(client_->client_id_),
                         .route_id = route.id,
                         .order_id = route.order_id,
                         .exec_id = exec_id,
                         .status = types::ExecStatus::NEW};
        generateRandomObj(fill.data, types::DATA_SIZE);
        return client_->state_->addFillForRoute(std::move(fill), route.id);
      });
}

} // namespace app
} // namespace omscompare
