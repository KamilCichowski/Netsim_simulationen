#include "nodes.hxx"

void ReceiverPreferences::add_(IPackageReceiver* receiver) {
    const std::size_t old_count = preferences_.size();

    if (old_count == 0) {
        preferences_.emplace(receiver, 1.0);
        return;
    }

    const double new_prob = 1.0 / (old_count + 1);

    for (auto& entry : preferences_) {
        entry.second = new_prob;
    }

    preferences_.emplace(receiver, new_prob);
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver) {
    const std::size_t old_count = preferences_.size();

    if (old_count <= 1) {
        preferences_.erase(receiver);
        return;
    }

    const double updated_prob = 1.0 / (old_count - 1);

    for (auto& entry : preferences_) {
        if (entry.first != receiver) {
            entry.second = updated_prob;
        }
    }

    preferences_.erase(receiver);
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    const double p = pg_();

    if (p < 0.0 || p > 1.0) {
        return nullptr;
    }

    double cumulative = 0.0;

    for (auto& entry : preferences_) {
        cumulative += entry.second;

        if (cumulative < 0.0 || cumulative > 1.0) {
            return nullptr;
        }

        if (p <= cumulative) {
            return entry.first;
        }
    }

    return nullptr;
}

void PackageSender::send_package() {
    if (!bufor_) {
        return;
    }
    IPackageReceiver* chosen = receiver_preferences_.choose_receiver();

    if (chosen) {
        chosen->receive_package(std::move(*bufor_));
    }

    bufor_.reset();
}

void Worker::do_work(Time current) {
    if (!bufor_ && !q_->empty()) {
        bufor_.emplace(q_->pop());
        t_ = current;
        return;
    }

    if (current - t_ + 1 == pd_) {
        push_package(Package(bufor_->get_id()));
        bufor_.reset();

        if (!q_->empty()) {
            bufor_.emplace(q_->pop());
        }
    }
}

void Worker::receive_package(Package&& pkg) {
    q_->push(std::move(pkg));
}

void Storehouse::receive_package(Package&& pkg) {
    d_->push(std::move(pkg));
}

void Ramp::deliver_goods(Time current) {
    if (!bufor_) {
        push_package(Package());
        bufor_.emplace(id_);
        t_ = current;
        return;
    }

    if (current - di_ == t_) {
        push_package(Package());
    }
}
