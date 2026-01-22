#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include "black_scholes.h"
#include "greeks.h"
#include "monte_carlo.h"
#include "implied_vol.h"

namespace {

struct Args {
    std::string method = "bs";     // bs | mc
    std::string type   = "call";   // call | put
    std::string mode   = "plain";  // plain | anti | cv | anti+cv (mc only)

    double S = NAN;
    double K = NAN;
    double T = NAN;
    double r = NAN;
    double sigma = NAN;

    std::size_t paths = 200000;
    std::uint64_t seed = 123456;

    bool greeks = false;

    // Implied vol (BS only)
    bool iv = false;
    double market_price = NAN;
    double iv_init = 0.2;
};

static bool is_flag(const std::string& s) {
    return s.rfind("--", 0) == 0;
}

static void usage(const char* prog) {
    std::cerr
        << "Usage:\n"
        << "  " << prog << " --method bs|mc --type call|put --spot S --strike K --T T --r r "
        << "[--sigma sigma | --iv --market_price X] [options]\n\n"
        << "Core options:\n"
        << "  --method   bs|mc\n"
        << "  --type     call|put\n"
        << "  --spot     S\n"
        << "  --strike   K\n"
        << "  --T        T\n"
        << "  --r        r\n"
        << "  --sigma    sigma                     (required unless using --iv)\n\n"
        << "Monte Carlo options (when --method mc):\n"
        << "  --mode     plain|anti|cv|anti+cv\n"
        << "  --paths    N                         (default 200000)\n"
        << "  --seed     uint64                    (default 123456)\n\n"
        << "Extras:\n"
        << "  --greeks   compute greeks (bs only for now)\n"
        << "  --iv       compute implied vol from --market_price (bs only)\n"
        << "  --market_price X   market option price for implied vol\n"
        << "  --iv_init X        initial guess for sigma (default 0.2)\n"
        << "  --help, -h         show this help\n\n"
        << "Examples:\n"
        << "  " << prog << " --method bs --type call --spot 100 --strike 100 --T 1 --r 0.05 --sigma 0.2 --greeks\n"
        << "  " << prog << " --method mc --type call --spot 100 --strike 100 --T 1 --r 0.05 --sigma 0.2 --mode anti+cv --paths 200000 --seed 7\n"
        << "  " << prog << " --method bs --type call --spot 100 --strike 100 --T 1 --r 0.05 --iv --market_price 10.45 --greeks\n";
}

static double parse_double(const std::string& s, const std::string& name) {
    try {
        size_t idx = 0;
        double v = std::stod(s, &idx);
        if (idx != s.size()) throw std::invalid_argument("trailing chars");
        return v;
    } catch (...) {
        throw std::runtime_error("Invalid value for " + name + ": '" + s + "'");
    }
}

static std::uint64_t parse_u64(const std::string& s, const std::string& name) {
    try {
        size_t idx = 0;
        unsigned long long v = std::stoull(s, &idx);
        if (idx != s.size()) throw std::invalid_argument("trailing chars");
        return static_cast<std::uint64_t>(v);
    } catch (...) {
        throw std::runtime_error("Invalid value for " + name + ": '" + s + "'");
    }
}

static std::size_t parse_size(const std::string& s, const std::string& name) {
    try {
        size_t idx = 0;
        unsigned long long v = std::stoull(s, &idx);
        if (idx != s.size()) throw std::invalid_argument("trailing chars");
        return static_cast<std::size_t>(v);
    } catch (...) {
        throw std::runtime_error("Invalid value for " + name + ": '" + s + "'");
    }
}

static MCMode parse_mc_mode(const std::string& s) {
    if (s == "plain")   return MCMode::Plain;
    if (s == "anti")    return MCMode::Antithetic;
    if (s == "cv")      return MCMode::ControlVariateBS;
    if (s == "anti+cv") return MCMode::AntitheticControlBS;
    throw std::runtime_error("Invalid --mode '" + s + "'. Use plain|anti|cv|anti+cv");
}

static void validate(const Args& a) {
    auto bad = [&](const std::string& msg) { throw std::runtime_error(msg); };

    if (!(a.method == "bs" || a.method == "mc")) bad("Invalid --method. Use bs|mc");
    if (!(a.type == "call" || a.type == "put"))  bad("Invalid --type. Use call|put");

    // Required numeric inputs (sigma not required if using --iv)
    if (!std::isfinite(a.S) || !std::isfinite(a.K) || !std::isfinite(a.T) ||
        !std::isfinite(a.r) || (!a.iv && !std::isfinite(a.sigma))) {
        bad("Missing required inputs. Provide --spot --strike --T --r and --sigma (unless using --iv)");
    }

    if (a.S <= 0.0) bad("--spot must be > 0");
    if (a.K <= 0.0) bad("--strike must be > 0");
    if (a.T < 0.0)  bad("--T must be >= 0");

    if (!a.iv) {
        if (a.sigma < 0.0) bad("--sigma must be >= 0");
    }

    if (a.method == "mc") {
        if (a.paths < 2) bad("--paths must be >= 2 for MC");
    }

    if (a.greeks && a.method != "bs") {
        bad("--greeks is currently supported for --method bs only (add MC greeks later)");
    }

    if (a.iv) {
        if (a.method != "bs") bad("--iv is supported only for --method bs");
        if (!std::isfinite(a.market_price)) bad("--iv requires --market_price X");
        if (a.iv_init <= 0.0) bad("--iv_init must be > 0");
        if (a.T == 0.0) bad("--iv is not supported for T=0 (vol not identifiable at expiry)");
    }
}

static Args parse_args(int argc, char** argv) {
    Args a;

    for (int i = 1; i < argc; i++) {
        std::string key = argv[i];

        if (key == "--help" || key == "-h") {
            usage(argv[0]);
            std::exit(0);
        }

        if (!is_flag(key)) {
            throw std::runtime_error("Unexpected token: '" + key + "'. Flags must start with --");
        }

        if (key == "--greeks") {
            a.greeks = true;
            continue;
        }
        if (key == "--iv") {
            a.iv = true;
            continue;
        }

        if (i + 1 >= argc) {
            throw std::runtime_error("Missing value after '" + key + "'");
        }

        std::string val = argv[++i];

        if (key == "--method") a.method = val;
        else if (key == "--type") a.type = val;
        else if (key == "--mode") a.mode = val;
        else if (key == "--spot") a.S = parse_double(val, "--spot");
        else if (key == "--strike") a.K = parse_double(val, "--strike");
        else if (key == "--T") a.T = parse_double(val, "--T");
        else if (key == "--r") a.r = parse_double(val, "--r");
        else if (key == "--sigma") a.sigma = parse_double(val, "--sigma");
        else if (key == "--paths") a.paths = parse_size(val, "--paths");
        else if (key == "--seed") a.seed = parse_u64(val, "--seed");
        else if (key == "--market_price") a.market_price = parse_double(val, "--market_price");
        else if (key == "--iv_init") a.iv_init = parse_double(val, "--iv_init");
        else {
            throw std::runtime_error("Unknown flag: '" + key + "'");
        }
    }

    validate(a);
    return a;
}

static void print_header(const Args& a) {
    std::cout << std::fixed;

    std::cout << "Inputs\n";
    std::cout << "  method: " << a.method;
    if (a.method == "mc") std::cout << " (" << a.mode << ")";
    std::cout << "\n";
    std::cout << "  type:   " << a.type << "\n";
    std::cout << "  S:      " << a.S << "\n";
    std::cout << "  K:      " << a.K << "\n";
    std::cout << "  T:      " << a.T << "\n";
    std::cout << "  r:      " << a.r << "\n";
    if (!a.iv) {
        std::cout << "  sigma:  " << a.sigma << "\n";
    } else {
        std::cout << "  market_price: " << a.market_price << "\n";
        std::cout << "  iv_init:      " << a.iv_init << "\n";
    }

    if (a.method == "mc") {
        std::cout << "  paths:  " << a.paths << "\n";
        std::cout << "  seed:   " << a.seed << "\n";
    }
    std::cout << "\n";
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Args a = parse_args(argc, argv);

        // Early parse of MC mode to catch invalid mode even if not used
        if (a.method == "mc") (void)parse_mc_mode(a.mode);

        print_header(a);

        if (a.method == "bs") {
            double sigma_used = a.sigma;

            if (a.iv) {
                IVResult iv;
                if (a.type == "call") iv = impliedVolCall(a.market_price, a.S, a.K, a.T, a.r, a.iv_init);
                else                  iv = impliedVolPut (a.market_price, a.S, a.K, a.T, a.r, a.iv_init);

                if (!iv.converged || !std::isfinite(iv.sigma)) {
                    throw std::runtime_error("Implied vol failed (check market_price bounds or try a different iv_init).");
                }

                sigma_used = iv.sigma;

                std::cout << "Implied Volatility (Black–Scholes)\n";
                std::cout << "  iv:           " << std::setprecision(8) << sigma_used << "\n";
                std::cout << "  iterations:   " << iv.iterations << "\n\n";
            }

            double price = NAN;
            if (a.type == "call") price = callPrice(a.S, a.K, a.T, a.r, sigma_used);
            else                  price = putPrice (a.S, a.K, a.T, a.r, sigma_used);

            std::cout << "Result (Black–Scholes)\n";
            std::cout << "  price:  " << std::setprecision(8) << price << "\n";

            if (a.greeks) {
                std::cout << "\nGreeks (Analytical)\n";
                if (a.type == "call") {
                    std::cout << "  delta:  " << std::setprecision(8) << callDelta(a.S, a.K, a.T, a.r, sigma_used) << "\n";
                } else {
                    std::cout << "  delta:  " << std::setprecision(8) << putDelta(a.S, a.K, a.T, a.r, sigma_used) << "\n";
                }
                std::cout << "  gamma:  " << std::setprecision(10) << gamma(a.S, a.K, a.T, a.r, sigma_used) << "\n";
                std::cout << "  vega:   " << std::setprecision(8) << vega(a.S, a.K, a.T, a.r, sigma_used) << "\n";
            }

            return 0;
        }

        // MC
        const MCMode mode = parse_mc_mode(a.mode);
        MCResult res;
        if (a.type == "call") res = mcCallPrice(a.S, a.K, a.T, a.r, a.sigma, a.paths, a.seed, mode);
        else                  res = mcPutPrice (a.S, a.K, a.T, a.r, a.sigma, a.paths, a.seed, mode);

        std::cout << "Result (Monte Carlo)\n";
        std::cout << "  price:  " << std::setprecision(8) << res.price << "\n";
        std::cout << "  stderr: " << std::setprecision(8) << res.stderr << "\n";
        std::cout << "  95% CI: [" << std::setprecision(8) << res.ci_low
                  << ", " << std::setprecision(8) << res.ci_high << "]\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        usage(argv[0]);
        return 1;
    }
}