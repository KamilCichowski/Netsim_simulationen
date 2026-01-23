#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <functional>
#include <random>
#include <map>
#include <iostream>
#include <set>
#include <numeric> // <--- DODANO TO (naprawia błąd std::accumulate)

#include "types.hxx"
#include "factory.hxx"

enum class ElementType {
    RAMP, WORKER, STOREHOUSE, LINK
};

struct ParsedLineData {
    ElementType element_type;
    std::map<std::string, std::string> params;
};

ParsedLineData parse_line(const std::string& line);
Factory load_factory_structure(std::istream& is);
void save_factory_structure(const Factory& factory, std::ostream& os);

extern std::random_device rd;
extern std::mt19937 rng;
extern double default_probability_generator();

// Ta zmienna jest używana przez ReceiverPreferences
extern ProbabilityGenerator probability_generator;

void simulate(Factory& factory, TimeOffset duration, std::function<void(Factory&, Time)> report_function);

class SpecificTurnsReportNotifier {
public:
    explicit SpecificTurnsReportNotifier(const std::set<Time>& turns);
    bool should_generate_report(Time t) const;

private:
    std::set<Time> turns_;
};

class IntervalReportNotifier {
public:
    explicit IntervalReportNotifier(TimeOffset interval);
    bool should_generate_report(Time t) const;

private:
    TimeOffset interval_;
};

#endif // HELPERS_HPP_