enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

class IPackageReceiver {
public:
    virtual ~IPackageReceiver() = default;
    virtual ReceiverType get_receiver_type() const = 0;
};
