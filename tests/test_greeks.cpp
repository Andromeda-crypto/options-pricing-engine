// testing greeks against knowm analytical values 

#include <iostream>
#include <cmath>

#include "black_scholes.h"
#include "greeks.h"
#include "utils.h"

static bool approx_abs(double a, double b, double tol) {
    return std::fabs(a-b) <= tol;
}

static bool approx_rel(double a, double b, double rel_tol) {
    const double denom = std::max(1.0, std::max(std::fabs(a), std::fabs(b)));
    return std::fabs(a - b) / denom <= rel_tol;
}

int main() {
    const double S = 100.0;
    const double K = 100.0;
    const double T = 1.0;
    const double r = 0.05;
    const double sigma = 0.2;

    // step sizes 
    const double hS = 1e-4 * S;
    const double hV = 1e-4;

    // Analytical greeks 
    const double delta_c = callDelta(S,K,T,r,sigma);
    const double delta_p = putDelta(S,K,T,r,sigma);
    const double gam = gamma(S,K,T,r,sigma);
    const double veg = vega(S,K,T,r,sigma);

    // finite-differecnes Delta (call/put)
    const double c_up   = callPrice(S + hS, K, T, r, sigma);
    const double c_down = callPrice(S - hS, K, T, r, sigma);
    const double fd_delta_c = (c_up - c_down) / (2.0 * hS);

    const double p_up   = putPrice(S + hS, K, T, r, sigma);
    const double p_down = putPrice(S - hS, K, T, r, sigma);
    const double fd_delta_p = (p_up - p_down) / (2.0 * hS);

     // --- Finite-difference Gamma (use call; should match put too) ---
    const double c_mid = callPrice(S, K, T, r, sigma);
    const double fd_gamma = (c_up - 2.0 * c_mid + c_down) / (hS * hS);

    // --- Finite-difference Vega (use call; should match put too) ---
    const double c_vol_up   = callPrice(S, K, T, r, sigma + hV);
    const double c_vol_down = callPrice(S, K, T, r, sigma - hV);
    const double fd_vega = (c_vol_up - c_vol_down) / (2.0 * hV);
    
     if (!approx_rel(delta_c, fd_delta_c, 1e-4)) {
        std::cerr << "FAIL: callDelta FD mismatch\n"
                  << "analytical=" << delta_c << " fd=" << fd_delta_c << "\n";
        return 1;
    }

    if (!approx_rel(delta_p, fd_delta_p, 1e-4)) {
        std::cerr << "FAIL: putDelta FD mismatch\n"
                  << "analytical=" << delta_p << " fd=" << fd_delta_p << "\n";
        return 1;
    }

    if (!approx_rel(gam, fd_gamma, 1e-5)) {
        std::cerr << "FAIL: gamma FD mismatch\n"
                  << "analytical=" << gam << " fd=" << fd_gamma << "\n";
        return 1;
    }

    if (!approx_rel(veg, fd_vega, 1e-5)) {
        std::cerr << "FAIL: vega FD mismatch\n"
                  << "analytical=" << veg << " fd=" << fd_vega << "\n";
        return 1;
    }

    if (!approx_abs(delta_p, delta_c - 1.0, 1e-12)) {
        std::cerr << "FAIL : put-call delta identity\n";
    }
    
    std::cout <<"PASS : Greeks finite-difference checks\n";
    return 0;
}

