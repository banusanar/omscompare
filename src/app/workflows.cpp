#include "workflows.h"
#include "basket.h"
#include "idtype.h"
#include "order.h"
#include <client_state.h>
#include <tl/expected.hpp>

namespace omscompare {
namespace app {

namespace {}

tl::expected<types::IdType, types::Error>
WorkFlow::createOrder(const std::string &clord_id,
                      std::optional<types::IdType> &basket_id) {
  types::Order order{.id = types::getNewOrderIdForClient(client_.client_id_),
                     .clord_id =
                         types::getNewClordIdForClient(client_.client_id_),
                     .parent_order_id = 0,
                     .basket_id = basket_id};
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
WorkFlow::routeOrder(const types::IdType order_id, const std::string &broker);

tl::expected<void, types::Error>
WorkFlow::ackRoute(const types::IdType route_id);

tl::expected<types::IdType, types::Error>
WorkFlow::sendNewFillForRoute(const types::IdType route_id,
                              const std::string &exec_id);

tl::expected<types::IdType, types::Error>
WorkFlow::sendNewFillForRoute(const types::FixClOrdIdType &route_clordid,
                              const types::FixClOrdIdType &exec_id);

} // namespace app
} // namespace omscompare
