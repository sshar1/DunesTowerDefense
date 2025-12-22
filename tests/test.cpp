#include <iostream>

int main() {
    static_assert(2 + 2 == 4);

    std::cout << "Tested successfully!" << std::endl;

    return 0;
}