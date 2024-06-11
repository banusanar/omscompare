#ifndef OMSCOMPARE_OMSTYPES_ERROR_H_
#define OMSCOMPARE_OMSTYPES_ERROR_H_
#include <string>

namespace omscompare {
namespace types {

struct Error {
  int rcode{0};
  std::string what;
};
} // namespace types
} // namespace omscompare

#endif