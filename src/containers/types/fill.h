#ifndef OMSCOMPARE_OMSTYPES_FILL_H_
#define OMSCOMPARE_OMSTYPES_FILL_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>
#include <system_error>
#include "basket.h"

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
    std::optional<IdType> original_id; //If this is a corrected fill, what was the original id
    std::byte data[1024];
};

    }
}

#endif