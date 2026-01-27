#pragma once
#ifndef NODES_HXX
#define NODES_HXX

#include "types.hxx"
#include "package.hxx"
#include "storage_types.hxx"
#include "helpers.hxx"
#include <memory>
#include <map>
#include <optional>
#include <utility>
class IPackageReceiver {
public:
    virtual void receive_package(Package&& p) = 0;

    virtual ElementID get_id() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    // Always provide receiver type in this project configuration.
    virtual ReceiverType get_receiver_type() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver *, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator)
        : pg_(std::move(pg)) {}

    const_iterator cbegin() const { return preferences_.cbegin(); }
    const_iterator cend() const { return preferences_.cend(); }

    const_iterator begin() const { return preferences_.cbegin(); }
    const_iterator end() const { return preferences_.cend(); }

    void add_receiver(IPackageReceiver *r);
    void remove_receiver(IPackageReceiver *r);
    void remove_receiver(ElementID id);
    IPackageReceiver *choose_receiver();

    const preferences_t &get_preferences() const { return preferences_; }

    bool empty() const { return preferences_.empty(); }

private:
    preferences_t preferences_;
    ProbabilityGenerator pg_;
};

class PackageSender {
public:
    ReceiverPreferences receiver_preferences_;

    PackageSender() = default;
    PackageSender(PackageSender &&pack_sender) = default;

    void send_package();

    const std::optional<Package> &get_sending_buffer() const { return bufor_; }

protected:
    void push_package(Package &&package) { bufor_.emplace(package.get_id()); };

private:
    std::optional<Package> bufor_ = std::nullopt;
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id,
               std::unique_ptr<IPackageStockpile> d =
                   std::make_unique<PackageQueue>(PackageQueueType::FIFO))
        : id_(id), d_(std::move(d)) {}

    void receive_package(Package &&p) override;

    ElementID get_id() const override { return id_; }

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }

    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }

    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }

    const IPackageStockpile& get_stock() const { return *d_; }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
};


class Worker : public IPackageReceiver, public PackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
        : PackageSender(), id_(id), pd_(pd), t_(0), q_(std::move(q)) {}

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return pd_; }
    TimeOffset get_processing_time() const { return pd_; }
    Time get_package_processing_start_time() const { return t_; }

    void receive_package(Package &&p) override;

    ElementID get_id() const override { return id_; }

    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }

    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }
    IPackageStockpile::const_iterator end() const override { return q_->end(); }

    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

    PackageQueueType get_queue_type() const { return q_->get_queue_type(); }
    const std::optional<Package>& get_processing_buffer() const { return bufor_; }
    const IPackageQueue* get_queue() const { return q_.get(); }

    // Ułatwienie konfiguracji połączeń – deleguje do ReceiverPreferences.
    void add_receiver(IPackageReceiver* receiver) { receiver_preferences_.add_receiver(receiver); }

private:
    ElementID id_;
    TimeOffset pd_;
    Time t_;
    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> bufor_ = std::nullopt;
};


class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di)
        : PackageSender(), id_(id), di_(di) {}

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const { return di_; }
    ElementID get_id() const { return id_; }

    // Ułatwienie konfiguracji sieci – przekazuje dalej do ReceiverPreferences.
    void add_receiver(IPackageReceiver* receiver) { receiver_preferences_.add_receiver(receiver); }

private:
    ElementID id_;
    TimeOffset di_;
};

#endif // NODES_HXX