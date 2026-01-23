#include "nodes.hxx"

// --- ReceiverPreferences ---

ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg)
    : pg_(std::move(pg)) {}

void ReceiverPreferences::add_receiver(IPackageReceiver* receiver) {
    double old_count = static_cast<double>(preferences_.size());
    if (old_count == 0) {
        preferences_.emplace(receiver, 1.0);
        return;
    }
    double new_prob = 1.0 / (old_count + 1);
    for (auto& entry : preferences_) {
        entry.second = new_prob;
    }
    preferences_.emplace(receiver, new_prob);
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver) {
    auto it = preferences_.find(receiver);
    if (it == preferences_.end()) return;

    preferences_.erase(it);
    double count = static_cast<double>(preferences_.size());
    if (count > 0) {
        double new_prob = 1.0 / count;
        for (auto& entry : preferences_) {
            entry.second = new_prob;
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double p = pg_();
    if (p < 0.0 || p > 1.0) return nullptr;

    double cumulative = 0.0;
    for (auto& entry : preferences_) {
        cumulative += entry.second;
        if (p <= cumulative) {
            return entry.first;
        }
    }
    // Fallback dla błędów zaokrągleń
    if (!preferences_.empty()) return preferences_.begin()->first;
    return nullptr;
}

// --- PackageSender ---

void PackageSender::push_package(Package&& package) {
    sending_buffer_.emplace(std::move(package));
}

void PackageSender::send_package() {
    if (sending_buffer_) {
        IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
        if (receiver) {
            receiver->receive_package(std::move(*sending_buffer_));
            sending_buffer_.reset();
        }
    }
}

// --- Ramp ---

Ramp::Ramp(ElementID id, TimeOffset di)
    : id_(id), di_(di), t_(0) {}

void Ramp::deliver_goods(Time current) {
    if (current % di_ == 1) { // Prosta logika cyklu: start w t=1, potem co 'di_'
        push_package(Package());
    }
}

// --- Worker ---

Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
    : id_(id), pd_(pd), t_(0), q_(std::move(q)) {}

void Worker::do_work(Time current) {
    // 1. Jeśli nie pracuję i mam coś w kolejce, biorę do roboty
    if (!processing_buffer_ && !q_->empty()) {
        processing_buffer_.emplace(q_->pop());
        t_ = current;
    }

    // 2. Jeśli pracuję i czas minął
    if (processing_buffer_) {
        if (current - t_ + 1 >= pd_) {
            push_package(std::move(*processing_buffer_)); // Przesuń do bufora wysyłkowego
            processing_buffer_.reset();

            // Od razu spróbuj wziąć następną, jeśli jest (opcjonalnie)
            if (!q_->empty()) {
                processing_buffer_.emplace(q_->pop());
                t_ = current;
            }
        }
    }
}

void Worker::receive_package(Package&& pkg) {
    q_->push(std::move(pkg));
}

// --- Storehouse ---

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d)
    : id_(id), d_(std::move(d)) {}

void Storehouse::receive_package(Package&& pkg) {
    d_->push(std::move(pkg));
}