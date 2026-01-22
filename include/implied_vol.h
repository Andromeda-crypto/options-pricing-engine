#ifndef IMPLIED_VOL_H
#define IMPLIED_VOL_H

struct IVResult {
    double sigma;
    int iterations;
    bool converged;
};

IVResult impliedVolCall(double market_price, double S, double K, double T, double r,
                        double init_sigma = 0.2, double tol = 1e-8, int max_iter = 100);

IVResult impliedVolPut(double market_price, double S, double K, double T, double r,
                       double init_sigma = 0.2, double tol = 1e-8, int max_iter = 100);

#endif