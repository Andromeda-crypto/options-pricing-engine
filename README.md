# Options Pricing Engine

A high-performance C++ library for pricing European options using analytical methods (Black-Scholes) and numerical simulation (Monte Carlo). This project demonstrates advanced quantitative finance techniques, including variance reduction methods, Greeks computation, and implied volatility calculation.

## Overview

This options pricing engine provides a comprehensive suite of tools for quantitative analysis of European call and put options. The implementation focuses on accuracy, performance, and extensibility, making it suitable for both educational purposes and production-grade quantitative finance applications.

## Features

### Core Pricing Methods

- **Black-Scholes Analytical Pricing**: Fast, exact pricing for European options using the closed-form Black-Scholes formula
- **Monte Carlo Simulation**: Flexible numerical pricing with multiple variance reduction techniques:
  - Plain Monte Carlo
  - Antithetic variates
  - Control variate (using Black-Scholes as control)
  - Combined antithetic + control variate

### Advanced Capabilities

- **Greeks Calculation**: Analytical computation of option sensitivities:
  - Delta (price sensitivity to underlying)
  - Gamma (delta sensitivity)
  - Vega (volatility sensitivity)
- **Implied Volatility**: Newton-Raphson-based solver to extract implied volatility from market prices
- **Statistical Analysis**: Monte Carlo results include standard errors and 95% confidence intervals

### Engineering Excellence

- **Comprehensive Testing**: Unit tests covering edge cases, regression scenarios, and variance reduction validation
- **Performance Benchmarks**: Built-in benchmarking suite for performance analysis
- **Modern C++17**: Clean, maintainable code following modern C++ best practices
- **Modular Design**: Well-structured header/implementation separation for easy integration

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, or MSVC)
- Make (or equivalent build system)

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build all targets
make

# Or build specific targets
make options_pricer      # Main executable
make test_mc            # Monte Carlo tests
make test_iv            # Implied volatility tests
make test_greeks        # Greeks tests
make performance        # Benchmark suite
```

## Usage

### Command-Line Interface

The main `options_pricer` executable provides a flexible command-line interface for pricing options.

#### Basic Black-Scholes Pricing

```bash
./options_pricer --method bs --type call --spot 100 --strike 100 --T 1 --r 0.05 --sigma 0.2
```

#### Black-Scholes with Greeks

```bash
./options_pricer --method bs --type call --spot 100 --strike 100 --T 1 --r 0.05 --sigma 0.2 --greeks
```

#### Implied Volatility Calculation

```bash
./options_pricer --method bs --type call --spot 100 --strike 100 --T 1 --r 0.05 --iv --market_price 10.45 --greeks
```

#### Monte Carlo Pricing

```bash
# Plain Monte Carlo
./options_pricer --method mc --type call --spot 100 --strike 100 --T 1 --r 0.05 --sigma 0.2 --mode plain --paths 200000

# With variance reduction
./options_pricer --method mc --type call --spot 100 --strike 100 --T 1 --r 0.05 --sigma 0.2 --mode anti+cv --paths 200000 --seed 7
```

### Command-Line Options

**Core Options:**

- `--method`: Pricing method (`bs` or `mc`)
- `--type`: Option type (`call` or `put`)
- `--spot S`: Current spot price
- `--strike K`: Strike price
- `--T T`: Time to expiration (years)
- `--r r`: Risk-free interest rate
- `--sigma sigma`: Volatility (required unless using `--iv`)

**Monte Carlo Options:**

- `--mode`: Simulation mode (`plain`, `anti`, `cv`, `anti+cv`)
- `--paths N`: Number of simulation paths (default: 200000)
- `--seed N`: Random seed for reproducibility (default: 123456)

**Advanced Options:**

- `--greeks`: Compute option Greeks (Black-Scholes only)
- `--iv`: Compute implied volatility from market price
- `--market_price X`: Market option price for implied vol calculation
- `--iv_init X`: Initial guess for implied vol solver (default: 0.2)

## Project Structure

```

options-pricing-engine/
├── include/              # Header files
│   ├── black_scholes.h  # Black-Scholes pricing
│   ├── monte_carlo.h    # Monte Carlo simulation
│   ├── greeks.h         # Greeks calculation
│   ├── implied_vol.h    # Implied volatility solver
│   └── utils.h          # Utility functions
├── src/                  # Implementation files
│   ├── black_scholes.cpp
│   ├── monte_carlo.cpp
│   ├── greeks.cpp
│   ├── implied_vol.cpp
│   ├── random.cpp       # Random number generation
│   └── main.cpp         # CLI entry point
├── tests/                # Test suite
│   ├── test_mc.cpp
│   ├── test_mc_regression.cpp
│   ├── test_mc_variance_reduction.cpp
│   ├── test_mc_edge_cases.cpp
│   ├── test_iv.cpp
│   └── test_greeks.cpp
├── benchmarks/           # Performance benchmarks
│   └── performance.cpp
└── CMakeLists.txt       # Build configuration
```

## Technical Details

### Black-Scholes Implementation

The analytical pricing uses the standard Black-Scholes formula for European options:

- **Call Price**: \(C = S_0 N(d_1) - K e^{-rT} N(d_2)\)
- **Put Price**: \(P = K e^{-rT} N(-d_2) - S_0 N(-d_1)\)

Where:

- \(d_1 = \frac{\ln(S_0/K) + (r + \sigma^2/2)T}{\sigma\sqrt{T}}\)
- \(d_2 = d_1 - \sigma\sqrt{T}\)

### Monte Carlo Methods

The Monte Carlo implementation uses geometric Brownian motion for path simulation:

\[S_T = S_0 \exp\left((r - \frac{\sigma^2}{2})T + \sigma\sqrt{T}Z\right)\]

**Variance Reduction Techniques:**

1. **Antithetic Variates**: Uses both \(Z\) and \(-Z\) to reduce variance
2. **Control Variate**: Uses Black-Scholes analytical price as control to reduce variance
3. **Combined Method**: Applies both techniques simultaneously for maximum efficiency

### Implied Volatility

The implied volatility solver uses the Newton-Raphson method with analytical vega for fast convergence. The implementation includes robust error handling and convergence checks.

## Running Tests

```bash
cd build

# Run all test suites
./test_mc
./test_mc_regression
./test_mc_variance_reduction
./test_mc_edge_cases
./test_iv
./test_greeks
```

## Performance Benchmarks

Run the performance benchmark to compare different Monte Carlo methods:

```bash
./performance
```

The benchmark suite evaluates:

- Black-Scholes baseline performance
- Monte Carlo performance across different path counts
- Variance reduction effectiveness
- Computational efficiency

## Mathematical Foundations

This implementation is based on established quantitative finance theory:

- **Black-Scholes Model**: The foundational model for European option pricing
- **Monte Carlo Methods**: Numerical methods for complex derivative pricing
- **Variance Reduction**: Techniques to improve Monte Carlo convergence
- **Greeks**: Risk sensitivity measures essential for portfolio management
- **Implied Volatility**: Market-implied volatility extraction from option prices

## Future Enhancements

Potential areas for extension:

- American option pricing (binomial/trinomial trees)
- Exotic options (barriers, Asian, etc.)
- Monte Carlo Greeks computation
- Parallel/GPU acceleration
- Additional variance reduction techniques (importance sampling, stratified sampling)

## License

This project is provided as-is for educational and professional development purposes.

---

**Note**: This implementation is designed for educational and research purposes. For production use in financial applications, additional validation, error handling, and regulatory compliance considerations should be addressed.
