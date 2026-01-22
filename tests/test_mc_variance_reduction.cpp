#include <iostream>
#include <cmath>

#include "monte_carlo.h"

int main() {
    const double S=100, K=100, T=1.0, r=0.05, sigma=0.2;
    const std::size_t n = 200000;
    const std::uint64_t seed = 123456;

    const auto plain = mcCallPrice(S,K,T,r,sigma,n,seed,MCMode::Plain);
    const auto anti  = mcCallPrice(S,K,T,r,sigma,n,seed,MCMode::Antithetic);

    if (!(anti.stderr < plain.stderr)) {
        std::cerr << "FAIL: antithetic did not reduce stderr\n"
                  << "plain stderr=" << plain.stderr << " anti stderr=" << anti.stderr << "\n";
        return 1;
    }

   
    if (!(anti.stderr <= 0.95 * plain.stderr)) {
        std::cerr << "FAIL: antithetic reduction too small\n"
                  << "plain stderr=" << plain.stderr << " anti stderr=" << anti.stderr << "\n";
        return 1;
    }

    std::cout << "PASS: variance reduction reduces stderr\n";
    return 0;
}
