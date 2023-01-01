#include "system_properties.h"

#include <unistd.h>
#include <stdexcept>

void shim::add_system_properties_shimmed_symbols(std::vector<shim::shimmed_symbol> &list) {
    list.push_back({"__system_property_find", __system_property_find});
}

const prop_info* shim::__system_property_find(const char *name) {
    return 0;
}
