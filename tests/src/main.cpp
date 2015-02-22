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

int
main() {
    std::cout << "comm" << std::endl;
    auto comm = tunnel::make<point>();
    std::cout << "chan" << std::endl;
    auto chan = std::move(std::get<0>(comm));
    std::cout << "port" << std::endl;
    auto port = std::move(std::get<1>(comm));
    std::cout << "chan_copy" << std::endl;
    tunnel::chan<point> chan_copy = chan.clone();

    std::cout << "chan_send" << std::endl;
    chan.send(std::move(point {1, 2}));
    std::cout << "received: " << port.recv().value << std::endl;

    return 0;
}