#include<iostream>
#include<cstddef>

// 객체 레이아웃 눈으로 보기
struct A {
    char c;
    int i;
};

int main() {
    A arr[2];
    std::cout << "sizeof(A) = " << sizeof(A) << "\n";
    std::cout << "&arr[0] = " << (void*)&arr[0] << "\n";
    std::cout << "&arr[1] = " << (void*)&arr[1] << "\n";

    std::ptrdiff_t diff = (char*)&arr[1] - (char*)&arr[0];
    std::cout <<"byte diff = " << diff <<"\n";
}