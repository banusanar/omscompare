#ifndef OMSCOMPARE_OMSTYPES_ROUTE_H_
#define OMSCOMPARE_OMSTYPES_ROUTE_H_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <types/idtype.h>

namespace omscompare {
namespace types {

enum class RouteStatus : int8_t {
  NEW = 0,
  PARTIALLY_FILLED = 1,
  FILLED = 2,
  DONE_FOR_DAY = 3,
  CANCELED = 4,
  REPLACED = 5,
  PENDING_CANCEL = 6,
  STOPPED = 7,
  REJECTED = 8,
  SUSPENDED = 9,
  PENDING_NEW = 'A',
  CALCULATED = 'B',
  EXPIRED = 'C',
  ACCEPTED_FOR_BIDDING = 'D',
  PENDING_REPLACE = 'E',
  SENT_TO_BROKER = 99,
  BROKER_ACKNOWLEDGE = 100
};

struct Route {
  IdType id;
  IdType order_id;
  FixClOrdIdType clord_id; // outgoing
  RouteStatus status;
  std::string broker;
  std::byte data[1024];
};

} // namespace types
} // namespace omscompare

#endif