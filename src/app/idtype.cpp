#include <types/idtype.h>
#include "uuid.h"
#include <map>

namespace omscompare {
namespace types {

namespace {
enum BaseIdType { ORDER, ROUTE, BASKET, FILL };
std::map<BaseIdType, int64_t> sequences{{BaseIdType::ORDER, 1},
                                        {BaseIdType::BASKET, 100},
                                        {BaseIdType::ROUTE, 1000},
                                        {BaseIdType::FILL, 10000}};
IdType getNew(ClientIdType, BaseIdType type) { return sequences[type]++; }
} // namespace

IdType getNewBasketIdForClient(ClientIdType x) {
  return getNew(x, BaseIdType::BASKET);
}

IdType getNewOrderIdForClient(ClientIdType x) {
  return getNew(x, BaseIdType::ORDER);
}

FixClOrdIdType getNewClordIdForClient(ClientIdType) {
  return uuid::generate_uuid_v4();
}

IdType getNewRouteIdForClient(ClientIdType x) {
  return getNew(x, BaseIdType::ROUTE);
}
FixClOrdIdType getNewRouteClordIdForClient(ClientIdType) {
  return uuid::generate_uuid_v4();
}

IdType getNewFillIdForClient(ClientIdType x) {
  return getNew(x, BaseIdType::FILL);
}

FixExecIdType getNewExecIdForClient(ClientIdType client_id, IdType route_id) {
  return uuid::generate_uuid_v4();
}
} // namespace types
} // namespace omscompare