#pragma once

#include <string>
#include <functional>

using ElementID = int;

// Time-related aliases used in the simulation.
using Time = int;
using TimeOffset = int;

// Probability generator used by ReceiverPreferences.
using ProbabilityGenerator = std::function<double()>;

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

// Type of receiver node in the network.
enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

// Colors used when checking reachability in Factory::is_consistent().
enum class NodeColor {
    UNVISITED,
    VISITED,
    VERIFIED
};
