#include<iostream>
#include<cstddef>
void byPointer(int* p) { *p = 88; }
int byValude(int v) { v = 99; return v; } // 반환값은 호출자 쪽의 독립된 값으로 저장됨

int main() {
    // 레퍼런스 연습
    int a = 10;
    int& r = a;
    int* p = &a; 
    r = 20 ;
    
    std::cout << "a = " << a << "\n";
    byPointer(&a);
    std::cout << "a = " << a << "\n";

}