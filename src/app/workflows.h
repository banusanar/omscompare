#include "client.h"
#include "idtype.h"
#include <atomic>
#include <optional>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <types/error.h>

namespace omscompare {
namespace app {

class WorkFlow {
public:
  tl::expected<types::IdType, types::Error>
  createOrder(const std::string &clord_id,
              std::optional<types::IdType> &basket_id);

  tl::expected<types::IdType, types::Error>
  createChildOrder(const std::string &clord_id,
                   const std::string &parent_clord_id,
                   std::optional<types::IdType> &basket_id);

  tl::expected<types::IdType, types::Error>
  createBasket(const std::string &basket_name);

  tl::expected<types::IdType, types::Error>
  routeOrder(const types::IdType order_id, const std::string &broker);

  tl::expected<void, types::Error> ackRoute(const types::IdType route_id);

  tl::expected<types::IdType, types::Error>
  sendNewFillForRoute(const types::IdType route_id, const std::string &exec_id);

  tl::expected<types::IdType, types::Error>
  sendNewFillForRoute(const types::FixClOrdIdType &route_clordid,
                      const types::FixClOrdIdType &exec_id);

  WorkFlow(Client &client) : client_(client) {
    if (!client_.is_ready()) {
      throw std::runtime_error(
          "Cannot initiate workflow for this client. Not ready");
    }
  }

private:
  Client &client_;
  std::atomic_bool is_success{false};
  //
  void sendToDownstreamSubscribers(){};
  int writeTransactionToPersistentDb() { return 0; };
};

} // namespace app
} // namespace omscompare
