// ============================================================================
// Sandbox/src/main.cpp
// Pattern A : Dispatcher typé (push - événementiel)
// ============================================================================

#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkSystem.h"
#include "NKWindow/Events/NkEventDispatcher.h"
#include "NKWindow/Events/NkEventSystem.h"
#include "NKWindow/Events/NkGamepadSystem.h"
#include "NKWindow/Core/NkMain.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/NkRendererConfig.h"
#include "NKTime/NkChrono.h"

#include "NKLogger/NkLog.h"
#include "NKMath/NKMath.h"

#include "NKMemory/NkMemory.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numeric>
#include <iostream>
#include <assert.h>
#include <cstdlib>
#include "Vec4d.h" 

#ifndef NK_SANDBOX_RENDERER_API
#define NK_SANDBOX_RENDERER_API nkentseu::NkRendererApi::NK_SOFTWARE
#endif

namespace {
using namespace nkentseu;
using namespace nkentseu::math;

float ClampUnit(float v) {
    if (v < 0.f) return 0.f;
    if (v > 1.f) return 1.f;
    return v;
}

NkU32 PackWaveColor(float r, float g, float b) {
    return NkRenderer::PackColor(
        static_cast<NkU8>(ClampUnit(r) * 255.f),
        static_cast<NkU8>(ClampUnit(g) * 255.f),
        static_cast<NkU8>(ClampUnit(b) * 255.f), 255);
}

void DrawPlasma(NkRenderer& renderer, NkU32 width, NkU32 height,
                float t, const NkVec2f& phase, float sat)
{
    if (!width || !height) return;
    const NkU32 blk = (width * height > 900u * 600u) ? 2u : 1u;
    const float iw = 1.f / width, ih = 1.f / height;
    for (NkU32 y = 0; y < height; y += blk) {
        float fy = y * ih - 0.5f;
        for (NkU32 x = 0; x < width; x += blk) {
            float fx  = x * iw - 0.5f;
            float rd  =  NkSqrt(fx*fx + fy*fy);
            float mix = (NkSin((fx + phase.x)*13.5f + t*1.7f)
                        +NkSin((fy + phase.y)*11.0f - t*1.3f)
                        +NkSin(rd*24.f - t*2.1f)) * 0.33333334f;
            float r = ClampUnit((0.5f+0.5f*NkSin(6.2831853f*(mix+0.00f))-0.5f)*sat+0.5f);
            float g = ClampUnit((0.5f+0.5f*NkSin(6.2831853f*(mix+0.33f))-0.5f)*sat+0.5f);
            float b = ClampUnit((0.5f+0.5f*NkSin(6.2831853f*(mix+0.66f))-0.5f)*sat+0.5f);
            NkU32 col = PackWaveColor(r, g, b);
            for (NkU32 by=0;by<blk&&(y+by)<height;++by)
                for (NkU32 bx=0;bx<blk&&(x+bx)<width;++bx)
                    renderer.SetPixel((NkI32)(x+bx),(NkI32)(y+by),col);
        }
    }
}

// =========================================================================
// Couche applicative — Pattern A : Dispatcher typé
// =========================================================================

class GameLayer {
public:
    void OnEvent(nkentseu::NkEvent* ev) {
        nkentseu::NkEventDispatcher d(ev);

        // Clavier
        NK_DISPATCH(d, nkentseu::NkKeyPressEvent, OnKeyPress);

        // Manette
        NK_DISPATCH(d, nkentseu::NkGamepadAxisEvent, OnGamepadAxis);
        NK_DISPATCH(d, nkentseu::NkGamepadButtonPressEvent, OnGamepadPress);

        // Fenêtre
        NK_DISPATCH(d, nkentseu::NkWindowCloseEvent, OnWindowClose);
        NK_DISPATCH(d, nkentseu::NkWindowResizeEvent, OnWindowResize);
    }

    bool OnKeyPress(nkentseu::NkKeyPressEvent& e) {
        switch (e.GetKey()) {
            case nkentseu::NkKey::NK_ESCAPE:
                mShouldClose = true;
                return true;
            case nkentseu::NkKey::NK_F11:
                mFullscreen = !mFullscreen;
                return true;
            case nkentseu::NkKey::NK_SPACE:
                mNeonMode = !mNeonMode;
                return true;
            default:
                return false;
        }
    }

