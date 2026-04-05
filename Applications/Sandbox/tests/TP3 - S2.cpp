#include <Unitest/Unitest.h>
#include <Unitest/TestMacro.h>

#include "NKMath/NKMath.h"
#include "NKLogger/NkLog.h"

#include "Vec4d.h"
#include "NkImage.h"

using namespace nkentseu::math;

// --------------------------------  TP6: Vec4d et projection perspective simple
TEST_CASE(Semaine2_TP3, Vec4dEtProjectionEtPerpectiveSimple) {
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

    // Projections dans l'espace 2D
    std::vector<Vec2d> proj;
    
    const int width = 512, height = 512;
    NkImage img(width, height);

    
    // Position de la camera et projections
    double z_cam = 2.0;
    for(auto& p : cube){
        p.z += z_cam;
        proj.push_back(ProjectPoint(p));
    }
    
    // Dessin des coins dans Image
    for(const auto& p : proj) {
        int x = (int)p.x, y = (int)p.y;
        // petit carré pour visibilité
        for(int dx = -2; dx <= 2; dx++)
            for(int dy = -2; dy <= 2; dy++)
                img.SetPixel(x+dx, y+dy, 255, 0, 0);
    }
    
    // Dessin dans l'image
    for(auto edge : edges)
        img.DrawLine((int)proj[edge.x].x, (int)proj[edge.x].y, (int)proj[edge.y].x, (int)proj[edge.y].y);
    img.SavePPM("cube.ppm");
}
