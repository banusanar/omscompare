#ifndef OMSCOMPARE_OMSMODEL_CLIENT_H_
#define OMSCOMPARE_OMSMODEL_CLIENT_H_

#include <cstdint>
#include <memory>
#include <mutex>

namespace omscompare {
    namespace omsmodel {

class ClientState;

using ClientIdType = uint64_t;

class Client {
    public:
        explicit Client(ClientIdType clientid);

        Client() = delete;
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;
        // Client(Client&&) = delete;
        // Client& operator=(Client&&) = delete;

        void init();    // can throw
        bool is_ready();
        void status();

    private:
        ClientIdType client_id_;
        bool is_ready_;
        std::mutex state_lock_;
        std::shared_ptr<ClientState> state_;
};


    }
}

#endif