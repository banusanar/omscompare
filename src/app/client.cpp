#include "client.h"
#include <client_state_boost.h>
#include <client_state_sqlite.h>

namespace omscompare {
namespace app {

void Client::init(Client::ContainerType type) {
  if (type == ContainerType::BOOST) {
    state_ = std::make_shared<model::ClientState>(client_id_);
  } else {
    state_ = std::make_shared<model::ClientStateSqlite>(client_id_);
  }
  is_ready_ = true;
  return;
}

// TODO
void Client::status() {}

} // namespace app
} // namespace omscompare