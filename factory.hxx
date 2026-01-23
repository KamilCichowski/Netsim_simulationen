#ifndef FACTORY_HXX
#define FACTORY_HXX

#include <list>
#include <algorithm>
#include <utility>
#include <map>
#include <stdexcept>
#include "nodes.hxx"

template <typename Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) { collection_.push_back(std::move(node)); }

    iterator find_by_id(ElementID id) {
        return std::find_if(collection_.begin(), collection_.end(),
            [id](const Node& n) { return n.get_id() == id; });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(collection_.cbegin(), collection_.cend(),
            [id](const Node& n) { return n.get_id() == id; });
    }

    void remove_by_id(ElementID id) {
        auto it = find_by_id(id);
        if (it != collection_.end()) {
            collection_.erase(it);
        }
    }

    iterator begin() { return collection_.begin(); }
    iterator end() { return collection_.end(); }
    const_iterator begin() const { return collection_.cbegin(); }
    const_iterator end() const { return collection_.cend(); }
    const_iterator cbegin() const { return collection_.cbegin(); }
    const_iterator cend() const { return collection_.cend(); }

private:
    container_t collection_;
};

class Factory {
public:
    void add_ramp(Ramp&& r);
    void remove_ramp(ElementID id);
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id);
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const;
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const;
    NodeCollection<Ramp>::const_iterator ramp_cend() const;

    void add_worker(Worker&& w);
    void remove_worker(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id);
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const;
    NodeCollection<Worker>::const_iterator worker_cbegin() const;
    NodeCollection<Worker>::const_iterator worker_cend() const;

    void add_storehouse(Storehouse&& s);
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id);
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const;
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const;
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const;

    // Gettery dla Reports (zwracają kolekcje)
    const NodeCollection<Ramp>& get_ramps() const { return ramps_; }
    const NodeCollection<Worker>& get_workers() const { return workers_; }
    const NodeCollection<Storehouse>& get_storehouses() const { return storehouses_; }

    bool is_consistent() const;
    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;

    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);
};

template <typename Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {
    for (auto& r : ramps_) r.receiver_preferences_.remove_receiver(dynamic_cast<IPackageReceiver*>(&*collection.find_by_id(id)));
    for (auto& w : workers_) w.receiver_preferences_.remove_receiver(dynamic_cast<IPackageReceiver*>(&*collection.find_by_id(id)));
    collection.remove_by_id(id);
}

#endif // FACTORY_HXX