#include <tunnel>
#include <memory>
#include <iostream>
#include <thread>

int main() {
    auto comm = tunnel::make<int>();
    auto chan = std::move(std::get<0>(comm));
    auto port = std::move(std::get<1>(comm));

    chan.send(std::move(0));
    chan.send(std::move(1));
    chan.send(std::move(2));
    chan.send(std::move(3));
    chan.send(std::move(4));
    chan.send(std::move(5));


    for (auto i : port) {
        std::cout << std::move(i) << std::endl;
    }

    return 0;
}
