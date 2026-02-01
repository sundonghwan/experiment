#include<iostream>

int main() {
    int arr[3] = {1, 2, 3};
    int* p = &arr[0];
    std::cout << arr[1] << "\n";
    std::cout << *(p + 1) << "\n";
}