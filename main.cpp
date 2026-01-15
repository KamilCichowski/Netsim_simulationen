#include <iostream>
#include "storage_types.hxx "


int main() {

    PackageQueue q(PackageQueueType::FIFO);

    q.push(Package());
    q.push(Package());

    auto p = q.pop();
    std::cout << p.get_id() << std::endl;

    return 0;
}
