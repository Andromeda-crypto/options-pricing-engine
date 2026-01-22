// greeks.cpp

#include "greeks.h"
#include "utils.h"
#include <cmath>
#include <algorithm>

double callDelta(double S, double K, double T, double r, double sigma) {
    if (S <= 0.0 || K <= 0.0 || T < 0.0 || sigma < 0.0) {
        return NAN;
    }
    if (T == 0.0) {
        return (S > K) ? 1.0 : 0.0;
    }

    if (sigma == 0.0) {
        return (S > K * std::exp(-r * T)) ? 1.0 : 0.0;
    }
    const double d1 = bs_d1(S, K, T, r, sigma);
    return normal_cdf(d1);
}

double putDelta(double S, double K, double T, double r, double sigma) {
    if (S <= 0.0 || K <= 0.0 || T < 0.0 || sigma < 0.0) {
        return NAN;
    }
    if (T == 0.0) {
        return (S < K) ? -1.0 : 0.0;
    }
    if (sigma == 0.0) {
        return (S < K * std::exp(-r * T)) ? -1.0 : 0.0;
    }

    const double d1 = bs_d1(S, K, T, r, sigma);
    return normal_cdf(d1) - 1.0;
}


double gamma(double S, double K, double T, double r, double sigma) {
    if (S <= 0.0 || T <= 0.0 || sigma <= 0.0) {
        return 0.0;
    }
    const double d1 = bs_d1(S,K,T,r,sigma);
    return normal_pdf(d1) /(S * sigma * std::sqrt(T));
}

double vega(double S, double K, double T, double r, double sigma) {
    if (S <= 0.0 || K <= 0.0 || T <= 0.0 || sigma <= 0.0) return 0.0;

    const double d1 = bs_d1(S, K, T, r, sigma);
    return S * normal_pdf(d1) * std::sqrt(T);  
}

