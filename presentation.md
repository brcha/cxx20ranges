---
theme : "night"
transition: "slide"
highlightTheme: "solarized-dark"
logoImg: "logoF.png"
slideNumber: true
title: "C++20 Ranges"
---

# C++20 Ranges

CppDUG, December 2020

Filip Brčić &lt;br<span style="display: none">nospam</span>cha@yandex<span style="display: none">nospam</span>.com&gt;

Live: https://cxx20ranges.brcha.com/

Source: https://github.com/brcha/cxx20ranges

---

### Summary

- What are Ranges? {.fragment}
- What is new in algorithms? {.fragment}
- What are Views? {.fragment}
- Current status {.fragment}
- Questions {.fragment}

---

## What are Ranges?

note: For one to understand the ranges, one must first understand concepts

--

## What are Concepts?

Another new feature of C++20 {.fragment}

<div class="fragment">Best summarized as SFINAE done better,</div><div class="fragment">with readable error messages.</div>


--

Let's say we would like to draw a graph of some function for which we have the Maclaurin expansion. For example:

<div>
\[ f(x) = 1 + x + 0.5x^2 + \frac{1}{6}x^3 + \frac{1}{24}x^4 + \frac{1}{120}x^5\]
</div>

note: That is actually exp(x)

--

We can easily write that with double

```c++{.fragment}
constexpr double f(double x) {
    double x2 = x*x;
    double x3 = x2*x;
    double x4 = x3*x;
    double x5 = x4*x;
    return 1+x+0.5*x2+(1/6.)*x3+(1/24.)*x4+(1/120.)*x5;
}
```

But, what if we want to generalize it to all floating point types?{.fragment}

Or to all types that have multiplication and addition like matrices, for example?{.fragment}


--

```c++
constexpr auto f(floating_point auto&& x) {
    auto x2 = x*x;
    auto x3 = x2*x;
    auto x4 = x3*x;
    auto x5 = x4*x;
    return 1+x+0.5*x2+(1/6.)*x3+(1/24.)*x4+(1/120.)*x5;
}
```

```c++{.fragment}
template < class T >
concept floating_point = std::is_floating_point_v<T>;
```

```c++{.fragment}
template< class T >
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
```

--

```c++
template <floating_point T>
constexpr auto f(T&& x) {
    auto x2 = x*x;
    auto x3 = x2*x;
    auto x4 = x3*x;
    auto x5 = x4*x;
    return 1+x+0.5*x2+(1/6.)*x3+(1/24.)*x4+(1/120.)*x5;
}
```

--

```c++
template <typename T>
    requires floating_point<T>
constexpr auto f(T&& x) {
    auto x2 = x*x;
    auto x3 = x2*x;
    auto x4 = x3*x;
    auto x5 = x4*x;
    return 1+x+0.5*x2+(1/6.)*x3+(1/24.)*x4+(1/120.)*x5;
}
```

--

```c++
template <typename T>
concept CanAddAndMultiply = requires(T a) {
    { a * a } -> std::same_as<T>;
    { a + a } -> std::same_as<T>;
    { 2.5 * a } -> std::convertible_to<T>;
};
```

```c++{.fragment}
template <typename T>
    requires CanAddAndMultiply<T>
constexpr T f(T&& x) {
    auto x2 = x*x;
    auto x3 = x2*x;
    auto x4 = x3*x;
    auto x5 = x4*x;
    return 1+x+0.5*x2+(1/6.)*x3+(1/24.)*x4+(1/120.)*x5;
}
```

--

```
$ g++ -std=c++20 -o concept_test concept_test.cpp 
concept_test.cpp: In function ‘int main()’:
concept_test.cpp:22:64: error: use of function ‘constexpr T f(T&&) [with T = const char (&)[14]]’ with unsatisfied constraints
   22 |     std::cout << "f(\"Hello, world!\") = " << f("Hello, world!") << '\n'; // Throws an error because strings are not addable and multiplicable
      |                                                                ^
concept_test.cpp:12:13: note: declared here
   12 | constexpr T f(T&& x) {
      |             ^
concept_test.cpp:12:13: note: constraints not satisfied
concept_test.cpp: In instantiation of ‘constexpr T f(T&&) [with T = const char (&)[14]]’:
concept_test.cpp:22:64:   required from here
concept_test.cpp:4:9:   required for the satisfaction of ‘CanAddAndMultiply<T>’ [with T = const char (&)[14]]
concept_test.cpp:4:29:   in requirements with ‘T a’ [with T = const char (&)[14]]
concept_test.cpp:5:9: note: the required expression ‘(a * a)’ is invalid
    5 |     { a * a } -> std::same_as<T>;
      |       ~~^~~
concept_test.cpp:6:9: note: the required expression ‘(a + a)’ is invalid
    6 |     { a + a } -> std::same_as<T>;
      |       ~~^~~
concept_test.cpp:7:11: note: the required expression ‘(2.5e+0 * a)’ is invalid
    7 |     { 2.5 * a } -> std::convertible_to<T>;
      |       ~~~~^~~
cc1plus: note: set ‘-fconcepts-diagnostics-depth=’ to at least 2 for more detail
```

