#include "GameLoop.h"
#include <sstream>

namespace NkEngine {

    GameLoop::GameLoop(NkWindow& window) : m_window(window) {}

    void GameLoop::Run(const GameLoopCallbacks& callbacks) {
        m_running = true;

        TimePoint currentTime = Clock::now();
        double accumulator = 0.0;

        int frames = 0;
        int updates = 0;
        double statsTimer = 0.0;

        while (m_running && m_window.IsOpen()) {

            TimePoint newTime = Clock::now();
            double frameTime = std::chrono::duration<double>(newTime - currentTime).count();
            currentTime = newTime;

            // Anti spiral of death
            if (frameTime > MAX_FRAME_TIME)
                frameTime = MAX_FRAME_TIME;

            accumulator += frameTime;
            statsTimer += frameTime;

            // Input (une fois par frame)
            if (callbacks.onInput)
                callbacks.onInput();

            // Fixed update (60 Hz)
            while (accumulator >= FIXED_DT) {
                if (callbacks.onFixedUpdate)
                    callbacks.onFixedUpdate(FIXED_DT);

                accumulator -= FIXED_DT;
                updates++;
            }

            // Interpolation alpha
            double alpha = accumulator / FIXED_DT;

            // Render
            if (callbacks.onRender)
                callbacks.onRender(alpha);

            frames++;

            // Mise à jour stats toutes les 0.5 secondes
            if (statsTimer >= 0.5) {
                m_fps = frames / statsTimer;
                m_ups = updates / statsTimer;

                std::ostringstream title;
                title << "NkEngine | FPS: " << (int)m_fps
                    << " | UPS: " << (int)m_ups;

                m_window.SetTitle(NkString(title.str().c_str()));
                frames = 0;
                updates = 0;
                statsTimer = 0.0;
            }
        }
    }

    void GameLoop::Stop() {
        m_running = false;
    }

    double GameLoop::GetFPS() const {
        return m_fps;
    }

    double GameLoop::GetFixedUPS() const {
        return m_ups;
    }

} // namespace NkEngine
