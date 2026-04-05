#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Mat4d.h"
#include "Quat.h"
#include "NkImage.h"

using namespace nkentseu::math;

// --------------------------------  TP11 : Animation SLERP
TEST_CASE(Semaine4_TP2, AnimationSLERP) {
    const int width = 512, height = 512;
    NkImage img(width, height);

    std::vector<Vec4d> cube = {
        {-0.5,-0.5,-0.5,1}, {0.5,-0.5,-0.5,1},
        {0.5, 0.5,-0.5,1}, {-0.5, 0.5,-0.5,1},
        {-0.5,-0.5, 0.5,1}, {0.5,-0.5, 0.5,1},
        {0.5, 0.5, 0.5,1}, {-0.5, 0.5, 0.5,1}
    };
        
    // Arêtes du cube (12)
    std::vector<Vec2d> edges = {
        {0,1},{1,2},{2,3},{3,0}, // face arrière
        {4,5},{5,6},{6,7},{7,4}, // face avant
        {0,4},{1,5},{2,6},{3,7}  // connexions
    };

    // Matrices de Vue et Projection pour le rasteriseur logiciel
    Vec3d eye{0,1,3}, target{0,0,0}, up{0,1,0};
    Mat4d V = LookAt(eye, target, up);
    Mat4d P = Perspective(60.0, double(width)/height, 0.1, 100.0);

    Quat q1, q2, q3;
    
    // 1. Animez une rotation sur 60 frames via le rasteriseur
    q1 = FromAxisAngle({0,1,0}, 0);
    q2 = FromAxisAngle({0,1,0}, NKENTSEU_PI_DOUBLE);
    
    for(int frame = 0; frame < 60; frame++){
        double t = frame / 59.0;
        Quat q = Slerp(q1, q2, t);
        Mat4d R = FromRT(ToMat3(q), {0,0,0});
    
        img = NkImage(width, height);
        std::vector<Vec3d> screen;
        for(auto v : cube){
            Vec4d p = P * (V * (R * v));     // rotation + Vue + Projection
            screen.push_back(ProjectToScreen(p, width, height));
        }
    
        for(auto edge : edges)
            img.DrawLine((int)screen[edge.x].x, (int)screen[edge.x].y, (int)screen[edge.y].x, (int)screen[edge.y].y, 255);
        img.SavePPM("Slerp__frame_TP11_"+std::to_string(frame)+".ppm");
    }
    
    for(int frame = 0; frame < 60; frame++){
        double t = frame / 59.0;
        Quat q = Lerp(q1, q2, t);
        Mat4d R = FromRT(ToMat3(q), {0,0,0});
    
        img = NkImage(width, height);
        std::vector<Vec3d> screen;
        for(auto v : cube){
            Vec4d p = P * (V * (R * v));     // rotation + Vue + Projection
            screen.push_back(ProjectToScreen(p, width, height));
        }
    
        for(auto edge : edges)
            img.DrawLine((int)screen[edge.x].x, (int)screen[edge.x].y, (int)screen[edge.y].x, (int)screen[edge.y].y, 255);
        img.SavePPM("Lerp__frame_TP11_"+std::to_string(frame)+".ppm");
    }
}
