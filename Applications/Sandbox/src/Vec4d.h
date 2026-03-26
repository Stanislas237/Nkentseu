#pragma once 
#include "Vec3d.h"

namespace NkMath{

    struct Vec4d { 
        double x, y, z, w; 
     
        Vec4d() : x(0), y(0), z(0), w(0) {} 
        Vec4d(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {} 
        Vec4d(const Vec3d& v, double w) : x(v.x), y(v.y), z(v.z), w(w) {} 
     
        // Conversion vers Vec3d — déhomogénéisation 
        Vec3d ToVec3() const { 
            assert(!nearlyZero(w) && "Cannot dehomogenize: w is zero (direction)"); 
            return {x/w, y/w, z/w}; 
        } 
     
        // Accès 
        double& operator[](int i) { assert(i<4); return (&x)[i]; } 
    }; 

    static_assert(sizeof(Vec4d) == 32, "Vec4d must be 32 bytes");

}