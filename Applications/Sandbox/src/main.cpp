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
#include <iostream>
#include <cstdlib>

#include "GameLoop.h"
#include "ProfileZone.h"

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
                float t, const NkVec2f& phase, float sat, NkU32 xStart = 0, NkU32 yStart = 0)
{
    if (!width || !height) return;
    const NkU32 blk = (width * height > 900u * 600u) ? 2u : 1u;
    const float iw = 1.f / width, ih = 1.f / height;
    for (NkU32 y = yStart; y < NkMin(yStart + NkU32(50), height); y += blk) {
        float fy = y * ih - 0.5f;
        for (NkU32 x = xStart; x < NkMin(xStart + NkU32(50), width); x += blk) {
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

} // namespace

// ============================================================================
int nkmain(const nkentseu::NkEntryState& /*state*/)
{
    using namespace NkEngine;

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
    // 5. Boucle principale
    // -------------------------------------------------------------------------
    auto& eventSystem = NkEvents();
    GameLoopCallbacks callbacks;
    NkVec2i inputDir = {0, 0};
    NkVec2i squarePos = {0, 0};
    float prevAngle, currAngle;
    float radius = 100.0f;
    float timeSeconds = 0.f;
    NkChrono chrono;

    bool running = true;

    callbacks.onInput = [&]() {
        ProfileZone z("Input");
        while (NkEvent* event = eventSystem.PollEvent())
        {   
            nkentseu::NkEventDispatcher d(event);
            
            d.Dispatch<nkentseu::NkKeyPressEvent>([&](nkentseu::NkKeyPressEvent& e) {

                switch (e.GetKey()) {
                    case nkentseu::NkKey::NK_UP:
                        inputDir.y = -1;
                        return true;
                    case nkentseu::NkKey::NK_DOWN:
                        inputDir.y = 1;
                        return true;
                    case nkentseu::NkKey::NK_LEFT:
                        inputDir.x = -1;
                        return true;
                    case nkentseu::NkKey::NK_RIGHT:
                        inputDir.x = 1;
                        return true;
                    default:
                        return false;
                }
            });
            
            d.Dispatch<nkentseu::NkKeyReleaseEvent>([&](nkentseu::NkKeyReleaseEvent& e) {

                switch (e.GetKey()) {
                    case nkentseu::NkKey::NK_UP:
                    case nkentseu::NkKey::NK_DOWN:
                        inputDir.y = 0;
                        return true;
                    case nkentseu::NkKey::NK_LEFT:
                    case nkentseu::NkKey::NK_RIGHT:
                        inputDir.x = 0;
                        return true;
                    default:
                        return false;
                }
            });    
                        
            d.Dispatch<nkentseu::NkWindowCloseEvent>([&](nkentseu::NkWindowCloseEvent& e) {
                (void)e;
                running = false;
                window.IsOpen() ? window.Close() : void();
                return true;
            });        
        }
    };

    callbacks.onRender = [&](double dt) {
        ProfileZone z("Render");
        
        // --- Logique de jeu ici ---
        float interpAngle = prevAngle + (currAngle - prevAngle) * dt;
        squarePos.x = 300 + cos(interpAngle) * radius;
        squarePos.y = 200 + sin(interpAngle) * radius;

        if (renderer) {
            renderer->BeginFrame(NkRenderer::PackColor(8, 10, 18, 255));
            const NkFramebufferInfo& fb = renderer->GetFramebufferInfo();
            NkU32 w = fb.width  ? fb.width  : window.GetSize().x;
            NkU32 h = fb.height ? fb.height : window.GetSize().y;
            DrawPlasma(*renderer, w, h, timeSeconds, NkVec2f(0.0f, 0.0f), 1.15f, squarePos.x, squarePos.y);
            renderer->EndFrame();
            renderer->Present();
        }
    };

    callbacks.onFixedUpdate = [&](double dt) {
        ProfileZone z("FixedUpdate");
        timeSeconds += static_cast<float>(dt);
        // squarePos += inputDir;
        prevAngle = currAngle;
        currAngle = 2.0f * timeSeconds * NK_PI_F; // 1 tour/sec
        
        static int counter = 0;
        counter++;
        if (counter % 30 == 0)
            NkChrono::Sleep(NkDuration::FromMilliseconds(25.0f));
    };

    GameLoop gameloop(window);
    gameloop.Run(callbacks);
    gameloop.Stop();

    // -------------------------------------------------------------------------
    // 6. Nettoyage
    // -------------------------------------------------------------------------
    if (renderer)
        renderer->Shutdown();

    window.Close();
    NkClose();
    return 0;
}
