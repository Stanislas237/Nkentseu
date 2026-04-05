#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "Vec2d.h"

using namespace nkentseu::math;

// --------------------------------  TP1 : Implémentez la fonction inspectFloat(float x)
TEST_CASE(Semaine1_TP1, FonctionInspectFloat) {
    inspectFloat(0.1f);
    inspectFloat(1.0f);
    inspectFloat(1.0f / 0.0f);
    inspectFloat(std::sqrt(-1.0f));
    inspectFloat(-0.0f);
    inspectFloat(0.0f);
    inspectFloat(std::numeric_limits<float>::min());
}
