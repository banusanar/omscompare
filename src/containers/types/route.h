#ifndef OMSCOMPARE_OMSTYPES_ROUTE_H_
#define OMSCOMPARE_OMSTYPES_ROUTE_H_

#include "basket.h"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace omscompare {
namespace types {

enum class RouteStatus {
  SENT,
  ACK,
  P_FILLED,
  FILLED,
  CANCEL,
  REJECTED,
};

struct Route {
  IdType id;
  IdType order_id;
  RouteStatus status;
  std::byte data[1024];
};

} // namespace types
} // namespace omscompare

#endif