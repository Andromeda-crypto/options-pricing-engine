// greeks.h

#ifndef GREEKS_H
#define GREEKS_H

double callDelta(double S, double K, double T, double r, double sigma);
double putDelta(double S, double K, double T, double r, double sigma);

double gamma(double S, double K, double T, double r, double sigma);
double vega(double S, double K, double T, double r, double sigma);


#endif