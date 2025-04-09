#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define RECT_WIDTH 50
#define RECT_HEIGHT 50

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} Color;

int rand_range(int from, int to) {
    return from + rand() % (to - from);
}

Color random_color() {
    return (Color) {
        .r = rand_range(75, 255),
        .g = rand_range(75, 255),
        .b = rand_range(75, 255),
        .a = 255
    };
}

int main(int argc, char** argv) {
    srand(time(0));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Couldn't create a window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Couldn't create a renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    bool quit = false;

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    float x = 0.0f;
    float y = 0.0f;
    float dx = 1.0f;
    float dy = 1.0f;

    Color color = { 255, 0, 0, 255 };

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        last = now;
        now = SDL_GetPerformanceCounter();
        double dt = ((now - last) * 1000 / (double) SDL_GetPerformanceFrequency());

        x += 0.25f * dx * dt;
        y += 0.25f * dy * dt;

        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        if (x <= 0) {
            x = 0;
            dx = 1.0f;
            color = random_color();
        }

        if (y <= 0) {
            y = 0;
            dy = 1.0f;
            color = random_color();
        }

        if (x + RECT_WIDTH >= width) {
            x = width - RECT_WIDTH;
            dx = -1.0f;
            color = random_color();
        }

        if (y + RECT_HEIGHT >= height) {
            y = height - RECT_HEIGHT;
            dy = -1.0f;
            color = random_color();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
            SDL_FRect rect = {
                .x = x,
                .y = y,
                .w = RECT_WIDTH,
                .h = RECT_HEIGHT
            };
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRectF(renderer, &rect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}