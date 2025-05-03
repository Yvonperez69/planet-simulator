#include <iostream>
#include "Body.hpp"

const double G = 6.674e-11; // constante gravitationnelle

// Constructeur

Body::Body(const std::string&  nom, vector2D position, vector2D vitesse, double m) : nom(nom),
position(position),
vitesse(vitesse),
m(m),
force(0,0) {}

// les méthodes

void Body::resetForce() {
    force = vector2D(0.0,0.0);
}
void Body::addForce(const Body& corp) {
    vector2D delta = position - corp.position;
    double distance = delta.norme();
    double eps = 1e6;
    double norme_force =  G*(m*corp.m)/(distance * distance + eps * eps);
    vector2D direction =  delta.normalize();

    force = force - direction*norme_force;
}
void Body::update(double dt) {
    vector2D accel = force*(1.0/m);
    vitesse = vitesse + accel*dt;
    position = position + vitesse*dt;
}

