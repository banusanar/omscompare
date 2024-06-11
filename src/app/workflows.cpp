#include "workflows.h"
#include <basket.h>
#include <fill.h>
#include <types/idtype.h>
#include <order.h>
#include <route.h>
#include <client_state.h>
#include <memory>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include <tl/expected.hpp>

namespace omscompare {
namespace app {

namespace {

using random_bytes_engine = std::independent_bits_engine<
    std::default_random_engine, CHAR_BIT, unsigned char>;
void generateRandomObj(std::byte* data, int size)
{
    using random_bytes_engine = std::independent_bits_engine<
    std::default_random_engine, CHAR_BIT, unsigned char>;
    random_bytes_engine rbe;
    std::generate_n(data, size, [rbe]() mutable { return std::byte(rbe());});
}

}

const int DATA_SIZE = 1024;

tl::expected<types::IdType, types::Error>
WorkFlow::createOrder(const std::string &clord_id,
                      std::optional<types::IdType> &basket_id) {
  types::Order order{.id = types::getNewOrderIdForClient(client_.client_id_),
                     .clord_id =
                         types::getNewClordIdForClient(client_.client_id_),
                     .parent_order_id = 0,
                     .basket_id = basket_id};
  generateRandomObj(order.data, DATA_SIZE);
  return client_.state_->addOrder(std::move(order));
}

tl::expected<types::IdType, types::Error>
WorkFlow::createChildOrder(const std::string &clord_id,
                           const std::string &parent_clord_id,
                           std::optional<types::IdType> &basket_id) {
  return client_.state_->findOrderByClordId(parent_clord_id)
      .and_then([&](types::Order parent_order)
                    -> tl::expected<types::IdType, types::Error> {
        types::Order order{
            .id = types::getNewOrderIdForClient(client_.client_id_),
            .clord_id = types::getNewClordIdForClient(client_.client_id_),
            .parent_order_id = parent_order.id,
            .basket_id = basket_id};
        generateRandomObj(order.data, DATA_SIZE);
        return client_.state_->addOrder(std::move(order));
      });
}

tl::expected<types::IdType, types::Error>
WorkFlow::createBasket(const std::string &basket_name) {
  types::Basket basket{.id = types::getNewBasketIdForClient(client_.client_id_),
                       .name = basket_name,
                       .is_active = true};
  return client_.state_->addBasket(std::move(basket));
}

tl::expected<types::IdType, types::Error>
WorkFlow::routeOrder(const types::IdType order_id, const std::string &broker) {
  types::Route route{.id = types::getNewRouteIdForClient(client_.client_id_),
                  .order_id = order_id,
                  .clord_id = types::getNewRouteClordIdForClient(client_.client_id_),
                  .status = types::RouteStatus::SENT_TO_BROKER,
                  .broker = broker
  };
  generateRandomObj(route.data, DATA_SIZE);
  return client_.state_->addRouteForOrder(std::move(route), order_id);
}

tl::expected<void, types::Error>
WorkFlow::ackRoute(const types::IdType route_id) {
  return client_.state_->findRoute(route_id)
      .and_then([&](types::Route route)
                    -> tl::expected<void, types::Error> {
        types::Route updroute{route};
        updroute.status = types::RouteStatus::BROKER_ACKNOWLEDGE;
        return client_.state_->updateRouteForOrder(std::move(updroute));
      });
}

tl::expected<types::IdType, types::Error>
WorkFlow::createNewManualFillForRoute(const types::IdType route_id) {
  return client_.state_->findRoute(route_id)
      .and_then([&](types::Route route)
                    -> tl::expected<types::IdType, types::Error> {
        types::Fill fill{
          .id = types::getNewFillIdForClient(client_.client_id_),
          .route_id = route.id,
          .order_id = route.order_id,
          .exec_id = types::getNewExecIdForClient(client_.client_id_, route.id),
          .status = types::ExecStatus::NEW
        };
        generateRandomObj(fill.data, DATA_SIZE);
        return client_.state_->addFillForRoute(std::move(fill), route_id);
      });
}

tl::expected<types::IdType, types::Error>
WorkFlow::addFillForRoute(const types::FixClOrdIdType &route_clordid,
                              const types::FixClOrdIdType &exec_id) {
  return client_.state_->findRouteByClordId(route_clordid)
        .and_then([&](types::Route route)
                      -> tl::expected<types::IdType, types::Error> {
          types::Fill fill{
            .id = types::getNewFillIdForClient(client_.client_id_),
            .route_id = route.id,
            .order_id = route.order_id,
            .exec_id = exec_id,
            .status = types::ExecStatus::NEW
          };
          generateRandomObj(fill.data, DATA_SIZE);
          return client_.state_->addFillForRoute(std::move(fill), route.id);
        });
}

} // namespace app
} // namespace omscompare
