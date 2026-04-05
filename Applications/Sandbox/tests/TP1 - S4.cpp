#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Mat4d.h"
#include "Quat.h"

using namespace nkentseu::math;

// --------------------------------  TP10 : Quaternions complets 
TEST_CASE(Semaine4_TP1, Quaternions) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    Mat3d m1, m2, m3;
    Quat q1, q2, q3;

    // 1. Vérifier Rotate et FromAxis (avec Pi)
    Vec3d i = {1,0,0};
    q1 = FromAxisAngle({0,1,0}, NKENTSEU_PI_DOUBLE / 2.0f);
    Vec3d j = Rotate(q1, i);

    ASSERT_TRUE(std::fabs(j.x - 0.0) < kEps);
    ASSERT_TRUE(std::fabs(j.y - 0.0) < kEps);
    ASSERT_TRUE(std::fabs(j.z + 1.0) < kEps);
    
    // 2. Aller-retour Quat => Mat3d => Quat
    for(int t = 0; t < 50; t++){
        q1 = { dist(rng), dist(rng), dist(rng), dist(rng) };
        q1 = q1.Normalized();
    
        m1 = ToMat3(q1);
        q2 = FromMat3(m1);
        q2 = q2.Normalized();
    
        ASSERT_TRUE(ApproxQuat(q1, q2, 1e-4f));
    }
    
    // 3.  Vérifiez que Quat x Quat.Inverse() = identité
    for(int t = 0; t < 50; t++){
        q1 = { dist(rng), dist(rng), dist(rng), dist(rng) };
        q1 = q1.Normalized();
        q2 = q1.Inverse();
        q3 = q1 * q2;
        ASSERT_TRUE(ApproxQuat(q3, Quat::Identity(), 1e-4f));
    }
}
