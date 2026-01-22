#include "implied_vol.h"
#include "black_scholes.h"
#include "greeks.h"

#include <cmath>
#include <algorithm>

namespace {
    inline double df(double r, double T) { return std::exp(-r * T); }

    // No-arbitrage bounds
    inline bool call_bounds_ok(double C, double S, double K, double r, double T) {
        const double lower = std::max(0.0, S - K * df(r, T));
        const double upper = S;
        return (C >= lower - 1e-12 && C <= upper + 1e-12);
    }

    inline bool put_bounds_ok(double P, double S, double K, double r, double T) {
        const double lower = std::max(0.0, K * df(r, T) - S);
        const double upper = K * df(r, T);
        return (P >= lower - 1e-12 && P <= upper + 1e-12);
    }

    // Evaluate f(sigma) = BS_price(sigma) - market_price
    inline double f_call(double sigma, double mkt, double S, double K, double T, double r) {
        return callPrice(S, K, T, r, sigma) - mkt;
    }
    inline double f_put(double sigma, double mkt, double S, double K, double T, double r) {
        return putPrice(S, K, T, r, sigma) - mkt;
    }

    template <typename F, typename Vega>
    IVResult solve_iv(F f, Vega vega_fn,
                      double mkt, double S, double K, double T, double r,
                      double init_sigma, double tol, int max_iter) {
        // sigma bounds (expandable if needed)
        double lo = 1e-6;
        double hi = 5.0;

        // Ensure bracket exists; if not, expand hi
        double flo = f(lo, mkt, S, K, T, r);
        double fhi = f(hi, mkt, S, K, T, r);

        for (int expand = 0; expand < 20 && flo * fhi > 0.0; expand++) {
            hi *= 2.0;
            fhi = f(hi, mkt, S, K, T, r);
            if (hi > 100.0) break;
        }

        // If still no bracket, fail
        if (flo * fhi > 0.0) {
            return {NAN, 0, false};
        }

        // Start Newton at init, but clamp into [lo,hi]
        double sigma = std::clamp(init_sigma, lo, hi);
        for (int it = 1; it <= max_iter; it++) {
            const double fs = f(sigma, mkt, S, K, T, r);

            if (std::fabs(fs) < tol) {
                return {sigma, it, true};
            }
            // Maintain bracket using current sigma
            if (fs > 0.0) {
                hi = sigma;
                fhi = fs;
            } else {
                lo = sigma;
                flo = fs;
            }

            const double v = vega_fn(S, K, T, r, sigma);
            // If vega too small, use bisection
            double next;
            if (v < 1e-10 || !std::isfinite(v)) {
                next = 0.5 * (lo + hi);
            } else {
                // Newton step
                next = sigma - fs / v;
                // If Newton jumps outside bracket, fall back to bisection
                if (next <= lo || next >= hi || !std::isfinite(next)) {
                    next = 0.5 * (lo + hi);
                }
            }
            // Convergence on sigma change
            if (std::fabs(next - sigma) < tol) {
                sigma = next;
                return {sigma, it, true};
            }

            sigma = next;
        }
        return {sigma, max_iter, false};
    }
}

IVResult impliedVolCall(double market_price, double S, double K, double T, double r,
                        double init_sigma, double tol, int max_iter) {
    if (!(S > 0.0 && K > 0.0 && T >= 0.0)) return {NAN, 0, false};
    if (!std::isfinite(market_price)) return {NAN, 0, false};

    if (T == 0.0) {
        // At expiry, vol is not identifiable unless price equals intrinsic exactly
        return {NAN, 0, false};
    }

    if (!call_bounds_ok(market_price, S, K, r, T)) {
        return {NAN, 0, false};
    }

    return solve_iv(f_call, vega, market_price, S, K, T, r, init_sigma, tol, max_iter);
}

IVResult impliedVolPut(double market_price, double S, double K, double T, double r,
                       double init_sigma, double tol, int max_iter) {
    if (!(S > 0.0 && K > 0.0 && T >= 0.0)) return {NAN, 0, false};
    if (!std::isfinite(market_price)) return {NAN, 0, false};

    if (T == 0.0) {
        return {NAN, 0, false};
    }

    if (!put_bounds_ok(market_price, S, K, r, T)) {
        return {NAN, 0, false};
    }

    return solve_iv(f_put, vega, market_price, S, K, T, r, init_sigma, tol, max_iter);
}
