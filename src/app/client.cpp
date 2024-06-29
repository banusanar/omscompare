#include "client.h"
#include <client_state_boost.h>
#include <client_state_boost_hashed.h>
#include <client_state_sqlite.h>
#include <stdexcept>

namespace omscompare {
namespace app {

void Client::init(Client::ContainerType type) {
  if (type == ContainerType::BOOST_ORDERED_INDEX) {
    state_ = std::make_shared<model::ClientStateBoost>(client_id_);
  } else if (type == ContainerType::BOOST_HASHED_INDEX) {
    state_ = std::make_shared<model::ClientStateBoostHashed>(client_id_);
  } else if (type == ContainerType::SQLite) {
    state_ = std::make_shared<model::ClientStateSqlite>(client_id_);
  } else {
    throw std::runtime_error("Invalid container type used");
  }
  client_type_ = type;
  is_ready_ = true;
  return;
}

// TODO
void Client::status() {}

} // namespace app
} // namespace omscompare