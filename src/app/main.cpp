#include <client.h>
#include <iostream>
#include <types/idtype.h>
#include <workflows.h>

int main() {
  using namespace omscompare;
  try {
    types::ClientIdType clientid = 1001;
    app::Client client(clientid);
    app::WorkFlow n1(client);

  } catch (const std::exception &ae) {
    std::cerr << "Caught exception [" << ae.what() << "]" << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;
    return -1;
  }

  return 0;
}