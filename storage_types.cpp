#include "storage_types.hxx"
#include <stdexcept>

PackageQueue::PackageQueue(PackageQueueType type) : type_(type) {}

void PackageQueue::push(Package&& package) {
    packages_.emplace_back(std::move(package));
}

Package PackageQueue::pop() {
    if (packages_.empty()) {
        throw std::logic_error("Queue is empty");
    }

    // Wybierz element w zależności od typu kolejki (FIFO/LIFO)
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

bool PackageQueue::empty() const { return packages_.empty(); }
std::size_t PackageQueue::size() const { return packages_.size(); }
PackageQueueType PackageQueue::get_queue_type() const { return type_; }

IPackageStockpile::const_iterator PackageQueue::begin() const { return packages_.begin(); }
IPackageStockpile::const_iterator PackageQueue::end() const { return packages_.end(); }
IPackageStockpile::const_iterator PackageQueue::cbegin() const { return packages_.cbegin(); }
IPackageStockpile::const_iterator PackageQueue::cend() const { return packages_.cend(); }