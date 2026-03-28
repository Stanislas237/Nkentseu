#include "GameLoop.h"
#include "NKLogger/NkLog.h"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace NkEngine {

    GameLoop::GameLoop(NkWindow& window) : m_window(window) {
        dtSamples.reserve(1000);
    }

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

            // TP3 : Logger l'accumulator
            logger.Info("Accumulator: {0} seconds", accumulator);

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

            // Stockage des dt pour debug (optionnel)
            if (dtSamples.size() < 1000)
                dtSamples.push_back(frameTime);
        }
    }

    void GameLoop::Stop() {
        m_running = false;

        // Export des dt pour analyse (optionnel)
        std::ofstream file("dt.csv");
        file << "dt\n";
        for (double dt : dtSamples)
            file << dt << "\n";
        file.close();

        auto [min, max] = std::minmax_element(dtSamples.begin(), dtSamples.end());
        double mean = std::accumulate(dtSamples.begin(), dtSamples.end(), 0.0) / dtSamples.size();
        double sq_sum = std::accumulate(dtSamples.begin(), dtSamples.end(), 0.0,
            [mean](double acc, double x) {
                return acc + (x - mean) * (x - mean);
            });
        double stddev = std::sqrt(sq_sum / dtSamples.size());

        logger.Info("Game loop stopped. Analytics :\n- Average dt: {0:.2f},\n- Min dt: {1:.2f},\n- Max dt: {2:.2f},\n- Ecart-Type: {3:.2f}", mean, *min, *max, stddev);
    }

    double GameLoop::GetFPS() const {
        return m_fps;
    }

    double GameLoop::GetFixedUPS() const {
        return m_ups;
    }

} // namespace NkEngine
