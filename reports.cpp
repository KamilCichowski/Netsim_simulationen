#include "reports.hxx"

#include <iostream>
#include <optional>
#include <vector>
#include <algorithm>

#include "factory.hxx"
#include "nodes.hxx"
#include "storage_types.hxx"

namespace {

// Helper for sorting receivers: storehouse < worker, then by id
int receiver_type_order(ReceiverType type) {
    switch (type) {
        case ReceiverType::STOREHOUSE: return 0;
        case ReceiverType::WORKER:     return 1;
    }
    return 2;
}

struct ReceiverInfo {
    ReceiverType type;
    ElementID id;
};

std::vector<ReceiverInfo> sorted_receivers(const ReceiverPreferences& prefs) {
    std::vector<ReceiverInfo> result;
    for (const auto& [receiver, _] : prefs.get_preferences()) {
        result.push_back(ReceiverInfo{receiver->get_receiver_type(), receiver->get_id()});
    }

    std::sort(result.begin(), result.end(), [](const ReceiverInfo& a, const ReceiverInfo& b) {
        int ta = receiver_type_order(a.type);
        int tb = receiver_type_order(b.type);
        if (ta != tb) return ta < tb;
        return a.id < b.id;
    });

    return result;
}

std::string receiver_type_name(ReceiverType type) {
    switch (type) {
        case ReceiverType::STOREHOUSE: return "storehouse";
        case ReceiverType::WORKER:     return "worker";
    }
    return "";
}

} // unnamed namespace

// =========================
// STRUCTURE REPORT
// =========================

void generate_structure_report(const Factory& f, std::ostream& os) {
    // Collect and sort ramps by id
    std::vector<const Ramp*> ramps;
    for (const auto& r : f.get_ramps()) ramps.push_back(&r);
    std::sort(ramps.begin(), ramps.end(),
              [](const Ramp* a, const Ramp* b) { return a->get_id() < b->get_id(); });

    // Collect and sort workers by id
    std::vector<const Worker*> workers;
    for (const auto& w : f.get_workers()) workers.push_back(&w);
    std::sort(workers.begin(), workers.end(),
              [](const Worker* a, const Worker* b) { return a->get_id() < b->get_id(); });

    // Collect and sort storehouses by id
    std::vector<const Storehouse*> stores;
    for (const auto& s : f.get_storehouses()) stores.push_back(&s);
    std::sort(stores.begin(), stores.end(),
              [](const Storehouse* a, const Storehouse* b) { return a->get_id() < b->get_id(); });

    // == LOADING RAMPS ==
    os << "== LOADING RAMPS ==" << std::endl;
    for (const auto* ramp : ramps) {
        os << "LOADING_RAMP #" << ramp->get_id() << std::endl;
        os << "  Delivery interval: " << ramp->get_delivery_interval() << std::endl;
        os << "  Receivers:" << std::endl;

        auto recs = sorted_receivers(ramp->receiver_preferences_);
        for (const auto& rec : recs) {
            os << "    " << receiver_type_name(rec.type) << " #" << rec.id << std::endl;
        }
        if (recs.empty()) {
            os << "    (none)" << std::endl;
        }
        os << std::endl;
    }
    if (!ramps.empty()) {
        // remove extra blank line after last ramp
        os.seekp(-1, std::ios_base::cur);
    }

    // == WORKERS ==
    if (!ramps.empty()) {
        os << std::endl;
    }
    os << "== WORKERS ==" << std::endl;
    for (const auto* worker : workers) {
        os << "WORKER #" << worker->get_id() << std::endl;
        os << "  Processing time: " << worker->get_processing_time() << std::endl;
        os << "  Queue type: " << to_string(worker->get_queue_type()) << std::endl;
        os << "  Receivers:" << std::endl;

        auto recs = sorted_receivers(worker->receiver_preferences_);
        for (const auto& rec : recs) {
            os << "    " << receiver_type_name(rec.type) << " #" << rec.id << std::endl;
        }
        if (recs.empty()) {
            os << "    (none)" << std::endl;
        }
        os << std::endl;
    }
    if (!workers.empty()) {
        os.seekp(-1, std::ios_base::cur);
    }

    // == STOREHOUSES ==
    if (!workers.empty()) {
        os << std::endl;
    }
    os << "== STOREHOUSES ==" << std::endl;
    for (const auto* store : stores) {
        os << "STOREHOUSE #" << store->get_id() << std::endl;
    }
}

// =========================
// SIMULATION TURN REPORT
// =========================

void generate_simulation_turn_report(
    const Factory& f,
    std::ostream& os,
    Time t
) {
    // Header
    os << "=== [ Turn: " << t << " ] ===" << std::endl;

    // Collect and sort workers and storehouses by id
    std::vector<const Worker*> workers;
    for (const auto& w : f.get_workers()) workers.push_back(&w);
    std::sort(workers.begin(), workers.end(),
              [](const Worker* a, const Worker* b) { return a->get_id() < b->get_id(); });

    std::vector<const Storehouse*> stores;
    for (const auto& s : f.get_storehouses()) stores.push_back(&s);
    std::sort(stores.begin(), stores.end(),
              [](const Storehouse* a, const Storehouse* b) { return a->get_id() < b->get_id(); });

    // WORKERS section
    os << "== WORKERS ==" << std::endl;
    for (const auto* worker : workers) {
        os << "WORKER #" << worker->get_id() << std::endl;

        // PBuffer
        const auto& buffer = worker->get_processing_buffer();
        if (buffer.has_value()) {
            TimeOffset pt = t - worker->get_package_processing_start_time() + 1;
            os << "  PBuffer: #" << buffer->get_id() << " (pt=" << pt << ")" << std::endl;
        } else {
            os << "  PBuffer: (empty)" << std::endl;
        }

        // Queue
        os << "  Queue: ";
        const auto* q = worker->get_queue();
        if (q == nullptr || q->begin() == q->end()) {
            os << "(empty)" << std::endl;
        } else {
            bool first = true;
            for (auto it = q->begin(); it != q->end(); ++it) {
                if (!first) os << ", ";
                os << "#" << it->get_id();
                first = false;
            }
            os << std::endl;
        }

        // SBuffer (sending buffer)
        const auto& sbuf = worker->get_sending_buffer();
        if (sbuf.has_value()) {
            os << "  SBuffer: #" << sbuf->get_id() << std::endl;
        } else {
            os << "  SBuffer: (empty)" << std::endl;
        }

        os << std::endl;
    }
    if (!workers.empty()) {
        os.seekp(-1, std::ios_base::cur);
    }

    // STOREHOUSES section
    if (!workers.empty()) {
        os << std::endl;
    }
    os << "== STOREHOUSES ==" << std::endl;
    for (const auto* store : stores) {
        os << "STOREHOUSE #" << store->get_id() << std::endl;
        os << "  Stock: ";

        const auto& stock = store->get_stock();
        if (stock.begin() == stock.end()) {
            os << "(empty)" << std::endl;
        } else {
            bool first = true;
            for (auto it = stock.begin(); it != stock.end(); ++it) {
                if (!first) os << ", ";
                os << "#" << it->get_id();
                first = false;
            }
            os << std::endl;
        }
    }
}
