#pragma once 
#include "Mat3d.h" 
#include <cmath> 
 
namespace NkMath { 
 
struct Quat { 
    double w, x, y, z;  // w = partie réelle, (x,y,z) = parties imaginaires
 
    Quat() : w(1), x(0), y(0), z(0) {}
    Quat(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {} 
 
    double Norm2() const { return w*w + x*x + y*y + z*z; } 
    double Norm()  const { return std::sqrt(Norm2()); } 
 
    Quat Normalized() const { 
        double n = Norm(); 
        assert(!nearlyZero(n)); 
        return {w/n, x/n, y/n, z/n}; 
    } 
 
    // Conjugué : inverse la rotation 
    Quat Conjugate() const { return {w, -x, -y, -z}; } 
 
    // Inverse : pour quaternion unitaire, Inverse() == Conjugate() 
    Quat Inverse() const { 
        double n2 = Norm2(); 
        assert(!nearlyZero(n2)); 
        return {w/n2, -x/n2, -y/n2, -z/n2}; 
    } 
 
    // Produit de Hamilton (composition de rotations) 
    Quat operator*(const Quat& o) const { 
        return { 
            w*o.w - x*o.x - y*o.y - z*o.z, 
            w*o.x + x*o.w + y*o.z - z*o.y, 
            w*o.y - x*o.z + y*o.w + z*o.x, 
            w*o.z + x*o.y - y*o.x + z*o.w 
        }; 
    } 
}; 
 
// Quaternion depuis axe-angle (formule de Rodrigues) 
inline Quat FromAxisAngle(const Vec3d& axis, double angleRad) { 
    Vec3d n  = axis.Normalized(); 
    double s = std::sin(angleRad / 2.0); 
    double c = std::cos(angleRad / 2.0); 
    return {c, n.x*s, n.y*s, n.z*s}; 
} 
 
// Rotation d'un vecteur par un quaternion 
// v' = q ⊗ (0,v) ⊗ q*  (version optimisée sans construire 2 quaternions) 
inline Vec3d Rotate(const Quat& q, const Vec3d& v) { 
    Vec3d qVec  = {q.x, q.y, q.z}; 
    Vec3d uv    = Cross(qVec, v); 
    Vec3d uuv   = Cross(qVec, uv); 
    return v + (uv * (2.0 * q.w)) + (uuv * 2.0); 
} 
 
} // namespace NkMath
