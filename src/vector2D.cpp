#include <iostream>
#include "vector2D.hpp"
#include <cmath>

// Constructeur
vector2D::vector2D(double x_val, double y_val) : x(x_val), y(y_val) {}

// Méthode print
void vector2D::print() const {
    std::cout << "(" << x << ", " << y << ")" << std::endl;
}

// Surcharge de l'opérateur +
vector2D vector2D::operator+(const vector2D& vec2) const {
    return vector2D(x + vec2.x, y + vec2.y);
}

// Surcharge de l'opérateur -
vector2D vector2D::operator-(const vector2D& vec2) const {
    return vector2D(x - vec2.x, y - vec2.y);
}

// Norme
double vector2D::norme() const {
    return std::sqrt(x * x + y * y);
}

// Surcharge de l'opérateur *
vector2D vector2D::operator*(const double& lambda) const {
    return vector2D(lambda * x, lambda * y);
}

// Normalisation
vector2D vector2D::normalize() const {
    double norm = norme();
    if (norm == 0.0) return vector2D(0.0, 0.0);
    return vector2D(x / norm, y / norm);
}

// Produit scalaire
double vector2D::dot(const vector2D& vec2) const {
    return x * vec2.x + y * vec2.y;
}