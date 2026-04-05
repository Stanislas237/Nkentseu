#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Mat4d.h"
#include "NkImage.h"

using namespace nkentseu::math;

// --------------------------------  TP9 : TRS et Décomposition
TEST_CASE(Semaine3_TP3, TRSEtDecomposition) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-5.0, 5.0);

    for(int t = 0; t < 20; t++){
        Vec3d outT{dist(rng), dist(rng), dist(rng)};
        Vec3d outR{dist(rng), dist(rng), dist(rng)};
        Vec3d outS{dist(rng) + 6, dist(rng) + 6, dist(rng) + 6}; // éviter <= 0
        
        // 1. Construire TRS
        Mat4d M = TRS(outT, outR, outS);
        
        // 2. Décomposer TRS
        Vec3d T2, R2, S2;
        DecomposeTRS(M, T2, R2, S2);
    
        // 3. Vérifier les valeurs
        ASSERT_TRUE(ApproxVec(outT, T2));
        ASSERT_TRUE(ApproxVec(outS, S2));
        // rotation : tolérance plus large (ambiguïtés angles)
        ASSERT_TRUE(ApproxVec(outR, R2, 5.0));
    }
}
