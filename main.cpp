#include "helpers.hxx"
#include "factory.hxx"

int main() {
    Factory factory;
    // TODO: inicjalizacja factory

    simulate(factory, 5,
        [](Factory&, Time) {
            // nic nie robimy
        }
    );
}