--

```
$ g++ -std=c++20 -o concept_test concept_test.cpp
$ ./concept_test 
f(1) = 2.71667
```

https://godbolt.org/z/4rraYd

--

## So, what are Ranges?

--

### Simplistically, a concept(s) to describe iterable containers with ```begin()``` and ```end()```.

For example ```std::vector``` {.fragment}

note: For ex: bidirectional_iterator, random_access_range, forward_range, input_range, …

--

### Thus making ```begin()``` and ```end()``` obsolete in algorithms' arguments.

<div class="fragment">
For example, replacing

```c++
std::vector<int> v { 7, 3, 2, 8, 5 };
std::sort(v.begin(), v.end());
```
</div>

<div class="fragment">
with

```c++
std::vector<int> v { 7, 3, 2, 8, 5 };
std::ranges::sort(v);
```
</div>

--

### All the while (mostly) maintaining compatibility with the "old" STL.

note: views with unreachable sentinels, or with move-only iterators are "less" then compatible

--

```c++
#include <ranges> // new header for ranges and views
#include <algorithms> // old header for new algorithms

namespace std::ranges { … }; // new algorithms and views
namespace std::ranges::views { … }; // adaptors namespace
namespace std::views = std::ranges::views; // shortcut of sorts
```

```c++{.fragment}
namespace rng = std::ranges;
namespace views = std::views;
```

--

## Any questions so far?

---

## What is new in algorithms?

New algorithms are now called "constrained algorithms". {.fragment}

--

```c++
std::unordered_map<int, float> m = {
    {1, 2.3f}, {2, 3.4f}, {3, 4.5f}
};

std::sort(m.begin(), m.end());
```

<div class="fragment">
If we try to compile the above code we get

```
$ g++ -std=c++20 -fconcepts sort_a_map.cpp 2>&1 | wc -l
1228
```
</div>

1228 lines of errors is a bit too much for humans to parse. {.fragment}

https://godbolt.org/z/8berT6 {.fragment}

--

```c++
std::unordered_map<int, float> m = {
    {1, 2.3f}, {2, 3.4f}, {3, 4.5f}
};

std::ranges::sort(m.begin(), m.end());
```

note: Since we have new ranges algorithms, we can immediately drop .begin() and .end()

--

```c++
std::unordered_map<int, float> m = {
    {1, 2.3f}, {2, 3.4f}, {3, 4.5f}
};

std::ranges::sort(m);
```

https://godbolt.org/z/bTehK1

--

