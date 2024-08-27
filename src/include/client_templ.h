#ifndef OMSCOMPARE_OMSMODEL_CLIENT_TEMPL_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_TEMPL_H_

#include "client_state_templ.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <types/idtype.h>

namespace omscompare {
namespace app {

template <typename C> class WorkFlowTempl;

template <typename CONTAINER_TYPE> class ClientTempl {
public:
  explicit ClientTempl(types::ClientIdType clientid)
      : client_id_(clientid), state_lock_(), state_() {}

  ClientTempl() = delete;
  ClientTempl(const ClientTempl &) = delete;
  ClientTempl &operator=(const ClientTempl &) = delete;
  // Client(Client&&) = delete;
  // Client& operator=(Client&&) = delete;

  void status() {}

  friend class WorkFlowTempl<CONTAINER_TYPE>;

private:
  types::ClientIdType client_id_;
  bool is_ready_;
  std::mutex state_lock_;
  model::ClientState<CONTAINER_TYPE> state_;
};

} // namespace app
} // namespace omscompare

#endif