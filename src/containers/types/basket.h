#ifndef OMSCOMPARE_OMSTYPES_BASKET_H_
#define OMSCOMPARE_OMSTYPES_BASKET_H_
#include <string>

namespace omscompare {
namespace types {

using IdType = uint64_t;

struct Basket {
  IdType id;
  std::string name;
  bool is_active;
};

} // namespace types
} // namespace omscompare

#endif