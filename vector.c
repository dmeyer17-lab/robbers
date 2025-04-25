/*

    Author: Devan Meyer
    4/23/2025

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

Vector vector_clone(Vector v, int size)
{
    Vector new_v = malloc(size * sizeof(int));
    memcpy(new_v, v, size * sizeof(int));
    return new_v;
}

int vector_less_or_equal(Vector a, Vector b, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (a[i] > b[i])
            return 0;
    }
    return 1;
}

void vector_add(Vector a, Vector b, int size)
{
    for (int i = 0; i < size; i++)
    {
        a[i] += b[i];
    }
}

void vector_print(Vector v, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", v[i]);
    }
    printf("\n");
}

void matrix_print(Matrix m, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        vector_print(m[i], cols);
    }
}