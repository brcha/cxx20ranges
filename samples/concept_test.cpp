#include <iostream>

template <typename T>
concept CanAddAndMultiply = requires(T a) {
    { a * a } -> std::same_as<T>;
    { a + a } -> std::same_as<T>;
    { 2.5 * a } -> std::convertible_to<T>;
};

template <typename T>
    requires CanAddAndMultiply<T>
constexpr T f(T&& x) {
    auto x2 = x*x;
    auto x3 = x2*x;
    auto x4 = x3*x;
    auto x5 = x4*x;
    return 1+x+0.5*x2+(1/6.)*x3+(1/24.)*x4+(1/120.)*x5;
}

int main() {
    std::cout << "f(1) = " << f(1.0) << '\n';
    //std::cout << "f(\"Hello, world!\") = " << f("Hello, world!") << '\n'; // Throws an error because strings are not addable and multiplicable
}
