#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Vec2d.h"

using namespace nkentseu::math;

// --------------------------------  TP2 : problèmes de précision
TEST_CASE(Semaine1_TP2, Precision) {
    float s1, s2;
    std::vector<float> v;

    // 1. Tableau de 1.000.000 et somme
    std::vector<float> data(1'000'000, 0.1f);
    
    // 2. Somme accumulate vs Somme Kahan
    s1 = std::accumulate(data.begin(), data.end(), 0.0f);
    s2 = kahanSum(data);
    logger.Info("\nSum with accumulate : {0}\nKahan sum : {1}\nReal value : 100000.0", s1, s2);
    
    // 3. Variance naïve VS Variance Welford
    v = std::vector<float>({1e8f, 1e8f, 1.0f, 2.0f});
    logger.Info("\nVariance Naive   : {0}\nVariance de Welford : {1}", varianceNaive(v), varianceWelford(v));
    
    // 4. Epsilon machine par boucle vs std::numeric_limits<float>::epsilon() 
    logger.Info("\nEpsilon Machine (loop) : {0}\nEpsilon Machine (std)  : {1}", epsilonMachine(), std::numeric_limits<float>::epsilon());
}
