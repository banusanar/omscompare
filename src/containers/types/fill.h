#ifndef OMSCOMPARE_OMSTYPES_FILL_H_
#define OMSCOMPARE_OMSTYPES_FILL_H_

#include "basket.h"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <system_error>

namespace omscompare {
namespace types {

enum class ExecStatus {
  NEW,
  CANCEL,
  CORRECT,

};

struct Fill {
  IdType id;
  IdType route_id;
  IdType order_id;
  ExecStatus status;
  std::optional<IdType>
      original_id; // If this is a corrected fill, what was the original id
  std::byte data[1024];
};

} // namespace types
} // namespace omscompare

#endif