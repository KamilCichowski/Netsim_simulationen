#ifndef REPORTS_HXX
#define REPORTS_HXX

#include <iosfwd>

#include "factory.hxx"
#include "types.hxx"

void generate_structure_report(const Factory& f, std::ostream& os);

void generate_simulation_turn_report(
    const Factory& f,
    std::ostream& os,
    Time t
);

#endif // REPORTS_HXX