```
$ g++ -std=c++20 -fconcepts -o rng_sort_a_map rng_sort_a_map.cpp 
rng_sort_a_map.cpp: In function ‘int main()’:
rng_sort_a_map.cpp:16:21: error: no match for call to ‘(const std::ranges::__sort_fn) (std::unordered_map<int, float>&)’
   16 |  std::ranges::sort(m);
      |                     ^
In file included from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/algorithm:64,
                 from rng_sort_a_map.cpp:2:
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/ranges_algo.h:2017:7: note: candidate: ‘template<class _Iter, class _Sent, class _Comp, class _Proj>  requires (random_access_iterator<_Iter>) && (sentinel_for<_Sent, _Iter>) && (sortable<_Iter, _Comp, _Proj>) constexpr _Iter std::ranges::__sort_fn::operator()(_Iter, _Sent, _Comp, _Proj) const’
 2017 |       operator()(_Iter __first, _Sent __last,
      |       ^~~~~~~~
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/ranges_algo.h:2017:7: note:   template argument deduction/substitution failed:
rng_sort_a_map.cpp:16:21: note:   candidate expects 4 arguments, 1 provided
   16 |  std::ranges::sort(m);
      |                     ^
In file included from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/algorithm:64,
                 from rng_sort_a_map.cpp:2:
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/ranges_algo.h:2030:7: note: candidate: ‘constexpr std::ranges::borrowed_iterator_t<_Range> std::ranges::__sort_fn::operator()(_Range&&, _Comp, _Proj) const [with _Range = std::unordered_map<int, float>&; _Comp = std::ranges::less; _Proj = std::identity; std::ranges::borrowed_iterator_t<_Range> = std::conditional<true, std::__detail::_Node_iterator<std::pair<const int, float>, false, false>, std::ranges::dangling>::type]’
 2030 |       operator()(_Range&& __r, _Comp __comp = {}, _Proj __proj = {}) const
      |       ^~~~~~~~
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/ranges_algo.h:2030:7: note: constraints not satisfied
In file included from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/iterator_concepts.h:35,
                 from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/stl_iterator_base_types.h:71,
                 from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/stl_construct.h:61,
                 from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/alloc_traits.h:33,
                 from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/ext/alloc_traits.h:34,
                 from /nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/unordered_map:41,
                 from rng_sort_a_map.cpp:1:
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/concepts: In instantiation of ‘constexpr std::ranges::borrowed_iterator_t<_Range> std::ranges::__sort_fn::operator()(_Range&&, _Comp, _Proj) const [with _Range = std::unordered_map<int, float>&; _Comp = std::ranges::less; _Proj = std::identity; std::ranges::borrowed_iterator_t<_Range> = std::conditional<true, std::__detail::_Node_iterator<std::pair<const int, float>, false, false>, std::ranges::dangling>::type]’:
rng_sort_a_map.cpp:16:21:   required from here
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/concepts:67:13:   required for the satisfaction of ‘derived_from<typename std::__detail::__iter_concept_impl<_Iter>::type, std::bidirectional_iterator_tag>’ [with _Iter = std::__detail::_Node_iterator<std::pair<const int, float>, false, false>]
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/iterator_concepts.h:580:13:   required for the satisfaction of ‘bidirectional_iterator<decltype (std::__detail::__ranges_begin(declval<_Container&>()))>’ [with _Container = std::unordered_map<int, float, std::hash<int>, std::equal_to<int>, std::allocator<std::pair<const int, float> > >&]
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/range_access.h:918:13:   required for the satisfaction of ‘bidirectional_range<_Tp>’ [with _Tp = std::unordered_map<int, float, std::hash<int>, std::equal_to<int>, std::allocator<std::pair<const int, float> > >&]
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/bits/range_access.h:923:13:   required for the satisfaction of ‘random_access_range<_Range>’ [with _Range = std::unordered_map<int, float, std::hash<int>, std::equal_to<int>, std::allocator<std::pair<const int, float> > >&]
/nix/store/zk6qfymg2wp15ghx737p7dz2yk3807ij-gcc-10.2.0/include/c++/10.2.0/concepts:67:28: note:   ‘std::bidirectional_iterator_tag’ is not a base of ‘std::forward_iterator_tag’
   67 |     concept derived_from = __is_base_of(_Base, _Derived)
      |                            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```

--

To summarize, new, constrained algorithms allow us to not use ```.begin()``` and ```.end()```, while making potential error messages a lot more usable.

But that is not all. {.fragment}

--

## Any questions so far?

---

## Projections

--

* Additional argument for algorithms
* That allows users to change which data is viewed by the algorithm {.fragment}
* It can be any *callable* that takes underlying data as it's only argument {.fragment}

--

```c++
struct Point {
    float x, y, z;
};

struct Face {
    std::array<Point, 3> points;
};

std::vector<Face> faces;
```

--

```c++
std::min_element(someFace.points.begin(), someFace.points.end(),
                [](const Point& p1, const Point& p2) {
                    return p1.z < p2.z;
                });
```

--

```c++
std::ranges::min_element(someFace.points,
                [](const Point& p1, const Point& p2) {
                    return p1.z < p2.z;
                });
```

--

```c++
template< ranges::forward_range R, class Proj = std::identity,
          std::indirect_strict_weak_order<
              std::projected<ranges::iterator_t<R>, Proj>>
              Comp = ranges::less >
constexpr ranges::borrowed_iterator_t<R>
min_element( R&& r, Comp comp = {}, Proj proj = {} );
```

--

```c++
std::ranges::min_element(someFace.points, std::ranges::less,
    [](const Point& p){
        return p.z;
    });
```

--

Remember

```c++ {.fragment data-code-focus=4}
template< ranges::forward_range R, class Proj = std::identity,
          std::indirect_strict_weak_order<
              std::projected<ranges::iterator_t<R>, Proj>>
              Comp = ranges::less >
constexpr ranges::borrowed_iterator_t<R>
min_element( R&& r, Comp comp = {}, Proj proj = {} );
```

--

```c++
std::ranges::min_element(someFace.points, {},
    [](const Point& p){
        return p.z;
    });
```

--

```c++
std::invoke(&Point::z, p)
```

is the same as {.fragment}

```c++ {.fragment}
p.z
```

--

Also

```c++ {.fragment}
std::invoke(&SomeClass::memberFunction, instance, args)
```

is the same as {.fragment}

```c++ {.fragment}
instance.memberFunction(args)
```

not that it matters for this example :) {.fragment}

--

```c++
std::ranges::min_element(someFace.points, {}, &Point::z);
```

This returns an iterator {.fragment}

--

```c++
std::ranges::min(someFace.points, {}, &Point::z);
```

--

```c++
std::sort(faces.begin(), faces.end(),
         [](const Face& f1, const Face& f2) {
             return std::ranges::min(f1, {}, &Point::z).z
                  < std::ranges::min(f2, {}, &Point::z).z
         });
```

--

