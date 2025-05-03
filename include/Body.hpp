#ifndef BODY_HPP
#define BODY_HPP


#include <iostream>
#include <string>
#include "vector2D.hpp"

struct Body {

    std::string nom;
    vector2D position;
    vector2D vitesse;
    double m;
    vector2D force;

    // constructeur
    Body(const std::string& nom, vector2D position, vector2D vitesse, double m);

    // les méthodes
    void resetForce();
    void addForce(const Body& corp);
    void update(double dt);

};
#endif // BODY_HPP