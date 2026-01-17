#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include "vector2D.hpp"
#include "Body.hpp"
#include <string>

// Déclaration de la fonction
void handleCameraInput(Camera& camera, const Uint8* state);

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
    float cameraOffsetX,
    float cameraOffsetY)
{
// Décale par rapport à la caméra (dans le monde)
float fx = (pos.x - cameraOffsetX) * scale;
float fy = (pos.y - cameraOffsetY) * scale;

// Centre dans la fenêtre (et inverse Y pour SDL)
float sx = (W / 2.0f) + fx;
float sy = (H / 2.0f) - fy;

// Arrondi et cast en int
return SDL_Point{
static_cast<int>(std::round(sx)),
static_cast<int>(std::round(sy))
};
}

struct Camera {
    double offsetX = 0.0;
    double offsetY = 0.0;
    double zoom = 1.0;
};

int main() {
    // ——— Paramètres de simulation ———
    const int   N     = 100;
    const double dt    = 360;
    const float  scale = 3*1e-9f;

    Camera camera;

    // 1) Création des corps
    Body sun(
        "Sun",
        vector2D(0.0, 0.0),
        vector2D(0.0, 0.0),
        1.989e30
    );

    Body earth(
        "Earth",
        vector2D(1.496e11, 0.0),
        vector2D(0.0, 29.78e3), //29.78e3
        5.972e24
    );

    Body moon(
        "Moon",
        vector2D(1.496e11 , 3.84e9),
        vector2D(-1.022e3, 29.78e3), //29.78e3 m/s
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
    int i = 0;

    while (running) {

        const Uint8* state = SDL_GetKeyboardState(NULL);
        // a) Événements
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
            handleCameraInput(camera, state);
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
            for (size_t i =0; i<bodies.size(); ++i) {
                
                trajectoire[i].push_back(bodies[i].position);
                if (trajectoire.size() > 100) trajectoire[i].erase(trajectoire[i].begin());
            }
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
                    auto a = worldToScreen(traj[j],scale,H,W,camera.offsetX,camera.offsetY);
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

// Définition de la fonction
void handleCameraInput(Camera& camera, const Uint8* state)
{
    const double panSpeed = 100.0 / camera.zoom;   // adapté au zoom
    const double zoomFactor = 1.1;

    // Déplacement au clavier
    if (state[SDL_SCANCODE_LEFT])  camera.offsetX -= panSpeed;
    if (state[SDL_SCANCODE_RIGHT]) camera.offsetX += panSpeed;
    if (state[SDL_SCANCODE_UP])    camera.offsetY += panSpeed;
    if (state[SDL_SCANCODE_DOWN])  camera.offsetY -= panSpeed;

    // Zoom avec clavier (par exemple + et - du pavé principal)
    if (state[SDL_SCANCODE_EQUALS] || state[SDL_SCANCODE_KP_PLUS]) {  // touche + ou pavé +
        camera.zoom *= zoomFactor;
    }
    if (state[SDL_SCANCODE_MINUS] || state[SDL_SCANCODE_KP_MINUS]) {  // touche - ou pavé -
        camera.zoom /= zoomFactor;
    }

    // Optionnel : reset zoom avec R
    if (state[SDL_SCANCODE_R]) {
        camera.zoom = 1.0;
        camera.offsetX = 0.0;
        camera.offsetY = 0.0;
    }
}


