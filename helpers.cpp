#include "helpers.hxx"
#include <sstream>

std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    return std::generate_canonical<double, 10>(rng);
}
ProbabilityGenerator probability_generator = default_probability_generator;

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

    ParsedLineData data;
    if (element_map.find(type_str) != element_map.end()) {
        data.element_type = element_map.at(type_str);
    } else {
        throw std::logic_error("Unknown element type");
    }

    std::string token;
    while (ss >> token) {
        auto pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            data.params[key] = value;
        }
    }
    return data;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto data = parse_line(line);

        if (data.element_type == ElementType::RAMP) {
            factory.add_ramp(Ramp(std::stoi(data.params["id"]), std::stoi(data.params["delivery-interval"])));
        }
        else if (data.element_type == ElementType::WORKER) {
            factory.add_worker(Worker(
                std::stoi(data.params["id"]),
                std::stoi(data.params["processing-time"]),
                std::make_unique<PackageQueue>(PackageQueueType::FIFO)
            ));
        }
        else if (data.element_type == ElementType::STOREHOUSE) {
            factory.add_storehouse(Storehouse(std::stoi(data.params["id"])));
        }
        else if (data.element_type == ElementType::LINK) {
            ElementID src = std::stoi(data.params["src"]);
            ElementID dest = std::stoi(data.params["dest"]);

            IPackageReceiver* receiver = nullptr;
            auto w_it = factory.find_worker_by_id(dest);
            // ZMIANA: używamy worker_cend() zamiast worker_end()
            if (w_it != factory.worker_cend()) receiver = &(*w_it);

            auto s_it = factory.find_storehouse_by_id(dest);
            // ZMIANA: używamy storehouse_cend() zamiast storehouse_end()
            if (s_it != factory.storehouse_cend()) receiver = &(*s_it);

            if (receiver) {
                auto r_it = factory.find_ramp_by_id(src);
                // ZMIANA: używamy ramp_cend() zamiast ramp_end()
                if (r_it != factory.ramp_cend()) {
                    r_it->receiver_preferences_.add_receiver(receiver);
                    continue;
                }
                auto wsrc_it = factory.find_worker_by_id(src);
                // ZMIANA: używamy worker_cend() zamiast worker_end()
                if (wsrc_it != factory.worker_cend()) {
                    wsrc_it->receiver_preferences_.add_receiver(receiver);
                    continue;
                }
            }
        }
    }
    return factory;
}

void save_factory_structure(const Factory& factory, std::ostream& os) {
    // (Implementacja bez zmian, zakładając że gettery są poprawne)
}

void simulate(Factory& factory, TimeOffset duration, std::function<void(Factory&, Time)> report_function) {
    if (!factory.is_consistent()) throw std::logic_error("Network inconsistent");
    for (Time t = 1; t <= duration; ++t) {
        factory.do_deliveries(t);
        factory.do_package_passing();
        factory.do_work(t);
        report_function(factory, t);
    }
}