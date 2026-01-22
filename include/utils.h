#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

double normal_pdf(double x);
double normal_cdf(double x);

// black scholes helpers
double bs_d1(double S, double K, double T, double r, double sigma);
double bs_d2_from_d1(double d1, double T, double sigma);

// RNG utilities 

double rand_standard_normal(std::uint64_t& state);
double rand_uniform_01(std::uint64_t& state);


#endif