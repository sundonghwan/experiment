#include<iostream>

int add(int a, int b) {
    return a + b;
};

int main(){
    int a = 10;
    int b = 20;
    int result = add(a, b);
    std::cout << "result: " << result << "\n";
}