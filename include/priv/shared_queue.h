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

#ifndef TUNNEL_SHARED_QUEUE
#define TUNNEL_SHARED_QUEUE

#include "mpsc_queue/include/mpsc_queue.h"
#include <iostream>

namespace priv {

// high level container for the mpsc::queue
// and atomic value to manage the states of the differents
// channels and the port
template<typename T>
struct shared_queue {

private:

    // the queue to store the messages
    mpsc::queue<T> queue;

    // total count of available channels
    std::atomic<int> chan_cnt;

    // is the port deleted or not
    std::atomic<bool> port_cnt;

public:

    shared_queue()
    : queue(mpsc::queue<T>()), chan_cnt(0), port_cnt(true) {}

    // push a new message in the mpsc::queue
    auto push(T&& v) { this->queue.push(std::move(v)); }

    // try to pop a message from the mpsc::queue
    auto pop() -> mpsc::pop_result<T> { return this->queue.pop(); }

    // is the mpsc::queue empty or not
    auto is_empty() const -> bool { return this->queue.is_empty(); }

    // is the port deleted or not
    auto has_port() const -> bool { return this->port_cnt.load(); }

    // return the count of available channels
    auto nb_chan() const -> int { return this->chan_cnt.load(); }

    // set the port state to false, should be call once
    // at the port destruction
    auto rm_port() { this->port_cnt.store(false); }

    // increment the channels counts,
    // could be called in two case.
    // when the initial channel is created
    // and when a channel is cloned
    auto inc_chan() { this->chan_cnt++; }

    // decrement the channels count
    // should be only call on the destruct of a channel
    // be careful to not decrement when a moved value is destroyed
    auto dec_chan() { this->chan_cnt--; }

};

}

#endif
