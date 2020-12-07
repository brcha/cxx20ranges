#include <ranges>
#include <iostream>

int main() {
    for (int i : std::views::iota(1)) {
        std::cout << i << ' ';
        if (i % 7 == 0)
            std::cout << '\n';
    }
    std::cout << '\n';
}