    bool OnGamepadAxis(nkentseu::NkGamepadAxisEvent& e) {
        float v = e.GetValue();
        switch (e.GetAxis()) {
            case nkentseu::NkGamepadAxis::NK_GP_AXIS_LX:
                mPhaseOffset.x += v * 0.02f;
                return true;
            case nkentseu::NkGamepadAxis::NK_GP_AXIS_LY:
                mPhaseOffset.y += v * 0.02f;
                return true;
            case nkentseu::NkGamepadAxis::NK_GP_AXIS_RT:
                mSaturationBoost = 1.f + ClampUnit(v) * 0.8f;
                return true;
            default:
                return false;
        }
    }

    bool OnGamepadPress(nkentseu::NkGamepadButtonPressEvent& e) {
        if (e.GetButton() == nkentseu::NkGamepadButton::NK_GP_SOUTH) {
            mNeonMode = !mNeonMode;
            nkentseu::NkGamepads().Rumble(e.GetGamepadIndex(), 0.2f, 0.5f, 0.f, 0.f, 80);
            return true;
        }
        return false;
    }

    bool OnWindowClose(nkentseu::NkWindowCloseEvent& e) {
        (void)e;
        mShouldClose = true;
        return true;
    }

    bool OnWindowResize(nkentseu::NkWindowResizeEvent& e) {
        mViewportW = e.GetWidth();
        mViewportH = e.GetHeight();
        return false;
    }

    NkVec2f GetPhaseOffset() const { return mPhaseOffset; }
    float GetSaturation() const { return mSaturationBoost; }
    bool IsNeonMode() const { return mNeonMode; }
    bool ShouldClose() const { return mShouldClose; }
    bool GetFullscreen() const { return mFullscreen; }
    void SetFullscreen(bool v) { mFullscreen = v; }

private:
    bool mShouldClose = false;
    bool mFullscreen = false;
    bool mNeonMode = false;
    nkentseu::NkVec2f mPhaseOffset = {0.f, 0.f};
    float mSaturationBoost = 1.15f;
    NkU32 mViewportW = 900, mViewportH = 600;
};

} // namespace

