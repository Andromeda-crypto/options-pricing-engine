// test cases for monte carlo calculations against known values 

#include <iostream>
#include <cmath>

#include "black_scholes.h"
#include "monte_carlo.h"

int main() {
    const double S=100, K=100, T=1.0, r=0.05, sigma=0.2;

    const double bs_call = callPrice(S,K,T,r,sigma);
    const auto mc = mcCallPrice(S,K,T,r,sigma, 200000, 123456);

    const double err = std::fabs(mc.price - bs_call);
    const double bound = 5.0 * mc.stderr + 0.01;

    if (err > bound) {
        std::cerr << "FAIL: MC call vs BS\n"
                  << "MC=" << mc.price << " stderr=" << mc.stderr
                  << " BS=" << bs_call << " |diff|=" << err
                  << " bound=" << bound << "\n";
        return 1;
    }

    auto plain = mcCallPrice(S,K,T,r,sigma,200000,123,MCMode::Plain);
    auto anti  = mcCallPrice(S,K,T,r,sigma,200000,123,MCMode::Antithetic);

    std::cout << "plain stderr = " << plain.stderr << "\n";
    std::cout << "anti  stderr = " << anti.stderr  << "\n";


    std::cout << "PASS: Monte Carlo vs Black-Scholes\n";
    return 0;
}


