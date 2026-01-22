#include "utils.h"
#include <cmath>
#include <cstdint>


// Standard normal probability distribution fucntion

double normal_pdf(double x) {
    static const double inv_sqrt_2pi = 0.39894228040143267794;
    return inv_sqrt_2pi * std::exp(-0.5 * x * x);
}

// Standard normal cumulative distribution function

double normal_cdf(double x) {
    return 0.5 * std::erfc(-x /std::sqrt(2.0));
}

double bs_d1(double S, double K, double T, double r, double sigma) {
    const double vol_sqrtT = sigma * std::sqrt(T);
    return (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / vol_sqrtT;
}

double bs_d2_from_d1(double d1, double T, double sigma) {
    return d1 - sigma * std::sqrt(T);
}

static inline std::uint64_t splitmix64(std::uint64_t& x) {
    std::uint64_t z = (x += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;

    return z ^ (z >> 31);
}


double rand_uniform_01(std::uint64_t& state) {
    // Convert 64-bit int to (0,1) with 53-bit precision
    const std::uint64_t u = splitmix64(state);
    const std::uint64_t mantissa = u >> 11; // top 53 bits
    const double x = (mantissa + 1.0) * (1.0 / 9007199254740992.0); // /2^53
    return x; 
}

double rand_standard_normal(std::uint64_t& state) {
    // Box–Muller transform
    const double u1 = rand_uniform_01(state);
    const double u2 = rand_uniform_01(state);

    const double R = std::sqrt(-2.0 * std::log(u1));
    const double theta = 2.0 * 3.14159265358979323846 * u2;

    return R * std::cos(theta); // N(0,1)
}

