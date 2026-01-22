#include <iostream>
#include <cmath>

#include "black_scholes.h"
#include "implied_vol.h"

static bool approx_rel(double a, double b, double rel=1e-8) {
    double denom = std::max(1.0, std::max(std::fabs(a), std::fabs(b)));
    return std::fabs(a-b)/denom <= rel;
}

int main() {
    const double S=100, K=100, T=1.0, r=0.05, sigma=0.2;

    // Call
    const double call_mkt = callPrice(S,K,T,r,sigma);
    auto ivc = impliedVolCall(call_mkt, S,K,T,r, 0.3);
    if (!ivc.converged || !approx_rel(ivc.sigma, sigma, 1e-8)) {
        std::cerr << "FAIL: implied vol call\n";
        return 1;
    }

    // Put
    const double put_mkt = putPrice(S,K,T,r,sigma);
    auto ivp = impliedVolPut(put_mkt, S,K,T,r, 0.3);
    if (!ivp.converged || !approx_rel(ivp.sigma, sigma, 1e-8)) {
        std::cerr << "FAIL: implied vol put\n";
        return 1;
    }

    std::cout << "PASS: implied vol\n";
    return 0;
}
