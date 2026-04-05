#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Vec2d.h"

using namespace nkentseu::math;

// --------------------------------  TP4 : Vec2d complet + 20 implémentations
TEST_CASE(Semaine2_TP1, Vec2dEtImpl) {
    // 1. Dot product (6 tests)
    ASSERT_TRUE(Dot({1,0}, {0,1}) == 0.0);      // 1
    ASSERT_TRUE(Dot({1,0}, {1,0}) == 1.0);      // 2
    ASSERT_TRUE(Dot({3,4}, {3,4}) == 25.0);     // 3
    ASSERT_TRUE(Dot({-1,0}, {1,0}) == -1.0);    // 4
    ASSERT_TRUE(Dot({2,3}, {4,5}) == 23.0);     // 5
    ASSERT_TRUE(Dot({0,0}, {5,7}) == 0.0);      // 6
    
    // 2. CROSS2D (4 tests)
    ASSERT_TRUE(Cross2D({1,0}, {0,1}) == 1.0);   // 7
    ASSERT_TRUE(Cross2D({0,1}, {1,0}) == -1.0);  // 8
    ASSERT_TRUE(Cross2D({1,1}, {1,1}) == 0.0);   // 9
    ASSERT_TRUE(Cross2D({2,0}, {0,2}) == 4.0);   // 10
    
    // 3. NORMALISATION (4 tests)
    Vec2d w = {3,4};
    Vec2d n = w.Normalized();
    ASSERT_TRUE(std::fabs(n.Norm() - 1.0) < kEps);   // 11
    
    // direction conservée
    ASSERT_TRUE(std::fabs(n.x - 0.6) < kEps);    // 12
    ASSERT_TRUE(std::fabs(n.y - 0.8) < kEps);    // 13
    
    // vecteur unitaire reste inchangé
    Vec2d u = {1,0};
    u = u.Normalized();
    ASSERT_TRUE(std::fabs(u.x - 1.0) < kEps);   // 14
    
    // 4. OPERATOR [] (5 tests)
    w = {10, 20};
    ASSERT_TRUE(w[0] == 10.0);   // 15
    ASSERT_TRUE(w[1] == 20.0);   // 16

    w[0] = 30;
    ASSERT_TRUE(w.x == 30.0);    // 17

    w[1] = 40;
    ASSERT_TRUE(w.y == 40.0);    // 18
    
    u = {5, 6};
    ASSERT_TRUE(u[0] == 5.0);    // 19
    
    // 5. STATIC ASSERT (1 test)
    static_assert(sizeof(Vec2d) == 16, "Vec2d must be 16 bytes"); // 20
}
