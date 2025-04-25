/*

    Author: Devan Meyer
    4/23/2025

    declarations related to banker's algorithm 
*/
#ifndef BANKER_H
#define BANKER_H

#include <stdio.h>

typedef struct
{
    int nres;
    int nproc;
    Vector total;
    Matrix max;
    Matrix allocation;
} SystemState;

typedef struct
{
    int **sequences;
    int count;
    int capacity;
} SafeList;

SystemState *read_state(const char *filename);
void free_sys_state(SystemState *state);

int compute_available(SystemState *state, Vector available);
int compute_need(SystemState *state, Matrix need);

int sanity_checks(SystemState *state);

int is_safe(Vector available, Matrix allocation, Matrix need, int nres, int nproc, Vector *finish_result);
void find_all_safe(Vector available, Matrix allocation, Matrix need, int nres, int nproc, SafeList *list);

SafeList *create_safe_list();
void free_safe_list(SafeList *list);
void print_safe(SafeList *list, int nproc);
void print_unsafe(Vector finish, int nproc);

#endif