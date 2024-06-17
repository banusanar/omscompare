#include "client.h"
#include <client_state.h>

namespace omscompare {
namespace app {

void Client::init() {
  state_ = std::make_shared<model::ClientState>();
  is_ready_ = true;
  return;
}

//TODO
void Client::status() { }

} // namespace app
} // namespace omscompare