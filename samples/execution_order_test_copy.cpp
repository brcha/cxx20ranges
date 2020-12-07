#include <iostream>
#include <ranges>
#include <vector>
#include <algorithm>

int main() {
    auto even = [](int i) { 
        std::cout << "even(" << i << ")\n";
        return i % 2 == 0;
    };
    auto square = [](int i) {
        std::cout << "square(" << i << ")\n";
        return i * i;
    };
    auto even_squares = std::views::filter(even)
                      | std::views::transform(square);

    std::vector<int> v { 1, 2, 3, 4, 5, 6 };

    std::vector<int> es;
    std::ranges::copy(v | even_squares, std::back_inserter(es));

    for (int i : es)
    {
        std::cout << "for: i=" << i << '\n';
    }

    for (int i : es)
    {
        std::cout << "for: i=" << i << '\n';
    }
}