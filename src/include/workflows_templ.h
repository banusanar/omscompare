#ifndef OMSCOMPARE_OMSMODEL_WORKFLOW_TEMPL_H_
#define OMSCOMPARE_OMSMODEL_WORKFLOW_TEMPL_H_

#include "client_templ.h"
#include "metrics.h"
#include "uuid.h"
#include <atomic>
#include <iomanip>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <types/error.h>
#include <types/idtype.h>

namespace omscompare {
namespace app {

template <typename CONTAINER_TYPE> class WorkFlowTempl {
public:
  WorkFlowTempl(std::string wf_name, ClientTempl<CONTAINER_TYPE> &client)
      : workflow_name(wf_name), client_(client), metric_() {}
  ~WorkFlowTempl();

  tl::expected<types::IdType, types::Error> createOrder(std::string clord_id,
                                                        std::optional<types::IdType> &&basket_id);

  tl::expected<types::IdType, types::Error>
  createChildOrder(std::string clord_id, std::string &&parent_clord_id,
                   std::optional<types::IdType> &&basket_id);

  tl::expected<types::IdType, types::Error> createBasket(std::string &&basket_name);

  tl::expected<types::IdType, types::Error> routeOrder(types::IdType order_id, std::string broker);

  tl::expected<void, types::Error> ackRoute(types::IdType route_id);

  tl::expected<types::IdType, types::Error> createNewManualFillForRoute(types::IdType route_id);

  tl::expected<types::IdType, types::Error> addFillForRoute(types::FixClOrdIdType route_clordid,
                                                            types::FixClOrdIdType &&exec_id);

  std::shared_ptr<const model::ClientStateBase> clientRO() const { return client_->state_; }

  struct Scope {
    Scope(model::Metrics &m, model::StateStatistics, std::string);
    ~Scope();

    model::Metrics &m;
    model::StateStatistics state;
    std::string funcname;
  };

private:
  std::string workflow_name;
  ClientTempl<CONTAINER_TYPE> &client_;
  model::Metrics metric_;
  std::atomic_bool is_success{false};
  //
  void sendToDownstreamSubscribers(){};
  int writeTransactionToPersistentDb() { return 0; };
};

template <typename T> WorkFlowTempl<T>::~WorkFlowTempl() {
  if (metric_.getCount() == 0) {
    return;
  }

  std::cout << metric_.getCount() << " operations took ";
  if (metric_.getTimeTaken() < 100000) {
    std::cout << metric_.getTimeTaken() << " msecs." << std::endl;
  } else {
    std::cout << metric_.getTimeTaken() / 100000 << " secs." << std::endl;
  }

  for (int idx = model::Bucket::MSECS_0_TO_10; idx < model::Bucket::MAX_BUCKET_VALUES; idx++) {
    if (metric_.bucketCounts()[idx] == 0) {
      continue;
    }
    std::cout << std::fixed << std::setprecision(4)
              << (metric_.bucketCounts()[idx] * 100 / (float)metric_.getCount())
              << "% operations took an avg of " << metric_.bucketAverages()[idx] << " msecs"
              << std::endl;
  }
  std::cerr << metric_.getWorstTime() << " was the max time for any event in this run" << std::endl;
}

template <typename T>
WorkFlowTempl<T>::Scope::Scope(model::Metrics &m, model::StateStatistics state,
                               std::string funcname)
    : m(m), state(state), funcname(funcname) {
  m.counter().start_watch();
}

template <typename T> WorkFlowTempl<T>::Scope::~Scope() {
  m.accum(m.counter().stop_watch(), state, funcname);
}

template <typename T>
tl::expected<types::IdType, types::Error>
WorkFlowTempl<T>::createOrder(std::string clord_id, std::optional<types::IdType> &&basket_id) {
  Scope s(metric_, clientRO()->counts(), __FUNCTION__);
  types::Order order{.id = types::getNewOrderIdForClient(client_.client_id_),
                     .clord_id = types::getNewClordIdForClient(client_.client_id_),
                     .parent_order_id = 0,
                     .basket_id = basket_id};
  uuid::generate_random_obj(order.data, types::DATA_SIZE);
  return client_.state_.addOrder(std::move(order));
}

template <typename T>
tl::expected<types::IdType, types::Error>
WorkFlowTempl<T>::createChildOrder(std::string clord_id, std::string &&parent_clord_id,
                                   std::optional<types::IdType> &&basket_id) {
  Scope s(metric_, clientRO()->counts(), __FUNCTION__);
  return client_.state_.findOrderByClordId(parent_clord_id)
      .and_then([&](types::Order parent_order) -> tl::expected<types::IdType, types::Error> {
        types::Order order{.id = types::getNewOrderIdForClient(client_.client_id_),
                           .clord_id = types::getNewClordIdForClient(client_.client_id_),
                           .parent_order_id = parent_order.id,
                           .basket_id = basket_id};
        uuid::generate_random_obj(order.data, types::DATA_SIZE);
        return client_.state_.addOrder(std::move(order));
      });
}

template <typename T>
tl::expected<types::IdType, types::Error>
WorkFlowTempl<T>::createBasket(std::string &&basket_name) {
  Scope s(metric_, clientRO()->counts(), __FUNCTION__);
  types::Basket basket{.id = types::getNewBasketIdForClient(client_.client_id_),
                       .name = basket_name,
                       .is_active = true};
  return client_.state_.addBasket(std::move(basket));
}

template <typename T>
tl::expected<types::IdType, types::Error> WorkFlowTempl<T>::routeOrder(types::IdType order_id,
                                                                       std::string broker) {
  Scope s(metric_, clientRO()->counts(), __FUNCTION__);
  types::Route route{.id = types::getNewRouteIdForClient(client_.client_id_),
                     .order_id = order_id,
                     .clord_id = types::getNewRouteClordIdForClient(client_.client_id_),
                     .status = types::RouteStatus::SENT_TO_BROKER,
                     .broker = broker};
  uuid::generate_random_obj(route.data, types::DATA_SIZE);
  return client_.state_.addRouteForOrder(std::move(route), order_id);
}

template <typename T>
tl::expected<void, types::Error> WorkFlowTempl<T>::ackRoute(types::IdType route_id) {
  Scope s(metric_, clientRO()->counts(), __FUNCTION__);
  return client_.state_.findRoute(route_id).and_then(
      [&](types::Route route) -> tl::expected<void, types::Error> {
        types::Route updroute{route};
        updroute.status = types::RouteStatus::BROKER_ACKNOWLEDGE;
        return client_.state_.updateRouteForOrder(std::move(updroute));
      });
}

template <typename T>
tl::expected<types::IdType, types::Error>
WorkFlowTempl<T>::createNewManualFillForRoute(types::IdType route_id) {
  Scope s(metric_, clientRO()->counts(), __FUNCTION__);
  return client_.state_.findRoute(route_id).and_then(
      [&](types::Route route) -> tl::expected<types::IdType, types::Error> {
        types::Fill fill{.id = types::getNewFillIdForClient(client_.client_id_),
                         .route_id = route.id,
                         .order_id = route.order_id,
                         .exec_id = types::getNewExecIdForClient(client_.client_id_, route.id),
                         .status = types::ExecStatus::NEW};
        uuid::generate_random_obj(fill.data, types::DATA_SIZE);
        return client_.state_.addFillForRoute(std::move(fill), route_id);
      });
}

template <typename T>
tl::expected<types::IdType, types::Error>
WorkFlowTempl<T>::addFillForRoute(types::FixClOrdIdType route_clordid,
                                  types::FixClOrdIdType &&exec_id) {
  Scope s(metric_, clientRO()->counts(), __FUNCTION__);
  return client_.state_.findRouteByClordId(route_clordid)
      .and_then([&](types::Route route) -> tl::expected<types::IdType, types::Error> {
        types::Fill fill{.id = types::getNewFillIdForClient(client_.client_id_),
                         .route_id = route.id,
                         .order_id = route.order_id,
                         .exec_id = exec_id,
                         .status = types::ExecStatus::NEW};
        uuid::generate_random_obj(fill.data, types::DATA_SIZE);
        return client_.state_.addFillForRoute(std::move(fill), route.id);
      });
}
} // namespace app
} // namespace omscompare

#endif