```c++
std::sort(faces.begin(), faces.end(),
         [](const Face& f1, const Face& f2) {
             return 
           std::min_element(f1.points.begin(), f1.points.end(),
                [](const Point& p1, const Point& p2) {
                    return p1.z < p2.z;
                })->z
         < std::min_element(f2.points.begin(), f2.points.end(),
                [](const Point& p1, const Point& p2) {
                    return p1.z < p2.z;
                })->z;

         });
```

{.fragment .current-only data-code-focus=5-7,9-11}

--

```c++
std::sort(faces.begin(), faces.end(),
         [](const Face& f1, const Face& f2) {
             return std::ranges::min(f1, {}, &Point::z).z
                  < std::ranges::min(f2, {}, &Point::z).z
         });
```

--

```c++
std::ranges::sort(faces,
         [](const Face& f1, const Face& f2) {
             return std::ranges::min(f1, {}, &Point::z).z
                  < std::ranges::min(f2, {}, &Point::z).z
         });
```

--

```c++
template< std::random_access_range R, class Comp = ranges::less,
          class Proj = std::identity >
requires sortable<ranges::iterator_t<R>, Comp, Proj>
constexpr ranges::iterator_t<R> sort( R&& r, Comp comp = {},
                                      Proj proj = {} );
```

--

```c++
std::ranges::sort(faces, {}, [](const Face& face){
    return std::ranges::min(face.points, {}, &Point::z).z;
});
```

https://godbolt.org/z/TdEWhK {.fragment}

--

Note: only algorithms in ```<algorithm>``` header have been "rangeified".

Hopefully, ```<numeric>``` will receive the same treatment in C++23. {.fragment}

Although if the "overall plan for C++23" is to be trusted, ranges are not on agenda. {.fragment}

https://wg21.link/p0592 {.fragment}


--

## Any questions so far?


---

## What are Views?

--

* Views are lazy and cheap
* Lazy and cheap is good (sometimes) {.fragment}
* Views are, basically, algorithms that can be bound with containers {.fragment}
* copied around for free {.fragment}
* and executed if and when needed. {.fragment}

note: Explain lazy and cheap nicely.

--

* Views are (almost) stateless
* The only data they contain is
* Whatever data is needed for the view (```take(10)``` for example)
* And a reference to the "source" view
* Containers are not views
* But, containers can be wrapped with a ```rev_view```

note: Actually storage is done by chaining templates

--

Let us see an example from the very cheap and quite lazy language

Haskell{.fragment}

--

```haskell
evenSquares :: [Int] -> [Int]
evenSquares l = map (^2) $ filter even l
```

Who can guess what this code does? {.fragment}

Absolutely nothing. {.fragment}

https://haskell.godbolt.org/z/osssj1 {.fragment}

--

```haskell
evenSquares :: [Int] -> [Int]
evenSquares l = map (^2) $ filter even l

main :: IO ()
main = do
    let es = evenSquares [ 1, 2, 3, 4, 5 ]
    return ()
```

```x86asm
Main_$trModule4_bytes:
        .asciz "main"
Main_$trModule3_closure:
        .quad   ghczmprim_GHCziTypes_TrNameS_con_info
        .quad   Main_$trModule4_bytes
Main_$trModule2_bytes:
        .asciz "Main"
Main_$trModule1_closure:
        .quad   ghczmprim_GHCziTypes_TrNameS_con_info
        .quad   Main_$trModule2_bytes
Main_$trModule_closure:
        .quad   ghczmprim_GHCziTypes_Module_con_info
        .quad   Main_$trModule3_closure+1
        .quad   Main_$trModule1_closure+1
        .quad   3
Main_main1_info:
        movl $ghczmprim_GHCziTuple_Z0T_closure+1,%ebx
        jmp *(%rbp)
Main_main1_closure:
        .quad   Main_main1_info
Main_main_info:
        jmp Main_main1_info
Main_main_closure:
        .quad   Main_main_info
Main_main2_info:
        movl $Main_main1_closure+1,%r14d
        jmp base_GHC.TopHandler_runMainIO1_info
Main_main2_closure:
        .quad   Main_main2_info
        .quad   0
:Main_main_info:
        jmp Main_main2_info
:Main_main_closure:
        .quad   :Main_main_info
        .quad   0
```

--

```haskell
evenSquares :: [Int] -> [Int]
evenSquares l = map (^2) $ filter even l

main :: IO ()
main = print $ evenSquares [1, 2, 3, 4, 5]
```

Only IO operations make the code to execute {.fragment}

https://haskell.godbolt.org/z/EevKbz {.fragment}

--

```c++
void even_squares(std::vector<int>& v) {
    auto rm = std::ranges::remove_if(v, [](int i) {
        return i % 2 != 0;
    });
    v.erase(rm.begin(), rm.end());
    std::ranges::transform(v, v.begin(), [](int i) {
        return i * i;
    });
}

void print_vector(std::vector<int>& v) {
    even_squares(v);
    std::ranges::copy(v,
        std::ostream_iterator<int>(std::cout, " "));
}
```

