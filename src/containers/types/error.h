
#include <string>

namespace omscompare {
    namespace types {

struct Error {
  int rcode{0};
  std::string what;
};
    }
}