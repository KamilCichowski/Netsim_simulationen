#include "helpers.hxx"
#include "factory.hxx"

#include <cstdlib>
#include <random>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <stdexcept>

static const std::map<std::string, ElementType> element_map {
        {"RAMP", ElementType::RAMP},
        {"WORKER", ElementType::WORKER},
        {"STOREHOUSE", ElementType::STOREHOUSE},
        {"LINK", ElementType::LINK}
};

ParsedLineData parse_line(const std::string& line) {
    std::istringstream ss(line);
    std::string type_str;
    ss >> type_str;

    if (element_map.count(type_str) == 0) {
        throw std::logic_error("Unknown element type");
    }

    ParsedLineData data;
    data.element_type = element_map.at(type_str);

    std::string token;
    while (ss >> token) {
        auto pos = token.find('=');
        if (pos == std::string::npos)
            throw std::logic_error("Invalid key=value");

        std::string key = token.substr(0, pos);
        std::string value = token.substr(pos + 1);
        data.params[key] = value;
    }

    return data;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        auto data = parse_line(line);

        switch (data.element_type) {
            case ElementType::RAMP: {
                ElementID id = std::stoi(data.params.at("id"));
                Time t = std::stoi(data.params.at("delivery-interval"));
                factory.add_ramp(Ramp{id, t});
                break;
            }
            case ElementType::WORKER: {
                ElementID id = std::stoi(data.params.at("id"));
                Time t = std::stoi(data.params.at("processing-time"));
                factory.add_worker(Worker{id, t});
                break;
            }
            case ElementType::STOREHOUSE: {
                ElementID id = std::stoi(data.params.at("id"));
                factory.add_storehouse(Storehouse{id});
                break;
            }
            case ElementType::LINK: {
                ElementID src = std::stoi(data.params.at("src"));
                ElementID dest = std::stoi(data.params.at("dest"));

                IPackageReceiver* receiver = nullptr;

                auto w_it = factory.find_worker_by_id(dest);
                if (w_it != factory.worker_cend()) {
                    receiver = &(*w_it);
                } else {
                    auto s_it = factory.find_storehouse_by_id(dest);
                    if (s_it != factory.storehouse_cend()) {
                        receiver = &(*s_it);
                    }
                }

                if (receiver == nullptr) {
                    throw std::logic_error("Invalid LINK destination");
                }

                auto r_it = factory.find_ramp_by_id(src);
                if (r_it != factory.ramp_cend()) {
                    r_it->add_receiver(receiver);
                    break;
                }

                auto wsrc_it = factory.find_worker_by_id(src);
                if (wsrc_it != factory.worker_cend()) {
                    wsrc_it->add_receiver(receiver);
                    break;
                }

                throw std::logic_error("Invalid LINK source");
            }

        }
    }

    return factory;
}

void save_factory_structure(const Factory& factory, std::ostream& os) {
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "RAMP id=" << it->get_id()
           << " delivery-interval=" << it->get_delivery_interval()
           << "\n";
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        os << "WORKER id=" << it->get_id()
           << " processing-time=" << it->get_processing_time()
           << "\n";
    }

    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    os.flush();

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        for (const auto& [receiver, _] : it->receiver_preferences_) {
            os << "LINK src=" << it->get_id()
               << " dest=" << receiver->get_id()
               << "\n";
        }
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        for (const auto& [receiver, _] : it->receiver_preferences_) {
            os << "LINK src=" << it->get_id()
               << " dest=" << receiver->get_id()
               << "\n";
        }
    }

}


// Do generowania wysokiej jakości ciągów liczb pseudolosowych warto użyć
// zaawansowanych generatorów, np. algorytmu Mersenne Twister.
// zob. https://en.cppreference.com/w/cpp/numeric/random
std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    // Generuj liczby pseudolosowe z przedziału [0, 1); 10 bitów losowości.
    return std::generate_canonical<double, 10>(rng);
}

std::function<double()> probability_generator = default_probability_generator;

// ===== SpecificTurnsReportNotifier =====

SpecificTurnsReportNotifier::SpecificTurnsReportNotifier(
    const std::set<Time>& turns
) : turns_(turns) {}

bool SpecificTurnsReportNotifier::should_generate_report(Time t) const {
    return turns_.find(t) != turns_.end();
}

// ===== IntervalReportNotifier =====

IntervalReportNotifier::IntervalReportNotifier(TimeOffset interval)
    : interval_(interval) {}

bool IntervalReportNotifier::should_generate_report(Time t) const {
    return (t - 1) % interval_ == 0;
}

// ===== simulate() =====

void simulate(
    Factory& factory,
    TimeOffset duration,
    std::function<void(Factory&, Time)> report_function
) {
    if (!factory.is_consistent()) {
        throw std::logic_error("Factory network is inconsistent");
    }

    for (Time t = 1; t <= duration; ++t) {
        factory.do_deliveries(t);
        factory.do_package_passing();
        factory.do_work(t);

        report_function(factory, t);
    }
}