note: This changed the actual vector

--

```x86asm
even_squares(std::vector<int, std::allocator<int> >&):
        mov     rsi, QWORD PTR [rdi+8]
        mov     r8, QWORD PTR [rdi]
        cmp     rsi, r8
        je      .L26
        push    rbp
        mov     rax, r8
        push    rbx
        mov     rbx, rdi
        sub     rsp, 8
        jmp     .L4
.L30:
        add     rax, 4
        mov     rdx, rax
        cmp     rsi, rax
        je      .L20
.L4:
        mov     rcx, rax
        test    BYTE PTR [rax], 1
        je      .L30
        cmp     rsi, rax
        je      .L21
        lea     r9, [rax+4]
        cmp     rsi, r9
        je      .L6
        mov     rdx, r9
.L8:
        mov     edi, DWORD PTR [rdx]
        test    dil, 1
        jne     .L7
        mov     DWORD PTR [rcx], edi
        add     rcx, 4
.L7:
        add     rdx, 4
        cmp     rdx, rsi
        jne     .L8
        lea     rbp, [rdx-4]
        sub     rbp, rax
        add     rbp, r9
        cmp     rcx, rbp
        je      .L20
        mov     r9, rdx
        sub     r9, rbp
        cmp     rdx, rbp
        je      .L10
        mov     rdx, r9
        mov     rdi, rcx
        mov     rsi, rbp
        call    memmove
        mov     rdx, QWORD PTR [rbx+8]
        mov     r8, QWORD PTR [rbx]
        mov     rcx, rax
        mov     rax, rdx
        sub     rax, rbp
        add     rcx, rax
        cmp     rdx, rcx
        jne     .L11
.L12:
        cmp     rdx, r8
        je      .L1
.L20:
        lea     rcx, [rdx-4]
        mov     rax, r8
        sub     rcx, r8
        mov     rsi, rcx
        shr     rsi, 2
        add     rsi, 1
        cmp     rcx, 8
        jbe     .L14
        mov     rcx, rsi
        shr     rcx, 2
        sal     rcx, 4
        add     rcx, r8
.L15:
        movdqu  xmm0, XMMWORD PTR [rax]
        add     rax, 16
        movdqa  xmm1, xmm0
        pmuludq xmm1, xmm0
        psrlq   xmm0, 32
        pmuludq xmm0, xmm0
        pshufd  xmm1, xmm1, 8
        pshufd  xmm0, xmm0, 8
        punpckldq       xmm1, xmm0
        movups  XMMWORD PTR [rax-16], xmm1
        cmp     rax, rcx
        jne     .L15
        mov     rax, rsi
        and     rax, -4
        lea     r8, [r8+rax*4]
        cmp     rsi, rax
        je      .L1
.L14:
        mov     eax, DWORD PTR [r8]
        imul    eax, eax
        mov     DWORD PTR [r8], eax
        lea     rax, [r8+4]
        cmp     rdx, rax
        je      .L1
        mov     eax, DWORD PTR [r8+4]
        imul    eax, eax
        mov     DWORD PTR [r8+4], eax
        lea     rax, [r8+8]
        cmp     rdx, rax
        je      .L1
        mov     eax, DWORD PTR [r8+8]
        imul    eax, eax
        mov     DWORD PTR [r8+8], eax
.L1:
        add     rsp, 8
        pop     rbx
        pop     rbp
        ret
.L6:
        xor     r9d, r9d
.L10:
        lea     rdx, [rcx+r9]
.L19:
        mov     QWORD PTR [rbx+8], rdx
        jmp     .L12
.L11:
        mov     rdx, rcx
        jmp     .L19
.L26:
        ret
.L21:
        mov     rdx, rsi
        jmp     .L20
```

https://godbolt.org/z/3d7xMW

Total number of lines (with -O3) is 167 {.fragment}

--

```c++
auto even_squares(std::vector<int>& v) {
    auto even = std::ranges::views::filter(v, [](int i) {
            return i % 2 == 0;
        });
    auto evenSquared = std::ranges::views::transform(even,
        [](int i) {
            return i * i;
        });
    return evenSquared;
}

void print_vector(std::vector<int>& v) {
    std::ranges::copy(even_squares(v),
        std::ostream_iterator<int>(std::cout, " "));
}
```

--

```x86asm
even_squares(std::vector<int, std::allocator<int> >&):
        mov     QWORD PTR [rdi+16], rsi
        mov     rax, rdi
        mov     BYTE PTR [rdi+25], 1
        mov     QWORD PTR [rdi+32], -1
        mov     BYTE PTR [rdi+41], 1
        ret
```

https://godbolt.org/z/3nG5aq

Total number of lines (with -O3) is 66 {.fragment}

note: LoC for old-style was 167

--

