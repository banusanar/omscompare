#include "client.h"
#include "client_state_base.h"
#include <client_state.h>

namespace omscompare {
namespace app {

void Client::init(Client::ContainerType type) {
  if(type == ContainerType::BOOST) {
    state_ = std::make_shared<model::ClientState>();
  }
  else {
    //TODO
  }
  is_ready_ = true;
  return;
}

//TODO
void Client::status() { }

} // namespace app
} // namespace omscompare