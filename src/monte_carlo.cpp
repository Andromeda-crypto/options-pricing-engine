#include "monte_carlo.h"
#include "utils.h"
#include "black_scholes.h"

#include <cmath>
#include <algorithm>

namespace {
    struct RunningStats {
        std::size_t n = 0;
        double mean = 0.0;
        double M2 = 0.0;

        void push(double x) {
            n++;
            const double dn = static_cast<double>(n);
            const double delta = x - mean;
            mean += delta / dn;
            const double delta2 = x - mean;
            M2 += delta * delta2;
        }

        double variance_unbiased() const {
            return (n > 1) ? (M2 / static_cast<double>(n - 1)) : 0.0;
        }
    };

    inline double terminal_price(double S, double T, double r, double sigma, double Z) {
        const double drift = (r - 0.5 * sigma * sigma) * T;
        const double diffusion = sigma * std::sqrt(T) * Z;
        return S * std::exp(drift + diffusion);
    }

    inline MCResult finalize(double df, const RunningStats& stats) {
        const double var = stats.variance_unbiased();
        const double price = df * stats.mean;
        const double stderr = df * std::sqrt(var / static_cast<double>(stats.n));

        // 95% CI using normal approximation
        const double z = 1.959963984540054; 
        const double ci_low = price - z * stderr;
        const double ci_high = price + z * stderr;

        return {price, stderr, ci_low, ci_high};
    }

    inline double payoff_call(double ST, double K) { return std::max(ST - K, 0.0); }
    inline double payoff_put (double ST, double K) { return std::max(K - ST, 0.0); }

    struct ControlVarSums {
        std::size_t n = 0;
        double sumX = 0.0, sumY = 0.0, sumXX = 0.0, sumYY = 0.0, sumXY = 0.0;

        void push(double X, double Y) {
            n++;
            sumX += X; sumY += Y;
            sumXX += X * X;
            sumYY += Y * Y;
            sumXY += X * Y;
        }

        double meanX() const { return sumX / static_cast<double>(n); }
        double meanY() const { return sumY / static_cast<double>(n); }

        double varY_unbiased() const {
            if (n < 2) return 0.0;
            const double n_d = static_cast<double>(n);
            const double meanYv = sumY / n_d;
            const double ss = sumYY - n_d * meanYv * meanYv;
            return ss / static_cast<double>(n - 1);
        }

        double covXY_unbiased() const {
            if (n < 2) return 0.0;
            const double n_d = static_cast<double>(n);
            const double meanXv = sumX / n_d;
            const double meanYv = sumY / n_d;
            const double ss = sumXY - n_d * meanXv * meanYv;
            return ss / static_cast<double>(n - 1);
        }
    };
}