```c++
auto even_squares(const std::vector<int>& v) {
    auto even = std::ranges::views::filter(v, [](int i) {
            return i % 2 == 0;
        });
    auto evenSquared = std::ranges::views::transform(even,
        [](int i) {
            return i * i;
        });
    return evenSquared;
}

void print_vector(const std::vector<int>& v) {
    std::ranges::copy(even_squares(v),
        std::ostream_iterator<int>(std::cout, " "));
}
```

https://godbolt.org/z/Gnn49h

Total number of lines (with -O3) is 66 {.fragment}

note: No need for non-const reference, since we are now not touching the vector

--

There is a special overload of ```operator|``` for views that allows their easy composition.

--

```c++
auto even_squares(const std::vector<int>& v) {
    auto even = [](int i) { return i % 2 == 0; };
    auto square = [](int i) { return i * i; };

    return v | std::views::filter(even)
             | std::views::transform(square);
}

void print_vector(const std::vector<int>& v) {
    std::ranges::copy(even_squares(v),
        std::ostream_iterator<int>(std::cout, " "));
}
```

note: std::vector is not a view

--

```c++
auto even_squares(const std::vector<int>& v) {
    auto even = [](int i) { return i % 2 == 0; };
    auto square = [](int i) { return i * i; };

    return std::views::all(v)
         | std::views::filter(even)
         | std::views::transform(square);
}

void print_vector(const std::vector<int>& v) {
    std::ranges::copy(even_squares(v),
        std::ostream_iterator<int>(std::cout, " "));
}
```

note: vector is not even needed

--

```c++
auto even_squares() {
    auto even = [](int i) { return i % 2 == 0; };
    auto square = [](int i) { return i * i; };

    return std::views::filter(even)
         | std::views::transform(square);
}

void print_vector(const std::vector<int>& v) {
    std::ranges::copy(v | even_squares(),
        std::ostream_iterator<int>(std::cout, " "));
}
```

note: neither is the function

--

```c++
auto even = [](int i) { return i % 2 == 0; };
auto square = [](int i) { return i * i; };
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);

void print_vector(const std::vector<int>& v) {
    std::ranges::copy(v | even_squares,
        std::ostream_iterator<int>(std::cout, " "));
}
```

--

Now

```c++ {.fragment}
auto even = [](int i) { return i % 2 == 0; };
auto square = [](int i) { return i * i; };
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);
```

is mostly equivalent to {.fragment}

```haskell {.fragment}
evenSquares :: [Int] -> [Int]
evenSquares l = map (^2) $ filter even l
```

--

```c++
auto even = [](int i) { return i % 2 == 0; };
auto square = [](int i) { return i * i; };
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);
```

```haskell
evenSquares :: [Int] -> [Int]
evenSquares = map (^2) $ filter even
```

note: Even more if arguments are left out in Haskell…

--

## What is the point?

* Lazy evaluation allows us to stack an infinite number of algorithms {.fragment}
* Nothing is executed before it is needed {.fragment}
* Meaning when the consumer requests the next item from the view, only then are operations executed and only for the next item {.fragment}
* But, beware, reading multiple times from a view yields multiple executions of all operations {.fragment}

note: Views work by chaining types in templates, so it's not infinite if there was a doubt

--

```c++
auto even = [](int i) { 
    std::cout << "even(" << i << ")\n";
    return i % 2 == 0;};
auto square = [](int i) {
    std::cout << "square(" << i << ")\n";
    return i * i;};
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);
std::vector<int> v { 1, 2, 3, 4, 5, 6 };
for (int i : v | even_squares )
    std::cout << "for: i=" << i << '\n';
```

note: Let's test those multiple passes

--

``` 
even(1)
even(2)
square(2)
for: i=4
even(3)
even(4)
square(4)
for: i=16
even(5)
even(6)
square(6)
for: i=36
```

--

```c++
auto even = [](int i) { 
    std::cout << "even(" << i << ")\n";
    return i % 2 == 0;};
auto square = [](int i) {
    std::cout << "square(" << i << ")\n";
    return i * i;};
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);
std::vector<int> v { 1, 2, 3, 4, 5, 6 };
for (int i : v | even_squares )
    std::cout << "for: i=" << i << '\n';
std::cout << '\n';
for (int i : v | even_squares)
    std::cout << "for: i=" << i << '\n';
```

--

```
even(1)
even(2)
square(2)
for: i=4
even(3)
even(4)
square(4)
for: i=16
even(5)
even(6)
square(6)
for: i=36

even(1)
even(2)
square(2)
for: i=4
even(3)
even(4)
square(4)
for: i=16
even(5)
even(6)
square(6)
for: i=36
```

--

