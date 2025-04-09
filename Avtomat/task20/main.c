#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

// Ширина квадрата
#define RECT_WIDTH 50
// Высота квадрата
#define RECT_HEIGHT 50

// Структура для хранения информации о цвете
typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} Color;

// Функция, генерирующая рандомное значение в каком-то диапазоне
int rand_range(int from, int to) {
    return from + rand() % (to - from);
}

// Функция, генерирующая рандомный цвет
Color random_color() {
    return (Color) {
        .r = rand_range(75, 255),
        .g = rand_range(75, 255),
        .b = rand_range(75, 255),
        .a = 255
    };
}

int main(int argc, char** argv) {
    // Инициализируем рандомайзер
    srand(time(0));

    // Инициализируем библиотеку SDL, если инициализация не удалась,
    // сообщаем об этом пользователю и завершаем программу с кодом ошибки
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Инициализируем окно, если инициализация не удалась,
    // сообщаем об этом пользователю и завершаем программу с кодом ошибки
    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Couldn't create a window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Инициализируем рендерер, если инициализация не удалась,
    // сообщаем об этом пользователю и завершаем программу с кодом ошибки
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Couldn't create a renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Текущее событие
    SDL_Event event;
    // Флаг означающий, что программа должна завершиться
    bool quit = false;

    // Текущее время
    Uint64 now = SDL_GetPerformanceCounter();
    // Предыдущее время
    Uint64 last = 0;

    // Координата x квадрата
    float x = 0.0f;
    // Координата y квадрата
    float y = 0.0f;
    // Направление квадрата по оси x
    float dx = 1.0f;
    // Направление квадрата по оси y
    float dy = 1.0f;

    // Цвет квадрата
    Color color = { 255, 0, 0, 255 };

    // Повторяем пока программа не должна завершиться
    while (!quit) {
        // Обрабатываем события
        while (SDL_PollEvent(&event)) {
            // Если пользователь хочет закрыть программу
            // устанавливаем флаг quit в true
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Обновляем предыдущее время
        last = now;
        // Получаем текущее время
        now = SDL_GetPerformanceCounter();
        // Вычисляем время кадра
        double dt = ((now - last) * 1000 / (double) SDL_GetPerformanceFrequency());

        // Обновляем координаты квадрата
        x += 0.25f * dx * dt;
        y += 0.25f * dy * dt;

        // Получаем размер окна
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        
        // Если координата x квадрата меньше 0 (левая сторона квадрата вышла за левую границу окна),
        // то меняем направление квадрата по оси x на противоположное и обновляем его цвет
        if (x <= 0) {
            x = 0;
            dx = 1.0f;
            color = random_color();
        }

        // Если координата y квадрата меньше 0 (верхняя сторона квадрата вышла за верхнюю границу окна),
        // то меняем направление квадрата по оси y на противоположное и обновляем его цвет
        if (y <= 0) {
            y = 0;
            dy = 1.0f;
            color = random_color();
        }

        // Если координата x квадрата + его ширина больше ширины окна
        // (правая сторона квадрата вышла за правую границу окна),
        // то меняем направление квадрата по оси x на противоположное и обновляем его цвет
        if (x + RECT_WIDTH >= width) {
            x = width - RECT_WIDTH;
            dx = -1.0f;
            color = random_color();
        }

        // Если координата y квадрата + его высота больше высоты окна
        // (нижняя сторона квадрата вышла за нижнюю границу окна),
        // то меняем направление квадрата по оси y на противоположное и обновляем его цвет
        if (y + RECT_HEIGHT >= height) {
            y = height - RECT_HEIGHT;
            dy = -1.0f;
            color = random_color();
        }

        // Ставим цвет очистки окна на черный
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        // Очищаем окно
        SDL_RenderClear(renderer);
            // Инициализируем структуру с позиицией и размером квадрата
            SDL_FRect rect = {
                .x = x,
                .y = y,
                .w = RECT_WIDTH,
                .h = RECT_HEIGHT
            };
            // Устанавливаем цвет квадрата
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            // Рисуем квадрат
            SDL_RenderFillRectF(renderer, &rect);
        SDL_RenderPresent(renderer);
    }

    // Уничтожаем рендерер (освобождаем ресурсы)
    SDL_DestroyRenderer(renderer);
    // Уничтожаем окно (освобождаем ресурсы)
    SDL_DestroyWindow(window);
    // Освобождаем оставшиеся ресурсы
    SDL_Quit();

    // Возвращаем нулевое значение, сообщая, что программа завершилась без ошибок
    return 0;
}