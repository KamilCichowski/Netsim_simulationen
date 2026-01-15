#include "factory.hxx"

bool Factory::is_consistent() const {
    return true;  // tymczasowo
}

void Factory::do_deliveries(Time t) {
    for (auto& ramp : ramps_) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& worker : workers_) {
        worker.send_package();
    }
}

void Factory::do_work(Time t) {
    for (auto& worker : workers_) {
        worker.do_work(t);
    }
}


bool Factory::has_reachable_storehouse(
    const PackageSender* sender,
    std::map<const PackageSender*, NodeColor>& node_colors
) const {
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    const auto& receivers = sender->receiver_preferences_.get_preferences();
    if (receivers.empty()) {
        throw std::logic_error("Sender has no receivers");
    }

    bool has_valid_receiver = false;

    for (const auto& [receiver, _] : receivers) {
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_valid_receiver = true;
        } else {
            auto worker_ptr = dynamic_cast<Worker*>(receiver);
            auto sender_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if (sender_ptr == sender) {
                continue;
            }

            has_valid_receiver = true;

            if (node_colors[sender_ptr] == NodeColor::UNVISITED) {
                has_reachable_storehouse(sender_ptr, node_colors);
            }
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (!has_valid_receiver) {
        throw std::logic_error("No reachable storehouse");
    }

    return true;
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> node_colors;

    for (const auto& ramp : ramps_) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto& worker : workers_) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }

    try {
        for (const auto& ramp : ramps_) {
            has_reachable_storehouse(&ramp, node_colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}
