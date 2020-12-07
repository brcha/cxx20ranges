#include <array>
#include <vector>
#include <algorithm>
#include <iostream>

struct Point {
    float x, y, z;
};

struct Face {
    std::array<Point, 3> points;
};

void print_point(const Point& p) {
    std::cout << '(' << p.x << ',' << p.y << ',' << p.z << ')';
}

void print_face(const Face& f) {
    std::cout << "[ ";
    std::ranges::for_each(f.points, [](const Point &p) {
        print_point(p);
        std::cout << ' ';
    });
    std::cout << "]\n";
}

int main() {
    std::vector<Face> faces {
        { 12, 13, 14, 15, 16, 17, 18, 19, 20 } ,
        { 7, 8, 9, 4, 5, 6, 1, 2, 3 },
        { 5, 5, 5, 5, 5, 5, 5, 5, 5 },
        { 7, 2, 1, 4, 9, 2, 0, 4, -3 }
    };

    std::ranges::sort(faces, {}, [](const Face& face){
        return std::ranges::min(face.points, {}, &Point::z).z;
    });

    std::ranges::for_each(faces, print_face);
}
