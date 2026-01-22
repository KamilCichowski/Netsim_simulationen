#include "factory.hxx"

void Factory::add_ramp(Ramp&& r) {
    ramps_.add(std::move(r));
}

void Factory::remove_ramp(ElementID id) {
    ramps_.remove_by_id(id);
}

NodeCollection<Ramp>::iterator Factory::find_ramp_by_id(ElementID id) {
    return ramps_.find_by_id(id);
}

NodeCollection<Ramp>::const_iterator Factory::find_ramp_by_id(ElementID id) const {
    return ramps_.find_by_id(id);
}

NodeCollection<Ramp>::const_iterator Factory::ramp_cbegin() const {
    return ramps_.cbegin();
}

NodeCollection<Ramp>::const_iterator Factory::ramp_cend() const {
    return ramps_.cend();
}

NodeCollection<Worker>::const_iterator Factory::worker_cbegin() const {
    return workers_.cbegin();
}

NodeCollection<Worker>::const_iterator Factory::worker_cend() const {
    return workers_.cend();
}

NodeCollection<Storehouse>::const_iterator Factory::storehouse_cbegin() const {
    return storehouses_.cbegin();
}

NodeCollection<Storehouse>::const_iterator Factory::storehouse_cend() const {
    return storehouses_.cend();
}

NodeCollection<Storehouse>::iterator Factory::find_storehouse_by_id(ElementID id) {
    return storehouses_.find_by_id(id);
}

NodeCollection<Storehouse>::const_iterator Factory::find_storehouse_by_id(ElementID id) const {
    return storehouses_.find_by_id(id);
}

void Factory::add_worker(Worker&& w) {
    workers_.add(std::move(w));
}

void Factory::add_storehouse(Storehouse&& s) {
    storehouses_.add(std::move(s));
}

void Factory::remove_worker(ElementID id) {
    remove_receiver(workers_, id);
}

void Factory::remove_storehouse(ElementID id) {
    remove_receiver(storehouses_, id);
}

bool has_reachable_storehouse(
    const PackageSender* sender,
    std::map<const PackageSender*, NodeColor>& colors
) {
    if (colors[sender] == NodeColor::VERIFIED)
        return true;

    colors[sender] = NodeColor::VISITED;

    const auto& prefs = sender->receiver_preferences_;
    if (prefs.empty()) {
        throw std::logic_error("Sender has no receivers");
    }

    bool ok = false;

    for (const auto& [receiver, _] : prefs) {
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            ok = true;
        } else {
            auto worker = dynamic_cast<Worker*>(receiver);
            auto next = dynamic_cast<PackageSender*>(worker);

            if (next == sender) continue;

            ok = true;

            if (colors[next] == NodeColor::UNVISITED) {
                has_reachable_storehouse(next, colors);
            }
        }
    }

    colors[sender] = NodeColor::VERIFIED;

    if (!ok) {
        throw std::logic_error("No reachable storehouse");
    }

    return true;
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> colors;

    for (const auto& r : ramps_)
        colors[&r] = NodeColor::UNVISITED;

    for (const auto& w : workers_)
        colors[&w] = NodeColor::UNVISITED;

    try {
        for (const auto& r : ramps_) {
            has_reachable_storehouse(&r, colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}

void Factory::do_deliveries(Time t) {
    for (auto& r : ramps_) {
        r.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& r : ramps_) r.send_package();
    for (auto& w : workers_) w.send_package();
}

void Factory::do_work(Time t) {
    for (auto& w : workers_) {
        w.do_work(t);
    }
}