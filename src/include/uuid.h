#include <string>

namespace omscompare {
namespace uuid {
std::string generate_uuid_v4();
void generate_random_obj(std::byte *data, int size);

} // namespace uuid
} // namespace omscompare