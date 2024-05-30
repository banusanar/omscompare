#include <client.h>
#include <client_state.h>

namespace omscompare {
namespace model {

void Client::init() {
  state_ = std::make_shared<ClientState>();
  is_ready_ = true;
  return;
}

void Client::status() { state_->status(); }

} // namespace model
} // namespace omscompare