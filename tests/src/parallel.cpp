#include <tunnel.h>
#include <iostream>
#include <thread>

int main() {
    auto comm = tunnel::make<std::string>();
    auto chan = std::move(std::get<0>(comm));
    auto port = std::move(std::get<1>(comm));

    auto t = std::thread([&]() {
        auto port_ = std::move(port);
        auto recv_res = port_.recv();
        std::cout << recv_res.value << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    chan.send(std::move("hello world from main thread !"));
    t.join();

    std::cout << "chan can send ? " << std::boolalpha << chan.can_send() << std::endl;

    return 0;
}
