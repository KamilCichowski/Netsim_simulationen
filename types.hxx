#pragma once

#include <string>

using ElementID = int;

enum class PackageQueueType {
    FIFO,
    LIFO
};

inline std::string to_string(PackageQueueType type) {
    switch (type) {
        case PackageQueueType::FIFO: return "FIFO";
        case PackageQueueType::LIFO: return "LIFO";
    }
    return "";
}
