#pragma once

#include "token.h"

namespace msa {

class CompactToken : public Token {

private:
    std::string binaryToken;

public:
    CompactToken(std::string const& binaryToken) : binaryToken(binaryToken) { }

    CompactToken(SecurityScope const& scope, ExpireTime expire, std::string const& binaryToken) :
            Token(scope, expire), binaryToken(binaryToken) { }

    CompactToken(rapidxml::xml_node<char> const& data);

    TokenType getType() const { return TokenType::Compact; }

    std::string const& getBinaryToken() const { return binaryToken; }

};

}