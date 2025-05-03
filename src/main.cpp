#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include "vector2D.hpp"
#include "Body.hpp"
#include <string>



// Trace un cercle rempli de radius r centré en (cx,cy)
inline void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
            if (dx*dx + dy*dy <= r*r) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

inline SDL_Point worldToScreen(const vector2D& pos,
    float scale,
    int W, int H,
    int offsetX, int offsetY)
{
// 1) Applique le zoom
float fx = pos.x * scale;
float fy = - pos.y * scale;

// 2) Centre dans la fenêtre et applique le pan
//    Note : on inverse l'axe Y pour SDL (y vers le bas)
float sx = (W / 2.0f) + fx + offsetX;
float sy = (H / 2.0f) - fy + offsetY;

// 3) Arrondi et cast en int
return SDL_Point{
static_cast<int>(std::round(sx)),
static_cast<int>(std::round(sy))
};
}

int main() {
    // ——— Paramètres de simulation ———
    const int   N     = 100;
    const double dt    = 36000;
    const float  scale = 3*1e-9f;
    int   offsetX = 0, offsetY = 0;

    // 1) Création des corps
    Body sun(
        "Sun",
        vector2D(1.496e11, 1.496e11),
        vector2D(0.0, -1e3),
        1.989e30
    );

    Body earth(
        "Earth",
        vector2D(2*1.496e11, 1.496e11),
        vector2D(0.0, 29.78e3), //29.78e3
        5.972e24
    );

    Body moon(
        "Moon",
        vector2D(2*1.496e11 + 3.84e9, 1.496e11),
        vector2D(0.0, 29.78e3 + 1.022e3), //29.78e3 m/s
        7.36e22
    );

    std::vector<Body> bodies = { sun, earth, moon };
    std::vector<std::vector<vector2D>> trajectoire(bodies.size());

    // ——— Initialisation SDL2 ———
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    const int W = 800, H = 800;
    SDL_Window*   window   = SDL_CreateWindow(
        "Planet Simulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        W, H,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // ——— Boucle principale ———
    bool running = true;
    SDL_Event e;
    while (running) {
        // a) Événements
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        // b) Simulation physique (N sous-étapes par frame)
        for (int step = 0; step < N; ++step) {
            for (auto& b : bodies) b.resetForce();
            for (size_t i = 0; i < bodies.size(); ++i) {
                for (size_t j = i + 1; j < bodies.size(); ++j) {
                    bodies[i].addForce(bodies[j]);
                    bodies[j].addForce(bodies[i]);
                }
            }
            for (auto& b : bodies) 
                b.update(dt / N);
            for (size_t i =0; i<bodies.size(); ++i) trajectoire[i].push_back(bodies[i].position);
        }

        // c) Rendu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (const auto& b : bodies) {
            // conversion position → écran
            int x = static_cast<int>(W/2 + b.position.x * scale);
            int y = static_cast<int>(H/2 + b.position.y * scale);
            
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            for (size_t i=0; i<trajectoire.size(); ++i) {
                auto& traj =trajectoire[i];
                for (size_t j = 0; j < traj.size(); ++j) {
                    auto a = worldToScreen(traj[j],scale,H,W,offsetX,offsetY);
                    SDL_RenderDrawPoint(renderer, a.x, a.y);
                }
            }

            // couleur selon le corps
            if (b.nom == "Sun") {
                SDL_SetRenderDrawColor(renderer, 255, 255,   0, 255);
                double R_S = 6.96e9;
                int R_S_p = static_cast<int>(R_S * scale);
                drawFilledCircle(renderer, x, y, R_S_p);
            }
                
            if (b.nom == "Earth") {
                SDL_SetRenderDrawColor(renderer,   0, 0, 255, 255);
                double R_T = 4.5e9;//6.371e6;
                int R_T_p = static_cast<int>( R_T * scale);
                drawFilledCircle(renderer, x, y, R_T_p);

            }
                
            if (b.nom == "Moon") {
                SDL_SetRenderDrawColor(renderer,   200, 200, 200, 255);
                double R_L = 1e9;//1.731e6;
                int R_L_p = static_cast<int>(R_L * scale);
                drawFilledCircle(renderer, x, y, R_L_p);
            }
                

            
        }

        SDL_RenderPresent(renderer);
    }

    // ——— Cleanup ———
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


