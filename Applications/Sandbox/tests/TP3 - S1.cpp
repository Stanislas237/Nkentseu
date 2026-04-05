#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Vec2d.h"

using namespace nkentseu::math;

// --------------------------------  TP3 : 33 tests unitaires sur Float.h    
TEST_CASE(Semaine1_TP3, TestsSurFloath) {
    // 1. isFiniteValid (5 tests)    
    ASSERT_TRUE(!isFiniteValid(std::numeric_limits<float>::quiet_NaN())); // 1
    ASSERT_TRUE(!isFiniteValid(std::numeric_limits<float>::infinity()));  // 2
    ASSERT_TRUE(!isFiniteValid(-std::numeric_limits<float>::infinity())); // 3
    ASSERT_TRUE(isFiniteValid(0.0f));                                     // 4
    ASSERT_TRUE(isFiniteValid(1.0f));                                     // 5

    // 2. nearlyZero (8 tests)
    ASSERT_TRUE(nearlyZero(0.0f, 1e-6f));     // 6
    ASSERT_TRUE(!nearlyZero(1e-5f, 1e-6f));   // 7
    ASSERT_TRUE(nearlyZero(1e-7f, 1e-6f));    // 8

    ASSERT_TRUE(nearlyZero(-1e-7f, 1e-6f));   // 9
    ASSERT_TRUE(!nearlyZero(-1e-6f, 1e-7f));  // 10

    ASSERT_TRUE(nearlyZero(1e-3f, 1e-2f));    // 11
    ASSERT_TRUE(!nearlyZero(1e-2f, 1e-3f));   // 12

    ASSERT_TRUE(nearlyZero(5e-8f, 1e-7f));    // 13

    // 3. approxEq (10 tests)
    ASSERT_TRUE(approxEq(1.0f, 1.0f, 1e-6f));           // 14
    ASSERT_TRUE(approxEq(1.0f, 1.0000001f, 1e-5f));     // 15
    ASSERT_TRUE(!approxEq(1.0f, 1.1f, 1e-3f));          // 16

    ASSERT_TRUE(approxEq(0.0f, 1e-7f, 1e-6f));          // 17
    ASSERT_TRUE(!approxEq(0.0f, 1e-4f, 1e-6f));         // 18

    ASSERT_TRUE(approxEq(-1.0f, -1.000001f, 1e-5f));    // 19
    ASSERT_TRUE(!approxEq(-1.0f, -1.1f, 1e-2f));        // 20

    ASSERT_TRUE(approxEq(1000.0f, 1000.0001f, 1e-3f));  // 21
    ASSERT_TRUE(approxEq(1000.0f, 1001.0f, 1e-3f));     // 22

    ASSERT_TRUE(approxEq(1e-7f, 2e-7f, 1e-6f));         // 23

    // 4. kahanSum vs accumulate (10 tests)  
    float s1, s2;
    std::vector<float> v;
  
    v = std::vector<float>(1000, 0.1f);
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    ASSERT_TRUE(std::fabs(s2 - 100.0f) < std::fabs(s1 - 100.0f)); // 24
    
    v = std::vector<float>(10000, 0.1f);
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    ASSERT_TRUE(std::fabs(s2 - 1000.0f) < std::fabs(s1 - 1000.0f)); // 25
    
    v = std::vector<float>({1e8f, 1.0f, -1e8f});
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    ASSERT_TRUE(std::fabs(s2 - 1.0f) <= std::fabs(s1 - 1.0f)); // 26

    v = std::vector<float>({1.0f, 1e8f, -1e8f});
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    ASSERT_TRUE(std::fabs(s2 - 1.0f) <= std::fabs(s1 - 1.0f)); // 27
    
    v = std::vector<float>(100000, 0.01f);
    s2 = kahanSum(v);
    ASSERT_TRUE(approxEq(s2, 1000.0f, 1e-2f)); // 28

    v = std::vector<float>(100000, 1e-5f);
    s2 = kahanSum(v);
    ASSERT_TRUE(approxEq(s2, 1.0f, 1e-3f)); // 29
    
    v = std::vector<float>({0.1f, 0.2f, 0.3f});
    s2 = kahanSum(v);
    ASSERT_TRUE(approxEq(s2, 0.6f, 1e-6f)); // 30

    v = std::vector<float>(50000, 0.2f);
    s2 = kahanSum(v);
    ASSERT_TRUE(approxEq(s2, 10000.0f, 1e-2f)); // 31
    
    v = std::vector<float>({1e7f, 1.0f, 1.0f, -1e7f});
    s2 = kahanSum(v);
    ASSERT_TRUE(approxEq(s2, 2.0f, 1e-3f)); // 32

    v = std::vector<float>(1000000, 0.1f);
    s2 = kahanSum(v);
    ASSERT_TRUE(approxEq(s2, 100000.0f, 1e-1f)); // 33
}
