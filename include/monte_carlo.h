// monte_carlo.h

#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include <cstddef>
#include <cstdint>

enum class MCMode {
    Plain,
    Antithetic,
    ControlVariateBS,     // uses BS as control variate (European only)
    AntitheticControlBS  
};

struct MCResult {
    double price;     
    double stderr;    
    double ci_low;    
    double ci_high;   
};

MCResult mcCallPrice(double S, double K, double T, double r, double sigma,
                     std::size_t n_paths, std::uint64_t seed,
                     MCMode mode = MCMode::Plain);

MCResult mcPutPrice(double S, double K, double T, double r, double sigma,
                    std::size_t n_paths, std::uint64_t seed,
                    MCMode mode = MCMode::Plain);

#endif