```c++
auto even = [](int i) { 
    std::cout << "even(" << i << ")\n";
    return i % 2 == 0;};
auto square = [](int i) {
    std::cout << "square(" << i << ")\n";
    return i * i;};
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);
std::vector<int> v { 1, 2, 3, 4, 5, 6 };
auto es = v | even_squares;
for (int i : es )
    std::cout << "for: i=" << i << '\n';
std::cout << '\n';
for (int i : es)
    std::cout << "for: i=" << i << '\n';
```

--

```
even(1)
even(2)
square(2)
for: i=4
even(3)
even(4)
square(4)
for: i=16
even(5)
even(6)
square(6)
for: i=36

even(2)
square(2)
for: i=4
even(3)
even(4)
square(4)
for: i=16
even(5)
even(6)
square(6)
for: i=36
```

--

What can we do about that?

--

```c++
auto even = [](int i) { 
    std::cout << "even(" << i << ")\n";
    return i % 2 == 0;};
auto square = [](int i) {
    std::cout << "square(" << i << ")\n";
    return i * i;};
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);
std::vector<int> v { 1, 2, 3, 4, 5, 6 };
std::vector<int> es;
std::ranges::copy(v | even_squares, std::back_inserter(es));
for (int i : es )
    std::cout << "for: i=" << i << '\n';
for (int i : es)
    std::cout << "for: i=" << i << '\n';
```

{.fragment data-code-focus=10-11}

note: it's probably a good idea to pre-reserve() es's memory.

--

```
even(1)
even(2)
square(2)
even(3)
even(4)
square(4)
even(5)
even(6)
square(6)
for: i=4
for: i=16
for: i=36
for: i=4
for: i=16
for: i=36
```

--

I mentioned containers not being views.

Our ```std::vector``` got wrapped by ```std::views::all``` into a ```std::ranges::rev_view```. {.fragment}

The views don't hold data, that enables them to be cheap. {.fragment}

So, ```rev_view``` doesn't hold the ```vector```, only a reference to it (a pointer). {.fragment}

--

That brings us to this example

```c++
auto even = [](int i) { 
    return i % 2 == 0;
};
auto square = [](int i) {
    return i * i;
};
auto even_squares = std::views::filter(even)
                  | std::views::transform(square);

for (int i : { 1, 2, 3, 4, 5, 6 } | even_squares )
    std::cout << "for: i=" << i << '\n';
```

--

```
$ g++ -std=c++20 -fconcepts rev_view_test.cpp 
rev_view_test.cpp: In function ‘int main()’:
rev_view_test.cpp:15:38: error: expected ‘)’ before ‘|’ token
   15 |     for (int i : { 1, 2, 3, 4, 5, 6 } | even_squares)
      |         ~                            ^~
      |                                      )
rev_view_test.cpp:15:39: error: expected primary-expression before ‘|’ token
   15 |     for (int i : { 1, 2, 3, 4, 5, 6 } | even_squares)
      |                                       ^
```

--

Only lvalue containers can become views.

Rvalues are not stored in memory, {.fragment}

so there is nothing the pointer can point to {.fragment}

and views don't hold any (not trivially copyable) data. {.fragment}

--

## Any questions so far?

---

## Have I mentioned infinite ranges?

--

Ranges are something with ```begin()``` and ```end()``` that is iterable.

Like a ```std::vector``` {.fragment}

But, unlike it, ranges have two iterator types. {.fragment}

One for ```begin()``` {.fragment}

and one for the ```end()```. {.fragment}

--

So, what the hell is a range?

```c++ {.fragment}
template< class T >

concept range = requires(T& t) {
  ranges::begin(t);
  ranges::end  (t);
};
```

--

Well, what is ```ranges::begin```?

```c++ {.fragment}
constexpr std::input_or_output_iterator auto begin(T&& t);
```

```c++ {.fragment}
template <class I>

concept input_or_output_iterator =
  requires(I i) {
    { *i } -> __Referenceable; // mostly not void
  } &&
  std::weakly_incrementable<I>;
```

```c++ {.fragment}
template<class I>
concept weakly_incrementable =
    std::default_initializable<I> && std::movable<I> &&
    requires(I i) {
        typename std::iter_difference_t<I>;
        requires <std::iter_difference_t<I>>;
        { ++i } -> std::same_as<I&>;
        i++;
    };
```

Ok, so, basically an iterator just like before. {.fragment}

note: __Referenceable is satisfied if and only if the expression *std::declval<I&>() is valid and has a referenceable type (in particular, not void).

--

How about ```ranges::end```?

```c++ {.fragment}
constexpr
std::sentinel_for<ranges::iterator_t<T>> auto end(T&& t);
```

--

<!-- .slide: data-background="https://media.giphy.com/media/LQiq27myXGPXO6WzAE/giphy.gif" -->

Sentinel? Is that from the Matrix?

--

<!-- .slide: data-background="https://media.giphy.com/media/LQiq27myXGPXO6WzAE/giphy.gif" -->

```c++
template<class S, class I>
concept sentinel_for =
    std::semiregular<S> &&
    std::input_or_output_iterator<I> &&
    __WeaklyEqualityComparableWith<S, I>;
```

