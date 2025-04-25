/*

    Author: Devan Meyer
    4/23/2025

*/

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "banker.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <scenario file>\n", argv[0]);
    return 1;
  }

  SystemState *state = read_state(argv[1]);
  if (!state)
  {
    fprintf(stderr, "Error: File not found or invalid format.\n");
    return 1;
  }

  Matrix need = malloc(state->nproc * sizeof(Vector));
  for (int i = 0; i < state->nproc; i++)
  {
    need[i] = malloc(state->nres * sizeof(int));
  }
  if (!compute_need(state, need))
  {
    for (int i = 0; i < state->nproc; i++)
    {
      for (int j = 0; j < state->nres; j++)
      {
        if (state->allocation[i][j] > state->max[i][j])
        {
          fprintf(stderr, "Integrity test failed: allocated resources exceed demand for Thread %d\n", i);
          fprintf(stderr, "Need %d instances of resource %d\n", state->max[i][j] - state->allocation[i][j], j);
          goto cleanup;
        }
      }
    }
  }

  Vector available = malloc(state->nres * sizeof(int));
  if (!compute_available(state, available))
  {
    fprintf(stderr, "Integrity test failed: allocated resources exceed total resources\n");
    goto cleanup;
  }

  int sanity = sanity_checks(state);
  if (sanity != 0)
  {
    if (sanity == -1)
    {
      fprintf(stderr, "Integrity test failed: allocated resources exceed total resources\n");
    }
    else
    {
      int thread = sanity - 1;
      fprintf(stderr, "Integrity test failed: allocated resources exceed demand for Thread %d\n", thread);
      for (int j = 0; j < state->nres; j++)
      {
        if (state->allocation[thread][j] > state->max[thread][j])
        {
          fprintf(stderr, "Need %d instances of resource %d\n", state->max[thread][j] - state->allocation[thread][j], j);
        }
      }
    }
    goto cleanup;
  }

  Vector finish = NULL;
  int safe = is_safe(available, state->allocation, need, state->nres, state->nproc, &finish);

  if (safe)
  {
    SafeList *list = create_safe_list();
    find_all_safe(available, state->allocation, need, state->nres, state->nproc, list);
    print_safe(list, state->nproc);
    free_safe_list(list);
  }
  else
  {
    print_unsafe(finish, state->nproc);
  }

  if (finish)
    free(finish);

cleanup: // freeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
  free(available);
  for (int i = 0; i < state->nproc; i++)
    free(need[i]);
  free(need);
  free_sys_state(state);
  return 0;
}