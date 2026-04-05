#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Mat4d.h"

using namespace nkentseu::math;

// --------------------------------  TP7 : Mat4d et Inverse
TEST_CASE(Semaine3_TP1, Mat4dEtInverse) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-10.0, 10.0);
    Mat4d m, r, inv;

    for(int t=0;t<10;t++) {
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                m(i, j) = dist(rng);
        
        // 1. M × Identity() == M
        r = m * Mat4d::Identity();
        ASSERT_TRUE(ApproxMat(r, m)); // 1–10
    
        // 2. M × M⁻¹ == Identity()
        if(Inverse(m, inv)) // skip singulière
            ASSERT_TRUE(ApproxMat(m * inv, Mat4d::Identity(), 1e-10f)); // 11–20
    }
    
    // 3. Inverse d'une matrice singulière retourne false
    m = Mat4d::Identity();
    // rendre singulière (ligne dupliquée)
    for(int j=0;j<4;j++)
        m(1, j) = m(0, j);
    ASSERT_TRUE(!Inverse(m, inv)); // 21
    
    // 4. RotateAxis({0,1,0}, PI/2) × {1,0,0,1} == {0,0,-1,1} 
    r = Mat4d::RotateAxis({0,1,0}, NKENTSEU_PI_DOUBLE / 2.0f);
    Vec4d s = {1,0,0,1}, q = r * s;

    ASSERT_TRUE(approxEq(q.x, 0.0));   // 22
    ASSERT_TRUE(approxEq(q.y, 0.0));   // 23
    ASSERT_TRUE(approxEq(q.z, -1.0));  // 24
}
