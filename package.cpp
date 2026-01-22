#include "package.hxx"

std::set<ElementID> Package::assigned_IDs_;
std::set<ElementID> Package::freed_IDs_;

ElementID Package::acquire_id() {
    if (!freed_IDs_.empty()) {
        auto it = freed_IDs_.begin();
        ElementID id = *it;
        freed_IDs_.erase(it);
        assigned_IDs_.insert(id);
        return id;
    }

    ElementID new_id = assigned_IDs_.empty()
        ? 1
        : (*assigned_IDs_.rbegin()) + 1;

    assigned_IDs_.insert(new_id);
    return new_id;
}

void Package::release_id(ElementID id) {
    assigned_IDs_.erase(id);
    freed_IDs_.insert(id);
}

Package::Package() : id_(acquire_id()) {}

Package::Package(ElementID id) : id_(id) {
    assigned_IDs_.insert(id);
}

Package::Package(Package&& other) noexcept : id_(other.id_) {
    other.id_ = -1;
}

Package& Package::operator=(Package&& other) noexcept {
    if (this != &other) {
        if (id_ != -1) {
            release_id(id_);
        }
        id_ = other.id_;
        other.id_ = -1;
    }
    return *this;
}

ElementID Package::get_id() const {
    return id_;
}

Package::~Package() {
    if (id_ != -1) {
        release_id(id_);
    }
}
