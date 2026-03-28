#pragma once 

#include "NKWindow/Core/NkWindow.h"
#include <functional> 
#include <chrono> 
#include <vector>
#include <fstream>

namespace NkEngine { 
    
    // Callbacks injectés par le jeu 
    struct GameLoopCallbacks { 
        std::function<void(double dt)>    onFixedUpdate;  // physique 60Hz 
        std::function<void(double alpha)> onRender;       // rendu interpolé 
        std::function<void()>             onInput;        // input poll 
    }; 
    
    using namespace nkentseu;
    
    class GameLoop { 
    public: 
        std::vector<double> dtSamples;

        explicit GameLoop(NkWindow& window); 
        
        // Lance la boucle — bloquant jusqu'à fermeture 
        void Run(const GameLoopCallbacks& callbacks); 
        
        // Arrête la boucle proprement 
        void Stop(); 
    
        // Statistiques 
        double GetFPS() const; 
        double GetFixedUPS() const;  // Updates Per Second de fixedUpdate 
    
    private: 
        using Clock     = std::chrono::steady_clock; 
        using TimePoint = std::chrono::time_point<Clock>; 
    
        NkWindow& m_window; 
        bool              m_running = false; 
        double            m_fps     = 0.0; 
        double            m_ups     = 0.0; 
        static constexpr double FIXED_DT      = 1.0 / 60.0; 
        static constexpr double MAX_FRAME_TIME = 0.25;  // anti-spiral 
    }; 
 
} // namespace NkEngine
