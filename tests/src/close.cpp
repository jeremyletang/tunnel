#include <tunnel>
#include <memory>
#include <iostream>

void drop_port(tunnel::port<int>&& port) {
    auto p = std::move(port);
}

int main() {
    auto comm = tunnel::make<int>();
    auto chan = std::move(std::get<0>(comm));
    auto port = std::move(std::get<1>(comm));

    drop_port(std::move(port));

    std::cout << "tunnel is closed: " << std::boolalpha << (chan == tunnel::closed) << std::endl;

    return 0;
}
