#include "factory.hxx"
#include "helpers.hxx"
#include "reports.hxx"
#include <iostream>

int main() {
    Factory factory;

    // Ręczna budowa sieci testowej
    factory.add_ramp(Ramp(1, 2)); // Ramp #1, co 2 tury
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO))); // Worker #1
    factory.add_storehouse(Storehouse(1)); // Storehouse #1

    // Połączenia: Ramp -> Worker -> Storehouse
    auto r = factory.find_ramp_by_id(1);
    auto w = factory.find_worker_by_id(1);
    auto s = factory.find_storehouse_by_id(1);

    r->receiver_preferences_.add_receiver(&(*w));
    w->receiver_preferences_.add_receiver(&(*s));

    // Symulacja
    try {
        simulate(factory, 10, [](Factory& f, Time t) {
            generate_simulation_turn_report(f, std::cout, t);
        });
    } catch (const std::exception& e) {
        std::cerr << "Błąd symulacji: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}