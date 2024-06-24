#include "client.h"
#include "metrics.h"
#include <atomic>
#include <memory>
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
  WorkFlow(std::string wf_name, std::shared_ptr<Client> client);
  ~WorkFlow();

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
  std::shared_ptr<Client> client_;
  model::Metrics metric_;
  std::atomic_bool is_success{false};
  //
  void sendToDownstreamSubscribers(){};
  int writeTransactionToPersistentDb() { return 0; };
};

} // namespace app
} // namespace omscompare
