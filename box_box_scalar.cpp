#include <assert.h>

#include <chrono>
#include <iostream>

struct Point2D
{
    float x;
    float y;
};

struct Rect2D
{
    Point2D mininum;
    Point2D maxnum;
};

#define AND &&

inline bool intersectld(float a_mininum, float a_maxnum, float b_mininum, float b_maxnum)
{
    return (a_mininum <= b_maxnum) AND(b_mininum <= a_maxnum);
}

inline bool intersect2d(const Rect2D& a_point, const Rect2D& b_point)
{
    return intersectld(a_point.mininum.x, a_point.maxnum.x, b_point.mininum.x, b_point.maxnum.x)
        AND intersectld(a_point.mininum.y, a_point.maxnum.y, b_point.mininum.y, b_point.maxnum.y);
}

static const int N = 4;

__attribute__((noinline)) int test(const Rect2D* boxes1, const Rect2D* boxes2)
{
    int intersects = 0;
    for (int i = 0; i < N; i++)
    {
        intersects += intersect2d(boxes1[i], boxes2[i]);
        asm volatile("" : : "r,m"(intersects) : "memory");
    }
    return intersects;
}

int main()
{
    Rect2D box{{0, 0}, {1, 1}};
    Rect2D boxes1[N] = {box, box, box, box};
    Rect2D boxes2[N] = {box, box, box, box};

    static const int M = 1024 * 1024 * 100;

    int  sum   = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < M; i++)
    {
        sum += test(boxes1, boxes2);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << sum << "\ttime is "
              << std::chrono::duration<double, std::micro>(end - start).count() << " us"
              << "\t" << std::chrono::duration<double, std::milli>(end - start).count() << " ms"
              << std::endl;

    return 0;
}
