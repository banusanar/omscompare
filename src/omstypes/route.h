#ifndef OMSCOMPARE_OMSTYPES_ROUTE_H_
#define OMSCOMPARE_OMSTYPES_ROUTE_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>
#include "basket.h"

namespace omscompare {
    namespace omstypes {

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

    }
}

#endif