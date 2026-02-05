#pragma once
#include <iostream>
#include <utility>


struct Trace {
    int id{};
    static inline int next_id = 1; 

    Trace() : id(next_id++) { std::cout << "[Trace " << id << "] default\n"; }

    ~Trace() { std::cout << "[Trace " << id << "] dtor\n"; }

    Trace(const Trace& o) : id(next_id++) {
        std::cout << "[Trace " << id << "] copy from " << o.id << "\n";
    }

    Trace(Trace&& o) noexcept : id(o.id) {
        o.id = -1;
        std::cout << "[Trace " << id << "] move\n";
    }

    Trace& operator=(const Trace& o) {
        std::cout << "[Trace " << id << "] copy= from " << o.id << "\n";
        return *this;
    }


    Trace& operator=(Trace&& o) noexcept {
        std::cout << "[Trace " << id << "] move=\n";
        id = o.id;
        o.id = -1;
        return *this;
    }
};