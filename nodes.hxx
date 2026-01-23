#ifndef NODES_HXX
#define NODES_HXX

#include "package.hxx"
#include "types.hxx"
#include "storage_types.hxx"
#include <memory>
#include <map>
#include <optional>
#include <utility>
#include <functional>

// UWAGA: Usunięto include helpers.hxx, aby uniknąć cyklu.
// Zamiast tego deklarujemy zmienną globalną ręcznie.
extern ProbabilityGenerator probability_generator;

class IPackageReceiver {
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual ReceiverType get_receiver_type() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator);

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const { return preferences_; }

    const_iterator begin() const { return preferences_.cbegin(); }
    const_iterator end() const { return preferences_.cend(); }
    const_iterator cbegin() const { return preferences_.cbegin(); }
    const_iterator cend() const { return preferences_.cend(); }

private:
    preferences_t preferences_;
    ProbabilityGenerator pg_;
};

class PackageSender {
public:
    ReceiverPreferences receiver_preferences_;

    PackageSender() = default;
    PackageSender(PackageSender&&) = default;

    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return sending_buffer_; }

protected:
    void push_package(Package&& package);
    std::optional<Package> sending_buffer_ = std::nullopt;
};

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di);

    void deliver_goods(Time t);
    TimeOffset get_delivery_interval() const { return di_; }
    ElementID get_id() const { return id_; }

private:
    ElementID id_;
    TimeOffset di_;
    Time t_;
};

class Worker : public IPackageReceiver, public PackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q);

    void do_work(Time t);
    TimeOffset get_processing_time() const { return pd_; }

    // Metody dostępowe dla raportów
    PackageQueueType get_queue_type() const { return q_->get_queue_type(); }
    const IPackageQueue& get_queue() const { return *q_; }
    const std::optional<Package>& get_processing_buffer() const { return processing_buffer_; }

    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }
    IPackageStockpile::const_iterator end() const override { return q_->end(); }

private:
    ElementID id_;
    TimeOffset pd_;
    Time t_;
    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> processing_buffer_ = std::nullopt;
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO));

    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }

    // Dla raportów
    const IPackageStockpile& get_stock() const { return *d_; }

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};

#endif // NODES_HXX