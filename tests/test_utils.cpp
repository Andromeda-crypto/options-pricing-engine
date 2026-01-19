#include <iostream>
#include <cmath>
#include "utils.h"

static bool approx(double a, double b, double tol = 1e-12) {
    return std::fabs(a - b) <= tol;
}

int main() {

    if (!approx(normal_cdf(0.0), 0.5, 1e-15)) {
        std::cerr << "FAIL: normal_cdf(0)\n";
        return 1;
    }

    if (!approx(normal_pdf(0.0), 0.39894228040143267794, 1e-15)) {
        std::cerr << "FAIL: normal_pdf(0)\n";
        return 1;
    }

    
    if (!approx(normal_cdf(1.0), 0.8413447460685429, 1e-12)) {
        std::cerr << "FAIL: normal_cdf(1)\n";
        return 1;
    }

    double x = 1.7;
    if (!approx(normal_pdf(-x), normal_pdf(x), 1e-15)) {
        std::cerr << "FAIL: pdf symmetry\n";
        return 1;
    }

    if (!approx(normal_cdf(-x), 1.0 - normal_cdf(x), 1e-12)) {
        std::cerr << "FAIL: cdf symmetry\n";
        return 1;
    }
    if (!(normal_cdf(-10.0) < 1e-20)) {
        std::cerr << "FAIL: cdf(-10) not near 0\n";
        return 1;
    }
    if (!(1.0 - normal_cdf(10.0) < 1e-20)) {
        std::cerr << "FAIL: cdf(10) not near 1\n";
        return 1;
    }

    std::cout << "PASS: utils normal_pdf/normal_cdf\n";
    return 0;
}
