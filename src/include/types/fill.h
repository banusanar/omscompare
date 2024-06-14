#ifndef OMSCOMPARE_OMSTYPES_FILL_H_
#define OMSCOMPARE_OMSTYPES_FILL_H_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <system_error>
#include <types/idtype.h>

namespace omscompare {
namespace types {

enum class ExecStatus : int8_t {
  NEW = 0,
  PARTIAL_FILL = 1,
  FILL = 2,
  DONE_FOR_DAY = 3,
  CANCELED = 4,
  REPLACED = 5,
  PENDING_CANCEL = 6,
  STOPPED = 7,
  REJECTED = 8,
  SUSPENDED = 9,
  PENDING_NEW = 'A',
  CALCULATED = 'B',
  EXPIRED = 'C'
};

struct Fill {
  IdType id;
  IdType route_id;
  IdType order_id;
  FixExecIdType exec_id;
  ExecStatus status;
  std::optional<IdType>
      original_id; // If this is a corrected fill, what was the original id
  std::byte data[1024];
};

} // namespace types
} // namespace omscompare

#endif