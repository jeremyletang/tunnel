# tunnel

Simple and lock-free channel communication in c++14

The API is based on the rust channel system for communication between threads.
The library use internally an mpsc queue to store message between channels and port
in a lock-free way.

The library should build with clang (-std=c++1y) and gcc (-std=gnu++1y).

## A simple parallel example

```c++
#include <tunnel.h>
#include <iostream>
#include <thread>

int main() {
    // instanciate a new tunnel
    auto comm = tunnel::make<std::string>();
    // get the channel
    auto chan = std::move(std::get<0>(comm));
    // get the port
    auto port = std::move(std::get<1>(comm));

    // launch a thread and capture the port
    auto t = std::thread([&]() {
        auto p = std::move(port);
        std::cout << p.recv().value << std::endl;
    });

    // let the port waiting for data
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // send some data in the tunnel
    chan.send(std::move("hello world from main thread !"));

    // wait for the thread exit
    t.join();

    return 0;
}
```

## Use tunnel

To use tunnel you need a c++14 compiler and cmake if you want to build the example

First you need to clone the repository and the submodule (mpsc_queue):

```shell
> git clone git@github.com:jeremyletang/tunnel.git --recursive
```

You can build the examples using cmake
```
cd tests && mkdir build && cd build && cmake .. && make
```

then the examples are in the folder `/tests/build/target`.