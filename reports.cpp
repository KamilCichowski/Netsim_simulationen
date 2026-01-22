#include "reports.hxx"

#include <iostream>
#include <optional>

#include "factory.hxx"
#include "nodes.hxx"
#include "storage_types.hxx"

// =========================
// STRUCTURE REPORT
// =========================

void generate_structure_report(const Factory& f, std::ostream& os) {
    os << "== FACTORY STRUCTURE REPORT ==" << std::endl;

    // ---- RAMPS ----
    os << std::endl << "Ramps:" << std::endl;
    for (const auto& ramp : f.get_ramps()) {
        os << "  Ramp #" << ramp.get_id() << std::endl;
    }

    // ---- WORKERS ----
    os << std::endl << "Workers:" << std::endl;
    for (const auto& worker : f.get_workers()) {
        os << "  Worker #" << worker.get_id() << std::endl;
        os << "    Processing time: " << worker.get_processing_time() << std::endl;
        os << "    Queue type: " << to_string(worker.get_queue_type()) << std::endl;
    }

    // ---- STOREHOUSES ----
    os << std::endl << "Storehouses:" << std::endl;
    for (const auto& storehouse : f.get_storehouses()) {
        os << "  Storehouse #" << storehouse.get_id() << std::endl;
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
    os << "== FACTORY SIMULATION REPORT ==" << std::endl;
    os << "Turn: " << t << std::endl;

    // ---- WORKERS ----
    os << std::endl << "Workers:" << std::endl;
    for (const auto& worker : f.get_workers()) {
        os << "  Worker #" << worker.get_id() << std::endl;

        // processing buffer
        const auto& buffer = worker.get_processing_buffer();
        if (buffer.has_value()) {
            os << "    Processing package: " << buffer->get_id() << std::endl;
        } else {
            os << "    Processing package: (none)" << std::endl;
        }

        // queue
        os << "    Queue: ";
        const auto& q = worker.get_queue();
        if (q.begin() == q.end()) {
            os << "(empty)";
        } else {
            for (auto it = q.begin(); it != q.end(); ++it) {
                os << it->get_id();
                if (it != std::prev(q.end())) {
                    os << ", ";
                }
            }
        }
        os << std::endl;
    }

    // ---- STOREHOUSES ----
    os << std::endl << "Storehouses:" << std::endl;
    for (const auto& storehouse : f.get_storehouses()) {
        os << "  Storehouse #" << storehouse.get_id() << std::endl;
        os << "    Stock: ";

        const auto& stock = storehouse.get_stock();
        if (stock.begin() == stock.end()) {
            os << "(empty)";
        } else {
            for (auto it = stock.begin(); it != stock.end(); ++it) {
                os << it->get_id();
                if (it != std::prev(stock.end())) {
                    os << ", ";
                }
            }
        }
        os << std::endl;
    }
}
