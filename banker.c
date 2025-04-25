/*

    Author: Devan Meyer
    4/23/2025

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "banker.h"

SystemState *read_state(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return NULL;

    SystemState *state = malloc(sizeof(SystemState));
    if (fscanf(fp, "%d", &state->nres) != 1)
        goto error;
    if (fscanf(fp, "%d", &state->nproc) != 1)
        goto error;

    state->total = malloc(state->nres * sizeof(int));
    for (int i = 0; i < state->nres; i++)
    {
        if (fscanf(fp, "%d", &state->total[i]) != 1)
            goto error;
    }

    state->max = malloc(state->nproc * sizeof(Vector));
    for (int i = 0; i < state->nproc; i++)
    {
        state->max[i] = malloc(state->nres * sizeof(int));
        for (int j = 0; j < state->nres; j++)
        {
            if (fscanf(fp, "%d", &state->max[i][j]) != 1)
                goto error;
        }
    }

    state->allocation = malloc(state->nproc * sizeof(Vector));
    for (int i = 0; i < state->nproc; i++)
    {
        state->allocation[i] = malloc(state->nres * sizeof(int));
        for (int j = 0; j < state->nres; j++)
        {
            if (fscanf(fp, "%d", &state->allocation[i][j]) != 1)
                goto error;
        }
    }

    fclose(fp);
    return state;

error:
    if (state)
    {
        free(state->total);
        if (state->max)
        {
            for (int i = 0; i < state->nproc; i++)
                free(state->max[i]);
            free(state->max);
        }
        if (state->allocation)
        {
            for (int i = 0; i < state->nproc; i++)
                free(state->allocation[i]);
            free(state->allocation);
        }
        free(state);
    }
    fclose(fp);
    return NULL;
}

void free_sys_state(SystemState *state)
{
    if (state)
    {
        free(state->total);
        for (int i = 0; i < state->nproc; i++)
        {
            free(state->max[i]);
            free(state->allocation[i]);
        }
        free(state->max);
        free(state->allocation);
        free(state);
    }
}

int compute_available(SystemState *state, Vector available)
{
    Vector sum = calloc(state->nres, sizeof(int));
    for (int i = 0; i < state->nproc; i++)
    {
        vector_add(sum, state->allocation[i], state->nres);
    }
    int valid = 1;
    for (int r = 0; r < state->nres; r++)
    {
        available[r] = state->total[r] - sum[r];
        if (available[r] < 0)
            valid = 0;
    }
    free(sum);
    return valid;
}

int compute_need(SystemState *state, Matrix need)
{
    for (int i = 0; i < state->nproc; i++)
    {
        for (int j = 0; j < state->nres; j++)
        {
            need[i][j] = state->max[i][j] - state->allocation[i][j];
            if (need[i][j] < 0)
                return 0;
        }
    }
    return 1;
}

int sanity_checks(SystemState *state)
{
    Vector sum = calloc(state->nres, sizeof(int));
    for (int i = 0; i < state->nproc; i++)
    {
        vector_add(sum, state->allocation[i], state->nres);
    }
    for (int r = 0; r < state->nres; r++)
    {
        if (sum[r] > state->total[r])
        {
            free(sum);
            return -1;
        }
    }
    free(sum);

    for (int i = 0; i < state->nproc; i++)
    {
        for (int j = 0; j < state->nres; j++)
        {
            if (state->allocation[i][j] > state->max[i][j])
            {
                return i + 1;
            }
        }
    }
    return 0;
}

int is_safe(Vector available, Matrix allocation, Matrix need, int nres, int nproc, Vector *finish_result)
{
    Vector work = vector_clone(available, nres);
    Vector finish = calloc(nproc, sizeof(int));
    int found;

    do
    {
        found = 0;
        for (int i = 0; i < nproc; i++)
        {
            if (!finish[i] && vector_less_or_equal(need[i], work, nres))
            {
                vector_add(work, allocation[i], nres);
                finish[i] = 1;
                found = 1;
                break;
            }
        }
    } while (found);

    int safe = 1;
    for (int i = 0; i < nproc; i++)
    {
        if (!finish[i])
        {
            safe = 0;
            break;
        }
    }

    if (finish_result)
        *finish_result = finish;
    else
        free(finish);

    free(work);
    return safe;
}

void find_all_safe_recursive(Vector work, Vector finish, int *sequence, int step, int nres, int nproc, Matrix allocation, Matrix need, SafeList *list)
{
    if (step == nproc)
    {
        int *seq = malloc(nproc * sizeof(int));
        memcpy(seq, sequence, nproc * sizeof(int));
        if (list->count >= list->capacity)
        {
            list->capacity *= 2;
            list->sequences = realloc(list->sequences, list->capacity * sizeof(int *));
        }
        list->sequences[list->count++] = seq;
        return;
    }

    for (int i = 0; i < nproc; i++)
    {
        if (!finish[i] && vector_less_or_equal(need[i], work, nres))
        {
            Vector new_work = vector_clone(work, nres);
            Vector new_finish = vector_clone(finish, nproc);
            int *new_seq = malloc((step + 1) * sizeof(int));
            memcpy(new_seq, sequence, step * sizeof(int));
            new_seq[step] = i;

            vector_add(new_work, allocation[i], nres);
            new_finish[i] = 1;

            find_all_safe_recursive(new_work, new_finish, new_seq, step + 1, nres, nproc, allocation, need, list);

            free(new_work);
            free(new_finish);
            free(new_seq);
        }
    }
}

void find_all_safe(Vector available, Matrix allocation, Matrix need, int nres, int nproc, SafeList *list)
{
    Vector work = vector_clone(available, nres);
    Vector finish = calloc(nproc, sizeof(int));
    int *seq = malloc(0);

    find_all_safe_recursive(work, finish, seq, 0, nres, nproc, allocation, need, list);

    free(work);
    free(finish);
    free(seq);
}

SafeList *create_safe_list()
{
    SafeList *list = malloc(sizeof(SafeList));
    list->count = 0;
    list->capacity = 10;
    list->sequences = malloc(list->capacity * sizeof(int *));
    return list;
}

void free_safe_list(SafeList *list)
{
    for (int i = 0; i < list->count; i++)
        free(list->sequences[i]);
    free(list->sequences);
    free(list);
}

void print_safe(SafeList *list, int nproc)
{
    for (int i = 0; i < list->count; i++)
    {
        printf("SAFE:");
        for (int j = 0; j < nproc; j++)
            printf(" T%d", list->sequences[i][j]);
        printf("\n");
    }
}

void print_unsafe(Vector finish, int nproc)
{
    printf("UNSAFE:");
    for (int i = 0; i < nproc; i++)
    {
        if (!finish[i])
            printf(" T%d", i);
    }
    printf(" can't finish\n");
}