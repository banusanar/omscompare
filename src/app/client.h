#ifndef OMSCOMPARE_OMSMODEL_CLIENT_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_H_

#include <cstdint>
#include <memory>
#include <mutex>

namespace omscompare {
namespace model {

class ClientState;

using ClientIdType = uint64_t;

class Client {
public:
  explicit Client(ClientIdType clientid)
      : client_id_(clientid), is_ready_(false), state_lock_(), state_() {}

  Client() = delete;
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;
  // Client(Client&&) = delete;
  // Client& operator=(Client&&) = delete;

  void init(); // can throw

  bool is_ready() { return is_ready_; };
  void status();

  template <typename REQUEST, typename RESPONSE>
  std::unique_ptr<RESPONSE> processRequest(std::shared_ptr<REQUEST> request);

private:
  ClientIdType client_id_;
  bool is_ready_;
  std::mutex state_lock_;
  std::shared_ptr<ClientState> state_;
};

} // namespace model
} // namespace omscompare

#endif