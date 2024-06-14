#ifndef OMSCOMPARE_OMSMODEL_CLIENT_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <types/idtype.h>

namespace omscompare {
namespace model {
class ClientState;
}

namespace app {

class WorkFlow;

class Client {
public:
  explicit Client(types::ClientIdType clientid)
      : client_id_(clientid), is_ready_(false), state_lock_(), state_() {}

  Client() = delete;
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;
  // Client(Client&&) = delete;
  // Client& operator=(Client&&) = delete;

  void init(); // can throw

  bool is_ready() { return is_ready_; };
  void status();

  friend class WorkFlow;
private:
  types::ClientIdType client_id_;
  bool is_ready_;
  std::mutex state_lock_;
  std::shared_ptr<model::ClientState> state_;
};

} // namespace app
} // namespace omscompare

#endif