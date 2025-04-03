#include <stdio.h>
#include <math.h>

int main() {
    float mars_r = 228.0f * 1000000.0f * 1000.0f;
    float earth_r = 149.0f * 1000000.0f * 1000.0f;

    float mars_t = 687.0f * 24.0f * 60.0f * 60.0f;
    float earth_t = 365.0f * 24.0f * 60.0f * 60.0f;

    float mars_w = 2.0f * M_PI / mars_t;
    float earth_w = 2.0f * M_PI / earth_t;

    FILE* output = fopen("output.txt", "w");

    for (float t = 0.0f; t < 1000.0f * 1000.0f * 1000.0f; t += 1000.0f) {
        float x = mars_r * cosf(mars_w * t) - earth_r * cosf(earth_w * t);
        float y = mars_r * sinf(mars_w * t) - earth_r * sinf(earth_w * t);

        fprintf(output, "%f, %f\n", x, y);
    }

    fclose(output);
    
    return 0;
}
