// black_scholes.cpp

#include "black_scholes.h"
#include "utils.h"

#include <cmath>
#include <algorithm>

namespace {
    inline double call_intrinsic(double S, double K) {
        return std::max(S - K, 0.0);
    }

    inline double put_intrinsic(double S, double K) {
        return std::max(K - S, 0.0);
    }

    inline double d1(double S, double K, double T, double r, double sigma) {
        const double vol_sqrtT = sigma * std::sqrt(T);
        return (std::log(S/K) + (r + 0.5 * sigma * sigma)*T) / vol_sqrtT;
    }

    inline double d2(double d1_val, double T, double sigma) {
        return (d1_val - sigma * std::sqrt(T));
    }
}


double callPrice(double S, double K, double T, double r, double sigma) {
    if (S <= 0.0 || K<=0.0 || T <= 0.0 || sigma < 0.0 ) {
        return NAN;
    }
    if (T == 0.0) { 
        return call_intrinsic(S,K);
    }
    if (sigma==0.0) {
        return std::max(S - K * std::exp(-r * T),0.0);
    }

    const double d1_val = bs_d1(S, K, T, r, sigma);
    const double d2_val = bs_d2_from_d1(d1_val,T,sigma);
    const double df = std::exp(-r * T);

    return S * normal_cdf(d1_val) - K * df * normal_cdf(d2_val);
}

double putPrice(double S, double K, double T, double r, double sigma) {
    if (S <= 0.0 || K <= 0.0 || T < 0.0 || sigma < 0.0) {
     return NAN;
    }

    if (T == 0.0) { 
    return put_intrinsic(S, K);
    }

    if (sigma == 0.0) {
        return std::max(K * std::exp(-r * T) - S, 0.0);
    }

    const double d1_val = bs_d1(S, K, T, r, sigma);
    const double d2_val = bs_d2_from_d1(d1_val, T, sigma);
    const double df = std::exp(-r * T);

    return K * df * normal_cdf(-d2_val) - S * normal_cdf(-d1_val);
}





