#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    double x;
    double y;
} Vector;

double magnitude(Vector v);
Vector normalize(Vector v);
Vector subtract(Vector a, Vector b);
double dotProduct(Vector a, Vector b);
Vector multiply(Vector v, double scalar);
Vector add(Vector a, Vector b);

#endif // VECTOR_H
