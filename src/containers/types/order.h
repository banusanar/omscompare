
#ifndef OMSCOMPARE_OMSTYPES_ORDER_H_
#define OMSCOMPARE_OMSTYPES_ORDER_H_

#include "basket.h"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace omscompare {
namespace types {

struct Order {
  IdType id;
  IdType parent_order_id;
  std::optional<IdType> basket_id;
  std::byte data[1024];
};

} // namespace types
} // namespace omscompare

#endif