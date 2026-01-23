#ifndef STORAGE_TYPES_HXX
#define STORAGE_TYPES_HXX

#include "package.hxx"
#include "types.hxx"
#include <list>

class IPackageStockpile {
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&& package) = 0;
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;

    virtual ~IPackageStockpile() = default;
};

class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
    virtual ~IPackageQueue() = default;
};

class PackageQueue : public IPackageQueue {
public:
    explicit PackageQueue(PackageQueueType type);

    void push(Package&& package) override;
    Package pop() override;
    PackageQueueType get_queue_type() const override;

    bool empty() const override;
    std::size_t size() const override;

    const_iterator begin() const override;
    const_iterator end() const override;
    const_iterator cbegin() const override;
    const_iterator cend() const override;

private:
    PackageQueueType type_;
    std::list<Package> packages_;
};

#endif // STORAGE_TYPES_HXX