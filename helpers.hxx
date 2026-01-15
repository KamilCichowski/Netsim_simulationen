#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <functional>
#include <random>

#include "types.hxx"

extern std::random_device rd;
extern std::mt19937 rng;

extern double default_probability_generator();

extern ProbabilityGenerator probability_generator;

class Factory;

void simulate(
    Factory& factory,
    TimeOffset duration,
    std::function<void(Factory&, Time)> report_function
);

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



#endif /* HELPERS_HPP_ */
