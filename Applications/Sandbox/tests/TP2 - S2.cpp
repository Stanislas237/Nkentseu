#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Vec3d.h"

using namespace nkentseu::math;

// --------------------------------  TP5 : Vec3d avec Gram-Schmidt
TEST_CASE(Semaine2_TP2, Vec3dEtGramSchmidt) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);

    // 1 & 2. Cross Product
    Vec3d i = {1,0,0}, j = {0,1,0}, k = {0,0,1};

    // règle main droite
    ASSERT_TRUE(ApproxVec(Cross(i, j), k));               // 1
    ASSERT_TRUE(ApproxVec(Cross(j, i), {0,0,-1}));        // 2
    // base complète
    ASSERT_TRUE(ApproxVec(Cross(j, k), i));               // 3
    ASSERT_TRUE(ApproxVec(Cross(k, i), j));               // 4
    // orthogonalité
    ASSERT_TRUE(approxEq(Dot(Cross(i, j), i), 0));        // 5
    ASSERT_TRUE(approxEq(Dot(Cross(i, j), j), 0));        // 6
    
    // 2. Gram-Schmidt sur 10 triplets aléatoires     
    for(int t = 0; t < 10; ++t) {
        Vec3d a{dist(rng), dist(rng), dist(rng)};
        Vec3d b{dist(rng), dist(rng), dist(rng)};
        Vec3d c{dist(rng), dist(rng), dist(rng)};
    
        // Gram-Schmidt
        Vec3d ui = a.Normalized();
        Vec3d vi = (b - Project(b, ui)).Normalized();
        Vec3d wi = (c - Project(c, ui) - Project(c, vi)).Normalized();
        // normes
        ASSERT_TRUE(approxEq(ui.Norm(), 1.0));  // 7
        ASSERT_TRUE(approxEq(vi.Norm(), 1.0));  // 8
        ASSERT_TRUE(approxEq(wi.Norm(), 1.0));  // 9
    
        // orthogonalité
        ASSERT_TRUE(approxEq(Dot(ui, vi), 0.0));  // 10
        ASSERT_TRUE(approxEq(Dot(ui, wi), 0.0));  // 11
        ASSERT_TRUE(approxEq(Dot(vi, wi), 0.0));  // 12
    }
    
    // 3. Project et Reject
    i = {3,4,0}, j = {1,0,0};
    Vec3d proj = Project(i, j);
    Vec3d rej = Reject(i, j);
    ASSERT_TRUE(ApproxVec(proj + rej, i)); // 13
}
