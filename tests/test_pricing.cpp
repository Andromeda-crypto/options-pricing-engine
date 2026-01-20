#include <iostream>
#include <cmath>
#include "black_scholes.h"



static bool approx(double a, double b, double tol=1e-3) {
    return std::fabs(a - b) <= tol;
}


int main() {
    {
        double c = callPrice(100,100,1.0,0.05,0.2);
        double p = putPrice(100,100, 1.0,0.05,0.2);

        if (!approx(c,10.4506)) {
            std::cerr <<"Fail call case A : " <<c<< "\n";
            return 1;
        }
        if (!approx(p,5.573)) {
            std::cerr<<"Fail put case A : " <<p<< "\n";
            return 1;
        }
    }

    {
        double c = callPrice(100,110,1.0,0.05,0.2);
        double p = putPrice(100,110,1.0,0.05,0.2);

        if (!approx(c,6.0401)) {
            std::cerr <<"Fail call case B : " <<c<< "\n";
            return 1;
        }
        if (!approx(p,10.6753)) {
            std::cerr <<"Fail put case B : " <<p<< "\n";
            return 1;
        }
    }
    std::cout <<"PASS : Black-scholes pricing\n";
    return 0;
}