```c++ {.fragment}
template <class T>
concept semiregular = std::copyable<T> &&
                      std::default_initializable<T>;
```

```c++ {.fragment}
template<class T, class U>
concept __WeaklyEqualityComparableWith =
    requires(const std::remove_reference_t<T>& t,
             const std::remove_reference_t<U>& u) {
      { t == u } -> boolean-testable;
      { t != u } -> boolean-testable;
      { u == t } -> boolean-testable;
      { u != t } -> boolean-testable;
    };
```

--

<!-- .slide: data-background="https://media.giphy.com/media/LQiq27myXGPXO6WzAE/giphy.gif" -->

So, what?

Sentinels are some objects {.fragment}

comparable with iterators, {.fragment}

that one can copy, {.fragment}

initialize, {.fragment}

increment {.fragment}

and that's it? {.fragment}

--

<!-- .slide: data-background="https://media.giphy.com/media/LQiq27myXGPXO6WzAE/giphy.gif" -->

Well,

```c++ {.fragment}
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
```


--

<!-- .slide: data-background="https://media.giphy.com/media/LQiq27myXGPXO6WzAE/giphy.gif" -->

```
3238586 3238587 3238588 3238589 3238590 3238591 3238592 
3238593 3238594 3238595 3238596 3238597 3238598 3238599 
3238600 3238601 3238602 3238603 3238604 3238605 3238606 
3238607 3238608 3238609 3238610 3238611 3238612 3238613 
3238614 3238615 3238616 3238617 3238618 3238619 3238620 
3238621 3238622 3238623 3238624 3238625 3238626 3238627 
3238628 3238629 3238630 3238631 3238632 3238633 3238634 
3238635 3238636 3238637 3238638 3238639 3238640 3238641 
3238642 3238643 3238644 3238645 3238646 3238647 3238648 
3238649 3238650 3238651 3238652 3238653 3238654 3238655 
3238656 3238657 3238658 3238659 3238660 3238661 3238662 
3238663 3238664 3238665 3238666 3238667 3238668 3238669 
3238670 3238671 3238672 3238673 3238674 3238675 3238676 
3238677 3238678 3238679 3238680 32386^C
```

--

Sentinels are some objects {.fragment}

that are comparable with iterators {.fragment}

that enable {.fragment}

infinite loops that one knows will end in some other way {.fragment}

or {.fragment}

infinite data streams to be consumed until the consumers are satisfied {.fragment}

which is, hopefully, before infinite time has passed. {.fragment}

note: Manually setting unreachable_sentinel to get an inifinite loop

--

That's why ranges and views are "mostly" compatible with old-style STL.

Thankfully, there is a ```std::views::common``` that converts a view into a ```common_range```. {.fragment}

```c++ {.fragment}
template<class T>
  concept common_range =
    ranges::range<T> &&
    std::same_as<ranges::iterator_t<T>, ranges::sentinel_t<T>>;
```

```c++ {.fragment}
  std::views::iota(1)
| std::views::transform([](int i}{return i*i;}))
| std::views::take(50)
| std::views::common
```

--

## Any questions so far?

---

## Current status


--

Currently, ranges are supported only by GCC 10.x.

As far as I've heard, MSVC support will be there soon. {.fragment}

--

But, there are alternatives.

--

Eric Niebler's range-v3

https://github.com/ericniebler/range-v3

A proto-range library, a basis for the proposal to include ranges in C++. {.fragment}

Supports C++14, 17 and 20. {.fragment}

Supports more than C++20 ranges (actions, more views and algorithms) {.fragment}


--

Casey Carter's cmcstl2

https://github.com/CaseyCarter/cmcstl2

An implementation of the ["The One Ranges Proposal"](https://wg21.link/p0896r4), which is now C++20 ranges. {.fragment}

Supports C++17. {.fragment}

--

Tristan Brindle's NanoRange

https://github.com/tcbrindle/NanoRange

A C++17 implementation of C++20 ranges. {.fragment}

Supports GCC, CLang and MSVC. {.fragment}

Has(can have) a single-header for include, so it can be used on Compiler-Explorer. {.fragment}

---

## Useful links

--

One Ranges Proposal: https://wg21.link/p0896r4

Standard Library Concepts: https://wg21.link/p0898

C++ standard's draft: https://wg21.link/n4868

Original Ranges proposal: https://wg21.link/N4128

--

CppReference - Ranges: https://en.cppreference.com/w/cpp/ranges

and Algorithms: https://en.cppreference.com/w/cpp/algorithm/ranges

---

## Questions?

---

## Thank you

Presentation: https://cxx20ranges.brcha.com

Source: https://github.com/brcha/cxx20ranges

Filip Brčić &lt;br<span style="display: none">nospam</span>cha@yandex<span style="display: none">nospam</span>.com&gt;