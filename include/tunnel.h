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
#include <thread>

#include "shared_queue.h"

namespace tunnel {

// Store the result of a call to recv or try_recv
// if the call to the function fail, state is equal to false and
// value is a default value, otherwise state is equal to true
// and the received value is stored in value.
template <typename T>
struct recv_result {
    T value;
    bool state;
};

// Return the success, or failure of a send call fron a channel
// if send success, value is a default value for the type T and state
// is equal to true, otherwise value is the value that failed to send
// and state is equal to false.
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

    // the queue wich is shared between the different channels and the
    // port
    std::shared_ptr<shared_queue<T>> queue;

    // private constructor, wich allow us to share the queue
    // between the channels and the port
    chan(std::shared_ptr<shared_queue<T>> q)
    : queue(q) {
        this->queue->inc_chan();
    }


public:

    // copy constructor deleted as we want explicit call to
    // clone to create a copy of the channel
    chan(chan& oth) = delete;

    // same as above
    chan(const chan& oth) = delete;

    // move constructor
    // channel cannot be implicitly copied
    // defautl behavior is move
    chan(chan&& oth)
    : queue(std::move(oth.queue)) {
        oth.queue = nullptr;
    }

    // destroy the channel and decrement the queue counter
    // of channels.
    ~chan() {
        // prevent remove when std::move()
        if (this->queue != nullptr) { this->queue->dec_chan(); }
    }

    // explicitly create a new channel from an existant one
    // the one will share the same message queue and increment the
    // queue internal channel's counter
    auto clone() -> chan {
        auto new_chan = chan(this->queue);
        return std::move(new_chan);
    }

    // Send a new message in the queue.
    // in the case that the port is deleted,
    // send_error.value will be the value to send and send_error.state will be false,
    // otherwise send_error.value will be a default value and send_error.state will be true
    auto send(T&& t) -> send_error<T>  {
        if (!this->queue->has_port()) {
             return send_error<T> {std::move(t), false};
        }
        this->queue->push(std::move(t));
        return send_error<T> {T(), true};
    }

    // Will the channel be able to send a message,
    // return true if the port already exist, false
    // if it is deleted.
    auto can_send() -> bool { return this->queue->has_port(); }

    // friend tunction to wich instantciate the original channel
    template <typename U>
    friend auto make() -> std::tuple<chan<U>, port<U>>;

};

template <class T>
struct port {

private:

    std::shared_ptr<shared_queue<T>> queue;

    port(std::shared_ptr<shared_queue<T>> q)
    : queue(q) {}


public:

    port(port& oth) = delete;
    port(const port& oth) = delete;

    port(port&& oth)
    : queue(std::move(oth.queue)) {
        oth.queue = nullptr;
    }

    ~port() {
        // prevent remove when std::move()
        if (this->queue != nullptr) { this->queue->rm_port(); }
    }

    auto try_recv() -> recv_result<T> {
        if (this->queue->is_empty() || !this->can_recv()) {
            return recv_result<T> {T(), false};
        }
        return recv_result<T> {this->queue->pop().value, true};
    }

    auto recv() -> recv_result<T> {
        while (this->can_recv()) {
            if (this->queue->is_empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } else {
                return recv_result<T> {this->queue->pop().value, true};
            }
        }
        return recv_result<T> {T(), false};
    }

    auto iter(std::function<void(T)> f) {
        while (!this->queue->is_empty()) {
            auto t = this->queue->pop().value;
            f(std::move(t));
        }
    }

    auto can_recv() -> bool { return this->queue->nb_chan() > 0; }

    auto nb_chan() -> int { return this->queue->nb_chan(); }

    // friend tunction to wich instantciate the original channel
    template <typename U>
    friend auto make() -> std::tuple<chan<U>, port<U>>;


};

// instanciate a new channel and port
// and return them as a tuple.
// the type T MUST be default constructible
template <class T>
auto make() -> std::tuple<chan<T>, port<T>> {
    static_assert(std::is_default_constructible<T>::value,
                  "tunnel can only send default constructible data.");
    auto q = std::make_shared<shared_queue<T>>();
    auto c = chan<T>(q);
    auto p = port<T>(q);
    return std::move(std::make_tuple(std::move(c), std::move(p)));
}

}

#endif
