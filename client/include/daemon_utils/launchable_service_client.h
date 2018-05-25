#pragma once

#include <simpleipc/client/service_client.h>
#include "daemon_launcher.h"

namespace daemon_utils {

class launchable_service_client : public simpleipc::client::service_client {

private:
    daemon_launcher& launcher;
    std::atomic_bool shutting_down;

public:
    explicit launchable_service_client(daemon_launcher& launcher) : service_client(
            simpleipc::client::service_client_impl_factory::create_platform_service()), launcher(launcher),
                                                                    shutting_down(false) {
        get_impl().set_callback_interface(&get_cb_interface());
        launcher.open(get_impl());
        wait_for_hello_message();

    }

    ~launchable_service_client() override {
        shutting_down = true;
    }

protected:
    void connection_closed() override {
        service_client::connection_closed();
        if (!shutting_down)
            launcher.open(get_impl());
    }

};

}