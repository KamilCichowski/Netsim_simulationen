#include "storage_types.hxx"
#include <stdexcept>

PackageQueue::PackageQueue(PackageQueueType type)
    : type_(type) {}

void PackageQueue::push(Package&& package) {
    packages_.emplace_back(std::move(package));
}

Package PackageQueue::pop() {
    if (packages_.empty()) {
        throw std::logic_error("Queue is empty");
    }

    Package pkg = (type_ == PackageQueueType::FIFO)
        ? std::move(packages_.front())
        : std::move(packages_.back());

    if (type_ == PackageQueueType::FIFO) {
        packages_.pop_front();
    } else {
        packages_.pop_back();
    }

    return pkg;
}

IPackageStockpile::const_iterator PackageQueue::begin() const {
    return packages_.begin();
}

IPackageStockpile::const_iterator PackageQueue::end() const {
    return packages_.end();
}

IPackageStockpile::const_iterator PackageQueue::cbegin() const {
    return packages_.cbegin();
}

IPackageStockpile::const_iterator PackageQueue::cend() const {
    return packages_.cend();
}

std::size_t PackageQueue::size() const {
    return packages_.size();
}

bool PackageQueue::empty() const {
    return packages_.empty();
}

PackageQueueType PackageQueue::get_queue_type() const {
    return type_;
}
