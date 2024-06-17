#ifndef OMSCOMPARE_TYPES_IDTYPE_H_
#define OMSCOMPARE_TYPES_IDTYPE_H_

#include <string>

namespace omscompare {
namespace types {

using ClientIdType = uint64_t;
using IdType = uint64_t;
using FixClOrdIdType = std::string;
using FixExecIdType = std::string;

const int DATA_SIZE = 256;

IdType getNewBasketIdForClient(ClientIdType);
IdType getNewOrderIdForClient(ClientIdType);
FixClOrdIdType getNewClordIdForClient(ClientIdType);

IdType getNewRouteIdForClient(ClientIdType);
FixClOrdIdType getNewRouteClordIdForClient(ClientIdType);

IdType getNewFillIdForClient(ClientIdType);
FixExecIdType getNewExecIdForClient(ClientIdType client_id, IdType route_id);

} // namespace types
} // namespace omscompare

#endif