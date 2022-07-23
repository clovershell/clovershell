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

/*
cpuinfo
vendor_id       : GenuineIntel
cpu family      : 6
model           : 85
model name      : Intel(R) Xeon(R) Gold 6226R CPU @ 2.90GHz
stepping        : 7
microcode       : 0x5003302
cpu MHz         : 1200.121
cache size      : 22528 KB
physical id     : 1
siblings        : 32
core id         : 15
cpu cores       : 16
apicid          : 63
initial apicid  : 63
fpu             : yes
fpu_exception   : yes
cpuid level     : 22
wp              : yes
flags           : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb cat_l3 cdp_l3 invpcid_single intel_ppin ssbd mba ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid cqm mpx rdt_a avx512f avx512dq rdseed adx smap clflushopt clwb intel_pt avx512cd avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local dtherm ida arat pln pts pku ospke avx512_vnni md_clear flush_l1d arch_capabilities
bugs            : spectre_v1 spectre_v2 spec_store_bypass swapgs taa itlb_multihit mmio_stale_data
bogomips        : 5802.17
clflush size    : 64
cache_alignment : 64
address sizes   : 46 bits physical, 48 bits virtual
power management:
*/
