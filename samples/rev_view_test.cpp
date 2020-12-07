#include <iostream>
#include <ranges>
#include <vector>

int main() {
    auto even = [](int i) { 
        return i % 2 == 0;
    };
    auto square = [](int i) {
        return i * i;
    };
    auto even_squares = std::views::filter(even)
                      | std::views::transform(square);

    for (int i : { 1, 2, 3, 4, 5, 6 } | even_squares)
    {
        std::cout << "for: i=" << i << '\n';
    }
}