#include <iostream>
#include <cmath>

#include "black_scholes.h"
#include "monte_carlo.h"

static void assert_mc_close_to_bs_call(double S, double K, double T, double r, double sigma,
                                       std::size_t n, std::uint64_t seed, MCMode mode,
                                       double k=5.0, double eps=0.02) {
    const double bs = callPrice(S, K, T, r, sigma);
    const auto mc = mcCallPrice(S, K, T, r, sigma, n, seed, mode);

    const double diff = std::fabs(mc.price - bs);
    const double bound = k * mc.stderr + eps;

    if (!(diff <= bound)) {
        std::cerr << "FAIL: MC call vs BS\n"
                  << "mode=" << static_cast<int>(mode)
                  << " S=" << S << " K=" << K << " T=" << T << " r=" << r << " sigma=" << sigma << "\n"
                  << "MC=" << mc.price << " stderr=" << mc.stderr
                  << " CI=[" << mc.ci_low << "," << mc.ci_high << "]\n"
                  << "BS=" << bs << " |diff|=" << diff << " bound=" << bound << "\n";
        std::exit(1);
    }
}

static void assert_mc_close_to_bs_put(double S, double K, double T, double r, double sigma,
                                      std::size_t n, std::uint64_t seed, MCMode mode,
                                      double k=5.0, double eps=0.02) {
    const double bs = putPrice(S, K, T, r, sigma);
    const auto mc = mcPutPrice(S, K, T, r, sigma, n, seed, mode);

    const double diff = std::fabs(mc.price - bs);
    const double bound = k * mc.stderr + eps;

    if (!(diff <= bound)) {
        std::cerr << "FAIL: MC put vs BS\n"
                  << "mode=" << static_cast<int>(mode)
                  << " S=" << S << " K=" << K << " T=" << T << " r=" << r << " sigma=" << sigma << "\n"
                  << "MC=" << mc.price << " stderr=" << mc.stderr
                  << " CI=[" << mc.ci_low << "," << mc.ci_high << "]\n"
                  << "BS=" << bs << " |diff|=" << diff << " bound=" << bound << "\n";
        std::exit(1);
    }
}

int main() {
    const std::size_t n = 200000;
    const std::uint64_t seed = 123456;

    // Core cases (include at-the-money and off-the-money)
    assert_mc_close_to_bs_call(100, 100, 1.0, 0.05, 0.2, n, seed, MCMode::Plain);
    assert_mc_close_to_bs_put (100, 100, 1.0, 0.05, 0.2, n, seed, MCMode::Plain);

    assert_mc_close_to_bs_call(100, 110, 1.0, 0.05, 0.2, n, seed, MCMode::Antithetic);
    assert_mc_close_to_bs_put (100, 110, 1.0, 0.05, 0.2, n, seed, MCMode::Antithetic);

    // Slightly shorter maturity
    assert_mc_close_to_bs_call(100, 100, 0.25, 0.03, 0.25, n, seed, MCMode::Antithetic);

    std::cout << "PASS: MC regression vs BS (stat bound)\n";
    return 0;
}
