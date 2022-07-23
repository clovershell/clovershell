#include <xmmintrin.h>

#include <chrono>
#include <iostream>

typedef __m128 float4;

struct Point2D
{
    float4 x;
    float4 y;
};

struct Rect2D
{
    Point2D mininum;
    Point2D maxnum;
};

// #define AND &&

inline float4 intersectld(float4 a_mininum, float4 a_maxnum, float4 b_mininum, float4 b_maxnum)
{
    float4 i = _mm_cmple_ps(a_mininum, b_maxnum);
    float4 j = _mm_cmple_ps(a_maxnum, b_mininum);
    return _mm_and_ps(i, j);
}

inline int intersect2d(const Rect2D& a_point, const Rect2D& b_point)
{
    float4 i =
        intersectld(a_point.mininum.x, a_point.maxnum.x, b_point.mininum.x, b_point.maxnum.x);
    float4 j =
        intersectld(a_point.mininum.y, a_point.maxnum.y, b_point.mininum.y, b_point.maxnum.y);
    return __builtin_popcount(_mm_movemask_ps(_mm_and_ps(i, j)));
}

__attribute__((noinline)) int test(const Rect2D& boxes1, const Rect2D& boxes2)
{
    int intersects = intersect2d(boxes1, boxes2);
    return intersects;
}


int main()
{
    float  a_minnum[] = {0, 0, 0, 0};
    float  b_minnum[] = {0, 0, 0, 0};
    float  a_maxnum[] = {1, 1, 1, 1};
    float  b_maxnum[] = {1, 1, 1, 1};
    Rect2D boxes1     = {{_mm_load_ps(a_minnum), _mm_load_ps(b_minnum)},
                         {_mm_load_ps(a_maxnum), _mm_load_ps(b_maxnum)}};
    Rect2D boxes2     = boxes1;

    static const int M = 1024 * 1024 * 100;

    int  sum   = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < M; i++)
    {
        sum += test(boxes1, boxes2);
        asm volatile("" : : "r,m"(sum) : "memory");
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << sum << "\ttime is "
              << std::chrono::duration<double, std::micro>(end - start).count() << " us"
              << "\t" << std::chrono::duration<double, std::milli>(end - start).count() << " ms"
              << std::endl;

    return 0;
}
