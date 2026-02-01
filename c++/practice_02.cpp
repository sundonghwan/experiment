#include<iostream>
#include<stdio.h>


struct A {
    int x;
};

struct B {
    int y;
};

int main (){
    A a;
    B b;
    a.x = 10;

    A* pa = &a;
    // pa의 역참조를 통한 x 값 출력
    std::cout << (*pa).x << "\n";
    
    // 화살표 연산자를 통한 x 값 출력
    std::cout << pa->x << "\n";
};