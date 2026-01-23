#include "factory.hxx"
#include <map>

// Implementacja metod prostych
void Factory::add_ramp(Ramp&& r) { ramps_.add(std::move(r)); }
void Factory::remove_ramp(ElementID id) { ramps_.remove_by_id(id); }
NodeCollection<Ramp>::iterator Factory::find_ramp_by_id(ElementID id) { return ramps_.find_by_id(id); }
NodeCollection<Ramp>::const_iterator Factory::find_ramp_by_id(ElementID id) const { return ramps_.find_by_id(id); }
NodeCollection<Ramp>::const_iterator Factory::ramp_cbegin() const { return ramps_.cbegin(); }
NodeCollection<Ramp>::const_iterator Factory::ramp_cend() const { return ramps_.cend(); }

void Factory::add_worker(Worker&& w) { workers_.add(std::move(w)); }
void Factory::remove_worker(ElementID id) { remove_receiver(workers_, id); }
NodeCollection<Worker>::iterator Factory::find_worker_by_id(ElementID id) { return workers_.find_by_id(id); }
NodeCollection<Worker>::const_iterator Factory::find_worker_by_id(ElementID id) const { return workers_.find_by_id(id); }
NodeCollection<Worker>::const_iterator Factory::worker_cbegin() const { return workers_.cbegin(); }
NodeCollection<Worker>::const_iterator Factory::worker_cend() const { return workers_.cend(); }

void Factory::add_storehouse(Storehouse&& s) { storehouses_.add(std::move(s)); }
void Factory::remove_storehouse(ElementID id) { remove_receiver(storehouses_, id); }
NodeCollection<Storehouse>::iterator Factory::find_storehouse_by_id(ElementID id) { return storehouses_.find_by_id(id); }
NodeCollection<Storehouse>::const_iterator Factory::find_storehouse_by_id(ElementID id) const { return storehouses_.find_by_id(id); }
NodeCollection<Storehouse>::const_iterator Factory::storehouse_cbegin() const { return storehouses_.cbegin(); }
NodeCollection<Storehouse>::const_iterator Factory::storehouse_cend() const { return storehouses_.cend(); }

// Logika biznesowa
void Factory::do_deliveries(Time t) {
    for (auto& r : ramps_) r.deliver_goods(t);
}

void Factory::do_package_passing() {
    for (auto& r : ramps_) r.send_package();
    for (auto& w : workers_) w.send_package();
}

void Factory::do_work(Time t) {
    for (auto& w : workers_) w.do_work(t);
}

// Spójność grafu
bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& colors) {
    if (colors[sender] == NodeColor::VERIFIED) return true;
    colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Sender has no receivers");
    }

    bool has_storehouse = false;
    for (const auto& [receiver, _] : sender->receiver_preferences_.get_preferences()) {
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_storehouse = true;
        } else if (receiver->get_receiver_type() == ReceiverType::WORKER) {
            auto worker_ptr = dynamic_cast<const Worker*>(receiver);
            auto sender_ptr = dynamic_cast<const PackageSender*>(worker_ptr);

            if (sender_ptr == sender) continue; // Self-loop check

            has_storehouse = true; // Droga istnieje, ale trzeba sprawdzić czy prowadzi do magazynu

            if (colors[sender_ptr] == NodeColor::UNVISITED) {
                has_reachable_storehouse(sender_ptr, colors);
            }
        }
    }

    colors[sender] = NodeColor::VERIFIED;
    if (!has_storehouse) throw std::logic_error("No reachable storehouse");
    return true;
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> colors;

    for (const auto& r : ramps_) colors[&r] = NodeColor::UNVISITED;
    for (const auto& w : workers_) colors[&w] = NodeColor::UNVISITED;

    try {
        for (const auto& r : ramps_) {
            has_reachable_storehouse(&r, colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }
    return true;
}