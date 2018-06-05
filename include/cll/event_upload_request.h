#pragma once

#include <memory>
#include <vector>
#include "event.h"

namespace cll {

struct EventUploadRequest {
    std::vector<std::pair<std::string, std::string>> headers;
    std::vector<Event> events;
};

class EventUploadResponse {
    int status;
};

}