
#ifndef OMSCOMPARE_OMSTYPES_ORDER_H_
#define OMSCOMPARE_OMSTYPES_ORDER_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>
#include "basket.h"

namespace omscompare {
    namespace omstypes {


struct Order {
    IdType id;
    IdType parent_order_id;
    std::optional<IdType> basket_id;
    std::byte data[1024];
};

    }
}

#endif