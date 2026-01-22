#include <iostream>
#include <cmath>
#include <algorithm>

#include "monte_carlo.h"

static bool approx(double a, double b, double tol=1e-12) {
    return std::fabs(a-b) <= tol;
}

int main() {
    const std::uint64_t seed = 7;

    // T=0: exact intrinsic
    {
        auto mc = mcCallPrice(100, 90, 0.0, 0.05, 0.2, 1000, seed, MCMode::Plain);
        const double intrinsic = std::max(100.0 - 90.0, 0.0);
        if (!approx(mc.price, intrinsic) || !approx(mc.stderr, 0.0)) {
            std::cerr << "FAIL: T=0 call\n";
            return 1;
        }
    }

    // sigma=0: deterministic under risk-neutral
    // ST = S * exp(rT), discounted payoff is max(S - K*exp(-rT), 0)
    {
        const double S=100, K=100, T=1.0, r=0.05, sigma=0.0;
        auto mc = mcCallPrice(S,K,T,r,sigma, 10000, seed, MCMode::Plain);
        const double det = std::max(S - K * std::exp(-r*T), 0.0);
        if (std::fabs(mc.price - det) > 1e-10) {
            std::cerr << "FAIL: sigma=0 call price\n";
            return 1;
        }
        if (mc.stderr > 1e-12) {
            std::cerr << "FAIL: sigma=0 stderr should be ~0\n";
            return 1;
        }
    }

    {
        auto mc = mcPutPrice(100, 110, 0.001, 0.02, 0.3, 50000, seed, MCMode::Antithetic);
        if (!std::isfinite(mc.price) || !std::isfinite(mc.stderr)) {
            std::cerr << "FAIL: small T produced non-finite\n";
            return 1;
        }
    }

    std::cout << "PASS: MC edge cases\n";
    return 0;
}
