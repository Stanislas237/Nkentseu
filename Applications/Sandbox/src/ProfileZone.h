#pragma once

#include <chrono>
#include "NKLogger/NkLog.h"

struct ProfileZone { 
    const char* name;    
    std::chrono::steady_clock::time_point start; 

    ProfileZone(const char* n) : name(n), start(std::chrono::steady_clock::now()) {} 
    
    ~ProfileZone() { 
        auto us = std::chrono::duration_cast<std::chrono::microseconds>( 
        std::chrono::steady_clock::now() - start).count(); 
        logger.Info("{0}: {1}\n", name, us); 
    } 
}; 
