#ifndef PACKAGE_HXX
#define PACKAGE_HXX

#include "types.hxx"
#include <set>

class Package {
public:
    Package();
    explicit Package(ElementID id);

    Package(Package&& other) noexcept;
    Package& operator=(Package&& other) noexcept;

    // Blokada kopiowania
    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;

    ElementID get_id() const;

    ~Package();

private:
    ElementID id_;
    static std::set<ElementID> assigned_IDs_;
    static std::set<ElementID> freed_IDs_;

    static ElementID acquire_id();
    static void release_id(ElementID id);
};

#endif // PACKAGE_HXX