MCResult mcCallPrice(double S, double K, double T, double r, double sigma,
                     std::size_t n_paths, std::uint64_t seed, MCMode mode) {
    if (S <= 0.0 || K <= 0.0 || T < 0.0 || sigma < 0.0 || n_paths < 2) return {NAN, NAN, NAN, NAN};

    if (T == 0.0) {
        const double p = std::max(S - K, 0.0);
        return {p, 0.0, p, p};
    }

    const double df = std::exp(-r * T);
    std::uint64_t state = seed;

    const bool useAnti = (mode == MCMode::Antithetic || mode == MCMode::AntitheticControlBS);
    const bool useCV   = (mode == MCMode::ControlVariateBS || mode == MCMode::AntitheticControlBS);

    ControlVarSums cv;
    RunningStats stats;

    for (std::size_t i = 0; i < n_paths; i++) {
        const double Z = rand_standard_normal(state);

        const double ST1 = terminal_price(S, T, r, sigma,  Z);
        const double payoff1 = payoff_call(ST1, K);
        double X = df * payoff1;
        double Y = df * ST1;

        if (useAnti) {
            const double ST2 = terminal_price(S, T, r, sigma, -Z);
            const double payoff2 = payoff_call(ST2, K);
            const double X2 = df * payoff2;
            const double Y2 = df * ST2;

            X = 0.5 * (X + X2);
            Y = 0.5 * (Y + Y2);
        }

        if (useCV) cv.push(X, Y);
        else stats.push(X);
    }

    if (!useCV) {
        return finalize(1.0, stats); 
    }

    const double EY = S; 
    const double varY = cv.varY_unbiased();
    const double covXY = cv.covXY_unbiased();
    const double b = (varY > 0.0) ? (covXY / varY) : 0.0;

    RunningStats controlled;
    // Re-run stream deterministically for controlled samples 
    state = seed;
    for (std::size_t i = 0; i < n_paths; i++) {
        const double Z = rand_standard_normal(state);

        const double ST1 = terminal_price(S, T, r, sigma,  Z);
        const double payoff1 = payoff_call(ST1, K);
        double X = df * payoff1;
        double Y = df * ST1;

        if (useAnti) {
            const double ST2 = terminal_price(S, T, r, sigma, -Z);
            const double payoff2 = payoff_call(ST2, K);
            const double X2 = df * payoff_call(ST2, K);
            const double Y2 = df * ST2;

            X = 0.5 * (X + X2);
            Y = 0.5 * (Y + Y2);
        }

        const double Xstar = X - b * (Y - EY);
        controlled.push(Xstar);
    }

    // finalize with df=1 because samples are already discounted
    return finalize(1.0, controlled);
}

MCResult mcPutPrice(double S, double K, double T, double r, double sigma,
                    std::size_t n_paths, std::uint64_t seed, MCMode mode) {
    if (S <= 0.0 || K <= 0.0 || T < 0.0 || sigma < 0.0 || n_paths < 2) return {NAN, NAN, NAN, NAN};

    if (T == 0.0) {
        const double p = std::max(K - S, 0.0);
        return {p, 0.0, p, p};
    }

    const double df = std::exp(-r * T);
    std::uint64_t state = seed;

    const bool useAnti = (mode == MCMode::Antithetic || mode == MCMode::AntitheticControlBS);
    const bool useCV   = (mode == MCMode::ControlVariateBS || mode == MCMode::AntitheticControlBS);

    ControlVarSums cv;
    RunningStats stats;

    for (std::size_t i = 0; i < n_paths; i++) {
        const double Z = rand_standard_normal(state);

        const double ST1 = terminal_price(S, T, r, sigma,  Z);
        const double payoff1 = payoff_put(ST1, K);
        double X = df * payoff1;
        double Y = df * ST1;

        if (useAnti) {
            const double ST2 = terminal_price(S, T, r, sigma, -Z);
            const double payoff2 = payoff_put(ST2, K);
            const double X2 = df * payoff2;
            const double Y2 = df * ST2;

            X = 0.5 * (X + X2);
            Y = 0.5 * (Y + Y2);
        }

        if (useCV) cv.push(X, Y);
        else stats.push(X);
    }

    if (!useCV) {
        return finalize(1.0, stats);
    }

    const double EY = S; 
    const double varY = cv.varY_unbiased();
    const double covXY = cv.covXY_unbiased();
    const double b = (varY > 0.0) ? (covXY / varY) : 0.0;

    RunningStats controlled;
    state = seed;
    for (std::size_t i = 0; i < n_paths; i++) {
        const double Z = rand_standard_normal(state);

        const double ST1 = terminal_price(S, T, r, sigma,  Z);
        const double payoff1 = payoff_put(ST1, K);
        double X = df * payoff1;
        double Y = df * ST1;

        if (useAnti) {
            const double ST2 = terminal_price(S, T, r, sigma, -Z);
            const double payoff2 = payoff_put(ST2, K);
            const double X2 = df * payoff2;
            const double Y2 = df * ST2;

            X = 0.5 * (X + X2);
            Y = 0.5 * (Y + Y2);
        }

        const double Xstar = X - b * (Y - EY);
        controlled.push(Xstar);
    }

    return finalize(1.0, controlled);
}
