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
