#pragma once

#include "../../domain/ports/outbound/logger_port.hpp"
#include <iostream>
#include <mutex>

namespace dsl::adapters::outbound {

class ConsoleLoggerAdapter final : public domain::ports::outbound::ILoggerPort {
public:
    void info(const std::string& message) override {
        log("[INFO]  ", message);
    }

    void debug(const std::string& message) override {
        log("[DEBUG] ", message);
    }

    void warn(const std::string& message) override {
        log("[WARN]  ", message);
    }

    void error(const std::string& message) override {
        log("[ERROR] ", message);
    }

private:
    void log(const char* level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        std::cout << level << msg << "\n";
    }

    std::mutex mtx_;
};

} // namespace dsl::adapters::outbound
