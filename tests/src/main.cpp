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
    return 0;
}