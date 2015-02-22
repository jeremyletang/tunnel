// The MIT License (MIT)
//
// Copyright (c) 2015 Jeremy Letang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef TUNNEL
#define TUNNEL

#include <tuple>
#include <memory>

#include "mpsc_queue/include/mpsc_queue.h"

namespace tunnel {

template <typename T>
struct recv_result {
    T value;
    bool state;
};

template <typename T>
struct send_error {
    T value;
    bool state;
};

template <class T>
struct port;

template <class T>
struct chan {

private:

    std::shared_ptr<mpsc::queue<T>> queue;

    chan(std::shared_ptr<mpsc::queue<T>> q)
    : queue(q) {}


public:

    chan(chan& oth) = delete;
    chan(const chan& oth) = delete;

    chan(chan&& oth)
    : queue(std::move(oth.queue)) {
        oth.queue = nullptr;
    }

    auto clone() -> chan {
        auto new_chan = chan(this->queue);
        return std::move(new_chan);
    }

    auto send(T&& t) -> send_error<T>  {
        this->queue->push(std::move(t));
        return send_error<T> {T(), true};
    }

    template <typename U>
    friend auto make() -> std::tuple<chan<U>, port<U>>;

};

template <class T>
struct port {

private:

    std::shared_ptr<mpsc::queue<T>> queue;

    port(std::shared_ptr<mpsc::queue<T>> q)
    : queue(q) {}


public:

    port(port& oth) = delete;
    port(const port& oth) = delete;

    port(port&& oth) {
        this->queue = oth.queue;
        oth.queue = nullptr;
    }

    auto recv() -> recv_result<T> {
        if (this->queue->is_empty()) {
            return recv_result<T> {T(), false};
        }
        return recv_result<T> {this->queue->pop().value, true};
    }

    template <typename U>
    friend auto make() -> std::tuple<chan<U>, port<U>>;


};

template <class T>
auto make() -> std::tuple<chan<T>, port<T>> {
    auto q = std::make_shared<mpsc::queue<T>>();
    auto c = chan<T>(q);
    auto p = port<T>(q);
    return std::move(std::make_tuple(std::move(c), std::move(p)));
}

}

#endif
