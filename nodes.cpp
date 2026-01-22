#include "nodes.hxx"

ReceiverType Worker::get_receiver_type() const {
    return ReceiverType::WORKER;
}

ReceiverType Storehouse::get_receiver_type() const {
    return ReceiverType::STOREHOUSE;
}

