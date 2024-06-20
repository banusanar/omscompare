#include "client.h"
#include "metrics.h"
#include <atomic>
#include <optional>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>
#include <types/error.h>
#include <types/idtype.h>

namespace omscompare {
namespace app {

class WorkFlow {
public:
  WorkFlow(std::string wf_name, Client &client);
  ~WorkFlow();

  tl::expected<types::IdType, types::Error> createOrder(const std::string &clord_id,
                                                        std::optional<types::IdType> &basket_id);

  tl::expected<types::IdType, types::Error>
  createChildOrder(const std::string &clord_id, const std::string &parent_clord_id,
                   std::optional<types::IdType> &basket_id);

  tl::expected<types::IdType, types::Error> createBasket(const std::string &basket_name);

  tl::expected<types::IdType, types::Error> routeOrder(const types::IdType order_id,
                                                       const std::string &broker);

  tl::expected<void, types::Error> ackRoute(const types::IdType route_id);

  tl::expected<types::IdType, types::Error>
  createNewManualFillForRoute(const types::IdType route_id);

  tl::expected<types::IdType, types::Error>
  addFillForRoute(const types::FixClOrdIdType &route_clordid, const types::FixClOrdIdType &exec_id);

  std::shared_ptr<const model::ClientStateBase> clientRO() const { return client_.state_; }

  struct Scope {
    Scope(model::Metrics &m);
    ~Scope();

    model::Metrics &m;
  };

private:
  std::string workflow_name;
  Client &client_;
  model::Metrics metric_;
  std::atomic_bool is_success{false};
  //
  void sendToDownstreamSubscribers(){};
  int writeTransactionToPersistentDb() { return 0; };
};

} // namespace app
} // namespace omscompare
