#include "utils.h"
#include <cmath>


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