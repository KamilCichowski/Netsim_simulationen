#include "helpers.hxx"
#include "factory.hxx"

#include <cstdlib>
#include <random>
#include <functional>
#include <stdexcept>

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

