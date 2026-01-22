#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>

#include "black_scholes.h"
#include "monte_carlo.h"

static double ms_since(const std::chrono::steady_clock::time_point& t0,
                       const std::chrono::steady_clock::time_point& t1) {
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

static void bench_case(double S, double K, double T, double r, double sigma,
                       std::uint64_t seed,
                       const std::vector<std::size_t>& paths_list) {
    // BS baseline
    const auto t0 = std::chrono::steady_clock::now();
    const double bs = callPrice(S, K, T, r, sigma);
    const auto t1 = std::chrono::steady_clock::now();
    const double bs_ms = ms_since(t0, t1);

    std::cout << "\nCase: S=" << S << " K=" << K << " T=" << T
              << " r=" << r << " sigma=" << sigma << "\n";
    std::cout << "BS(call)=" << std::setprecision(8) << bs
              << "  time_ms=" << std::setprecision(4) << bs_ms << "\n\n";

    std::cout << std::left
              << std::setw(12) << "n_paths"
              << std::setw(14) << "mode"
              << std::setw(14) << "price"
              << std::setw(14) << "stderr"
              << std::setw(14) << "time_ms"
              << "\n";

    auto run = [&](MCMode mode, const char* name, std::size_t n) {
        const auto a0 = std::chrono::steady_clock::now();
        const auto mc = mcCallPrice(S, K, T, r, sigma, n, seed, mode);
        const auto a1 = std::chrono::steady_clock::now();

        std::cout << std::left
                  << std::setw(12) << n
                  << std::setw(14) << name
                  << std::setw(14) << std::setprecision(8) << mc.price
                  << std::setw(14) << std::setprecision(6) << mc.stderr
                  << std::setw(14) << std::setprecision(4) << ms_since(a0, a1)
                  << "\n";
    };

    for (auto n : paths_list) {
        run(MCMode::Plain,      "plain", n);
        run(MCMode::Antithetic, "anti",  n);
        run(MCMode::ControlVariateBS,    "cv",    n);
        run(MCMode::AntitheticControlBS, "anti+cv", n);
    }
}

int main() {
    const std::vector<std::size_t> paths = {1000, 5000, 20000, 100000, 200000};
    const std::uint64_t seed = 123456;

    // Primary benchmark case 
    bench_case(100, 100, 1.0, 0.05, 0.2, seed, paths);
     
    // secondary case to verify values 
    bench_case(100, 110, 0.5, 0.03, 0.25, seed, paths);

    return 0;
}
