#include "idtype.h"
#include <optional>
#include <types/error.h>
#include <string>
#include <tl/expected.hpp>

namespace omscompare {
    namespace app {
namespace workflows {

tl::expected< types::IdType, types::Error> createOrder(const std::string& clord_id,
    std::optional<types::IdType>& basket_id);

tl::expected< types::IdType, types::Error> createChildOrder(const std::string& clord_id,
    const std::string& parent_clord_id,
    std::optional<types::IdType>& basket_id);

tl::expected< types::IdType, types::Error> createBasket(const std::string& basket_name);

tl::expected< types::IdType, types::Error> routeOrder(const types::IdType order_id, 
    const std::string& broker);

tl::expected< void, types::Error> ackRoute(const types::IdType route_id);

tl::expected< types::IdType, types::Error> sendNewFillForRoute(const types::IdType route_id,
    const std::string& exec_id);

tl::expected< types::IdType, types::Error> sendNewFillForRoute(const types::FixClOrdIdType& route_clordid,
    const types::FixClOrdIdType& exec_id);

}
    }
}