// ============================================================================
int nkmain(const nkentseu::NkEntryState& /*state*/)
{
    using namespace nkentseu;
    using namespace NkMath;

    // -------------------------------------------------------------------------
    // 1. Initialisation
    // -------------------------------------------------------------------------
    if (!NkInitialise({ .appName = "NkWindow Sandbox Pattern A" })) {
        logger.Error("[Sandbox] NkInitialise FAILED");
        return -1;
    }

    // -------------------------------------------------------------------------
    // 2. Fenêtre
    // -------------------------------------------------------------------------
    NkWindowConfig cfg;
    cfg.title       = "NkWindow Sandbox - Pattern A (Dispatcher)";
    cfg.width       = 900;
    cfg.height      = 600;
    cfg.centered    = true;
    cfg.resizable   = true;
    cfg.dropEnabled = true;

    NkWindow window(cfg);
    if (!window.IsOpen()) {
        logger.Error("[Sandbox] Window creation FAILED");
        NkClose();
        return -2;
    }

    // -------------------------------------------------------------------------
    // 3. Renderer
    // -------------------------------------------------------------------------
    NkRendererConfig rcfg;
    rcfg.api                   = NK_SANDBOX_RENDERER_API;
    rcfg.autoResizeFramebuffer = true;

    mem::NkUniquePtr<NkRenderer> renderer;
    if (rcfg.api != NkRendererApi::NK_NONE) {
        renderer = mem::NkMakeUnique<NkRenderer>();
        if (!renderer->Create(window, rcfg)) {
            logger.Error("[Sandbox] Renderer creation FAILED");
            NkClose();
            return -3;
        }
    }

    // -------------------------------------------------------------------------
    // 4. GameLayer - Pattern A (Dispatcher)
    // -------------------------------------------------------------------------
    GameLayer layer;

    // -------------------------------------------------------------------------
    // 5. Boucle principale
    // -------------------------------------------------------------------------
    auto& eventSystem = NkEvents();

    bool running = true;
    float timeSeconds = 0.f;
    NkChrono chrono;
    NkElapsedTime elapsed;

    float s1, s2;
    std::vector<float> v;
    Vec2d u, w, n;
    Vec3d u, w, n;

    // TP1 : Implémentez la fonction inspectFloat(float x)
    inspectFloat(0.1f);
    inspectFloat(1.0f);
    inspectFloat(1.0f / 0.0f);
    inspectFloat(std::sqrt(-1.0f));
    inspectFloat(-0.0f);
    inspectFloat(0.0f);
    inspectFloat(std::numeric_limits<float>::min());

    // TP2 : problèmes de précision
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
    
    // TP3 : 33 tests unitaires sur Float.h    
    // 1. isFiniteValid (5 tests)    
    assert(!isFiniteValid(std::numeric_limits<float>::quiet_NaN())); // 1
    assert(!isFiniteValid(std::numeric_limits<float>::infinity()));  // 2
    assert(!isFiniteValid(-std::numeric_limits<float>::infinity())); // 3
    assert(isFiniteValid(0.0f));                                     // 4
    assert(isFiniteValid(1.0f));                                     // 5

    // 2. nearlyZero (8 tests)
    assert(nearlyZero(0.0f, 1e-6f));     // 6
    assert(nearlyZero(1e-7f, 1e-6f));    // 7
    assert(!nearlyZero(1e-5f, 1e-6f));   // 8

    assert(nearlyZero(-1e-7f, 1e-6f));   // 9
    assert(!nearlyZero(-1e-5f, 1e-6f));  // 10

    assert(nearlyZero(1e-3f, 1e-2f));    // 11
    assert(!nearlyZero(1e-2f, 1e-3f));   // 12

    assert(nearlyZero(5e-8f, 1e-7f));    // 13

    // 3. approxEq (10 tests)
    assert(approxEq(1.0f, 1.0f, 1e-6f));           // 14
    assert(approxEq(1.0f, 1.0000001f, 1e-5f));     // 15
    assert(!approxEq(1.0f, 1.1f, 1e-3f));          // 16

    assert(approxEq(0.0f, 1e-7f, 1e-6f));          // 17
    assert(!approxEq(0.0f, 1e-4f, 1e-6f));         // 18

    assert(approxEq(-1.0f, -1.000001f, 1e-5f));    // 19
    assert(!approxEq(-1.0f, -1.1f, 1e-2f));        // 20

    assert(approxEq(1000.0f, 1000.0001f, 1e-3f));  // 21
    assert(approxEq(1000.0f, 1001.0f, 1e-3f));     // 22

    assert(approxEq(1e-7f, 2e-7f, 1e-6f));         // 23

    // 4. kahanSum vs accumulate (10 tests)    
    v = std::vector<float>(1000, 0.1f);
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    assert(std::fabs(s2 - 100.0f) < std::fabs(s1 - 100.0f)); // 24
    
    v = std::vector<float>(10000, 0.1f);
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    assert(std::fabs(s2 - 1000.0f) < std::fabs(s1 - 1000.0f)); // 25
    
    v = std::vector<float>({1e8f, 1.0f, -1e8f});
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    assert(std::fabs(s2 - 1.0f) <= std::fabs(s1 - 1.0f)); // 26

    v = std::vector<float>({1.0f, 1e8f, -1e8f});
    s1 = std::accumulate(v.begin(), v.end(), 0.0f);
    s2 = kahanSum(v);
    assert(std::fabs(s2 - 1.0f) <= std::fabs(s1 - 1.0f)); // 27
    
    v = std::vector<float>(100000, 0.01f);
    s2 = kahanSum(v);
    assert(approxEq(s2, 1000.0f, 1e-2f)); // 28

    v = std::vector<float>(100000, 1e-5f);
    s2 = kahanSum(v);
    assert(approxEq(s2, 1.0f, 1e-3f)); // 29
    
    v = std::vector<float>({0.1f, 0.2f, 0.3f});
    s2 = kahanSum(v);
    assert(approxEq(s2, 0.6f, 1e-6f)); // 30

    v = std::vector<float>(50000, 0.2f);
    s2 = kahanSum(v);
    assert(approxEq(s2, 10000.0f, 1e-2f)); // 31
    
    v = std::vector<float>({1e7f, 1.0f, 1.0f, -1e7f});
    s2 = kahanSum(v);
    assert(approxEq(s2, 2.0f, 1e-3f)); // 32

    v = std::vector<float>(1000000, 0.1f);
    s2 = kahanSum(v);
    assert(approxEq(s2, 100000.0f, 1e-1f)); // 33


    // TP4 : Vec2d complet + 20 implémentations
    // 1. Dot product (6 tests)
    assert(Dot({1,0}, {0,1}) == 0.0);      // 1
    assert(Dot({1,0}, {1,0}) == 1.0);      // 2
    assert(Dot({3,4}, {3,4}) == 25.0);     // 3
    assert(Dot({-1,0}, {1,0}) == -1.0);    // 4
    assert(Dot({2,3}, {4,5}) == 23.0);     // 5
    assert(Dot({0,0}, {5,7}) == 0.0);      // 6

    // 2. CROSS2D (4 tests)
    assert(Cross2D({1,0}, {0,1}) == 1.0);   // 7
    assert(Cross2D({0,1}, {1,0}) == -1.0);  // 8
    assert(Cross2D({1,1}, {1,1}) == 0.0);   // 9
    assert(Cross2D({2,0}, {0,2}) == 4.0);   // 10

    // 3. NORMALISATION (4 tests)
    w = {3,4};
    n = w.Normalized();
    assert(std::fabs(n.Norm() - 1.0) < kEps);   // 11

    // direction conservée
    assert(std::fabs(n.x - 0.6) < kEps);    // 12
    assert(std::fabs(n.y - 0.8) < kEps);    // 13

    // vecteur unitaire reste inchangé
    u = {1,0};
    u = u.Normalized();
    assert(std::fabs(u.x - 1.0) < kEps);   // 14

    // 4. OPERATOR [] (5 tests)
    w = {10, 20};
    assert(w[0] == 10.0);   // 15
    assert(w[1] == 20.0);   // 16
    w[0] = 30;
    assert(w.x == 30.0);    // 17
    w[1] = 40;
    assert(w.y == 40.0);    // 18
    u = {5, 6};
    assert(u[0] == 5.0);    // 19

    // 5. STATIC ASSERT (1 test)
    static_assert(sizeof(Vec2d) == 16, "Vec2d must be 16 bytes"); // 20


    // TP5 : Vec3d avec Gram-Schmidt
    // 1 & 2. Cross Product
    Vec3d i{1,0,0}, j{0,1,0}, k{0,0,1};

    // règle main droite
    assert(approxVec(i.cross(j), k));     // 1
    assert(approxVec(j.cross(i), {0,0,-1})); // 2

    // base complète
    assert(approxVec(j.cross(k), i));     // 3
    assert(approxVec(k.cross(i), j));     // 4

    // orthogonalité
    Vec3d c = i.cross(j);
    assert(approxEq(c.dot(i), 0));        // 5
    assert(approxEq(c.dot(j), 0));        // 6
}



    while (running && window.IsOpen())
    {
        NkElapsedTime e = chrono.Reset();

        // --- Pattern A : Dispatcher typé (OnEvent pour chaque event)
        while (NkEvent* event = eventSystem.PollEvent())
        {
            layer.OnEvent(event);
            
            if (layer.ShouldClose() || !window.IsOpen()) {
                running = false;
                break;
            }
        }

        if (!running || !window.IsOpen())
            break;

        // Appliquer fullscreen si changé
        if (layer.GetFullscreen() != window.GetConfig().fullscreen) {
            window.SetFullscreen(layer.GetFullscreen());
        }

        // --- Delta-time ---
        float dt = (float)elapsed.seconds;
        if (dt <= 0.f || dt > 0.25f) dt = 1.f / 60.f;
        timeSeconds += dt * (layer.IsNeonMode() ? 1.8f : 1.0f);

        // --- Rendu ---
        if (renderer) {
            renderer->BeginFrame(NkRenderer::PackColor(8, 10, 18, 255));
            const NkFramebufferInfo& fb = renderer->GetFramebufferInfo();
            NkU32 w = fb.width  ? fb.width  : window.GetSize().x;
            NkU32 h = fb.height ? fb.height : window.GetSize().y;
            DrawPlasma(*renderer, w, h, timeSeconds, layer.GetPhaseOffset(), layer.GetSaturation());
            renderer->EndFrame();
            renderer->Present();
        }        

        // --- Cap 60 fps ---
        elapsed = chrono.Elapsed();
        if (elapsed.milliseconds < 16)
            NkChrono::Sleep(16 - elapsed.milliseconds);
        else
            NkChrono::YieldThread();
    }

    // -------------------------------------------------------------------------
    // 6. Nettoyage
    // -------------------------------------------------------------------------
    if (renderer)
        renderer->Shutdown();

    window.Close();
    NkClose();
    return 0;
}
