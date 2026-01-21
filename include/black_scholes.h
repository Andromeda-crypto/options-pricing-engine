// black_scholes.h

#ifndef BLACK_SCHOLES_H
#define BLACK_SCHOLES_H

double callPrice(double S, double K, double T, double r, double sigma);
double putPrice(double S, double K, double T, double r, double sigma);

#endif