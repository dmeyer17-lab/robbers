/*

    Author: Devan Meyer
    4/23/2025

*/

#ifndef VECTOR_H
#define VECTOR_H

typedef int *Vector;
typedef int **Matrix;

Vector vector_clone(Vector v, int size);

int vector_less_or_equal(Vector a, Vector b, int size);

void vector_add(Vector a, Vector b, int size);
void vector_print(Vector v, int size);
void matrix_print(Matrix m, int rows, int cols);

#endif