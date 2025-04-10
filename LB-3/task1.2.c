#include <stdio.h>
#include <math.h>

typedef struct {
    float x, y, z;
    char* name;
} Vector;

Vector vector_mul(Vector a, Vector b) {
    return (Vector) {
        .x = a.x * b.x,
        .y = a.y * b.y,
        .z = a.z * b.z
    };
};

float vector_dot(Vector a, Vector b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
};

// https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
Vector vector_cross(Vector a, Vector b) {
    return (Vector) {
        .x = a.y * b.z - b.y * a.z,
        .y = a.z * b.x - b.z * a.x,
        .z = a.x * b.y - b.x * a.y,
        .name = NULL
    };
};

Vector vector_abs(Vector v) {
    return (Vector) {
        .x = fabsf(v.x),
        .y = fabsf(v.y),
        .z = fabsf(v.z),
        .name = NULL
    };
}

void vector_print(Vector v) {
    printf("{%.2f, %.2f, %.2f}\n", v.x, v.y, v.z);
}

int main() {
    Vector a = {
        .x = 0,
        .y = 10,
        .z = 3,
        .name = "a"
    };
    
    Vector b = {
        .x = -10,
        .y = 5,
        .z = -3,
        .name = "b"
    };

    Vector cross = vector_cross(a, b);
    float dot = vector_dot(a, b);
    Vector a_abs = vector_abs(a);
    Vector b_abs = vector_abs(b);

    printf("a = ");
    vector_print(a);

    printf("b = ");
    vector_print(b);

    printf("a x b = ");
    vector_print(cross);

    printf("a * b = %f\n", dot);

    printf("|a| = ");
    vector_print(a_abs);

    printf("|b| = ");
    vector_print(b_abs);

    return 0;
}