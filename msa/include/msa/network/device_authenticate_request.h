#pragma once

#include "security_token_request.h"
#include "../legacy_token.h"

namespace msa {
namespace network {

struct DeviceAuthenticateResponse;

class DeviceAuthenticateRequest : public SecurityTokenRequest<DeviceAuthenticateResponse> {

protected:
    void buildHeaderSecurity(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& header,
                             XMLSignContext& signContext) const override;

    void buildBody(rapidxml::xml_document<char>& doc, rapidxml::xml_node<char>& body,
                   XMLSignContext& signContext) const override;

    DeviceAuthenticateResponse handleResponse(SecurityTokenResponse const& resp) const override;

public:
    std::string username, password;

    DeviceAuthenticateRequest(std::string username, std::string password)
            : username(std::move(username)), password(std::move(password)) {}

};

struct DeviceAuthenticateResponse {

    std::shared_ptr<LegacyToken> token;

};

}
}