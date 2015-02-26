#include <tunnel.h>

#include <memory>
#include <iostream>

struct point {
    int x;
    int y;
};

std::ostream& operator<<(std::ostream& os, const point& p) {
    os << "point { x: " << p.x << ", y: " << p.y << " }";
    return os;
}

void send_msg(tunnel::chan<point> chan) {
    chan.send(std::move(point {42, 42}));
}

void consume_port(tunnel::port<point> port) {
    std::cout << std::boolalpha << "can recv: " << port.can_recv() << std::endl;
    std::cout << "received: " << port.recv().value << std::endl;
    std::cout << "nb chan: " << port.nb_chan() << std::endl;
}

int main() {
    auto comm = tunnel::make<point>();
    auto chan = std::move(std::get<0>(comm));
    auto port = std::move(std::get<1>(comm));
    auto chan_copy = chan.clone();
    auto moved_copy = std::move(chan_copy);

    send_msg(std::move(moved_copy));

    std::cout << "chan_send" << std::endl;
    chan.send(std::move(point {1, 2}));
    std::cout << "received: " << port.try_recv().value << std::endl;

    consume_port(std::move(port));
    std::cout << std::boolalpha << "can send: " << chan.can_send() << std::endl;

    auto send_result = chan.send(std::move(point {-42, -42}));
    std::cout << std::boolalpha << "send state: " << send_result.state << std::endl;
    std::cout << std::boolalpha << "send returned data: " << send_result.value << std::endl;


    return 0;
}
