#include <msa/client/service_client.h>

using namespace msa::client;

simpleipc::client::rpc_call<std::vector<BaseAccountInfo>> ServiceClient::getAccounts() {
    return simpleipc::client::rpc_call<std::vector<BaseAccountInfo>>(
            rpc("msa/get_accounts", nullptr), [](nlohmann::json const& d) {
                std::vector<BaseAccountInfo> ret;
                for (auto const& a : d["accounts"])
                    ret.push_back(BaseAccountInfo::fromJson(a));
                return ret;
            });
}

simpleipc::client::rpc_call<std::string> ServiceClient::pickAccount(std::string const& clientId,
                                                                    const std::string& cobrandId) {
    nlohmann::json data;
    if (!clientId.empty())
        data["client_id"] = clientId;
    if (!cobrandId.empty())
        data["cobrandid"] = cobrandId;
    return simpleipc::client::rpc_call<std::string>(
            rpc("msa/pick_account", data), [](nlohmann::json const& d) {
                return d["cid"].get<std::string>();
            });
}