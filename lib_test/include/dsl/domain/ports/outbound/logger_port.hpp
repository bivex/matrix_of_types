#pragma once

#include <string>

namespace dsl::domain::ports::outbound {

class ILoggerPort {
public:
    virtual ~ILoggerPort() = default;
    virtual void info(const std::string& message) = 0;
    virtual void debug(const std::string& message) = 0;
    virtual void warn(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
};

} // namespace dsl::domain::ports::outbound
