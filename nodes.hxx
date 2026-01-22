enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

enum class NodeColor {
    UNVISITED,
    VISITED,
    VERIFIED
};

class IPackageReceiver {
public:
    virtual ~IPackageReceiver() = default;
    virtual ReceiverType get_receiver_type() const = 0;
};



