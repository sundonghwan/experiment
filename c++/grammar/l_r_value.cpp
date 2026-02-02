#include<iostream>
#include <utility> // std::move

void f(int&) { std::cout << "lvalue ref\n"; }
void f(int&&) { std::cout << "rvalue ref\n"; }

int main() {
    int a = 1;

    f(a);            // lvalue
    f(1);            // rvalue(prvalue)
    f(a + 1);        // rvalue(prvalue)
    f(std::move(a)); // rvalue(xvalue)
}