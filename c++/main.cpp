#include <iostream>
#include <vector>
#include <utility>
#include "trace.hpp"

// 1) value category가 오버로드 선택을 바꾼다
void sink(Trace&)  { std::cout << "sink: lvalue overload\n"; }
void sink(Trace&&) { std::cout << "sink: rvalue/xvalue overload\n"; }

// 2) "이름 있는 rvalue reference 변수" 함정
Trace make_trace() {
    Trace t;
    return t; // (C++17+) NRVO/엘리전이 될 수 있음
}

// 3) forwarding이 왜 필요한지
template<class T>
void wrapper_bad(T&& x) {
    // x는 "이름 있는 변수"라서 항상 lvalue로 취급된다 (중요)
    sink(x);
}

template<class T>
void wrapper_good(T&& x) {
    sink(std::forward<T>(x));
}

int main() {
    std::cout << "\n=== A) overload & std::move ===\n";
    Trace a;
    sink(a);            // lvalue
    sink(Trace{});      // prvalue
    sink(std::move(a)); // xvalue

    std::cout << "\n=== B) named rvalue ref is lvalue ===\n";
    Trace&& rr = Trace{};
    // rr 자체는 "이름 있는 표현식" -> lvalue
    sink(rr);               // lvalue overload로 감 (헷갈리는 핵심)
    sink(std::move(rr));    // xvalue -> rvalue overload

    std::cout << "\n=== C) copy/move in vector (realloc) ===\n";
    std::vector<Trace> v;
    v.reserve(2);
    v.push_back(Trace{});   // 임시를 넣음
    v.push_back(Trace{});   // 여기까지는 대체로 move 위주
    std::cout << "-- push one more to force realloc --\n";
    v.push_back(Trace{});   // 재할당 시 기존 원소를 옮김( move/noexcept 영향)

    std::cout << "\n=== D) forwarding wrapper ===\n";
    Trace x;
    wrapper_bad(x);
    wrapper_bad(Trace{});
    wrapper_good(x);
    wrapper_good(Trace{});

    std::cout << "\n=== E) return value / elision ===\n";
    Trace r = make_trace();
    sink(r);
}
