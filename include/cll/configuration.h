#pragma once

#include <nlohmann/json.hpp>
#include <chrono>

namespace cll {

class ConfigurationCache;

template <typename T>
class ConfigurationProperty {

private:
    T value;
    bool valueSet;

public:
    void reset() {
        value = T();
        valueSet = false;
    }

    void set(T value) {
        this->value = value;
        valueSet = true;
    }

    void set(nlohmann::json const& json, std::string const& name);

    T const& get() const {
        return value;
    }

    bool isSet() const {
        return valueSet;
    }

};

class Configuration {

public:
    std::string const url;
    bool downloaded = false;
    std::chrono::system_clock::time_point expires;
    ConfigurationProperty<int> maxEventSizeInBytes;
    ConfigurationProperty<int> maxEventsPerPost;
    ConfigurationProperty<int> queueDrainInterval;

    explicit Configuration(std::string url) : url(std::move(url)) {}

    bool download(ConfigurationCache* cache);

    bool needsRedownload() const {
        return !downloaded ||
                std::chrono::system_clock::now() >= expires;
    }

private:
    void applyFromJson(nlohmann::json const& json);

    static nlohmann::json safeParseJson(std::string const& str);

};

}