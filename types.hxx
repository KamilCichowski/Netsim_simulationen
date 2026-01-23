#ifndef TYPES_HXX
#define TYPES_HXX

#include <functional>
#include <string>

using ElementID = int;
using Time = int;
using TimeOffset = int;

using ProbabilityGenerator = std::function<double()>;

enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

enum class PackageQueueType {
    FIFO,
    LIFO
};

enum class NodeColor {
    UNVISITED,
    VISITED,
    VERIFIED
};

inline std::string to_string(PackageQueueType type) {
    switch (type) {
        case PackageQueueType::FIFO: return "FIFO";
        case PackageQueueType::LIFO: return "LIFO";
    }
    return "";
}

#endif // TYPES_HXX