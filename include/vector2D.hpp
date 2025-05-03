#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include <iostream>

struct vector2D {

    double x;
    double y;

    // constructeur
    vector2D(double x_val = 0.0, double y_val = 0.0);

    // les méthodes
    void print() const;
    vector2D operator+(const vector2D& vec2) const;
    vector2D operator-(const vector2D& vec2) const;
    double norme() const;
    vector2D operator*(const double& lambda) const;
    vector2D normalize() const;
    double dot(const vector2D& vec2) const;

};

#endif // VECTOR2D_HPP