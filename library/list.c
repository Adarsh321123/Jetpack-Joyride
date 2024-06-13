#include "list.h"
#include "constants.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

list_t *list_init(size_t initial_capacity, free_func_t freer)
{
  list_t *v_list = malloc(sizeof(list_t));
  assert(v_list != NULL);
  v_list->length = initial_capacity;
  v_list->size = 0;
  v_list->elements = malloc(initial_capacity * sizeof(void *));
  assert(v_list->elements != NULL);
  for (size_t i = 0; i < initial_capacity; i++)
  {
    v_list->elements[i] = NULL;
  }
  v_list->freer = freer;
  return v_list;
}

void list_free(list_t *list)
{
  if (list == 0 || list == NULL)
  {
    return;
  }
  assert(list->elements != NULL);
  for (size_t i = 0; i < list->size; i++)
  {
    if (list->elements[i] != NULL)
    {
      if (list->freer != NULL)
      {
        list->freer(list->elements[i]);
      }
    }
  }
  free(list->elements);
  free(list);
}

size_t list_size(list_t *list)
{
  assert(list != NULL);
  return list->size;
}

void *list_get(list_t *list, size_t index)
{
  assert(index < list_size(list));
  return list->elements[index];
}

void list_add(list_t *list, void *value)
{
  assert(value != NULL);
  list->size = list->size + 1;
  size_t curr_size = list_size(list);
  if (curr_size > list->length)
  {
    void **temparr = malloc(list->size * sizeof(void *));
    assert(temparr != NULL);
    for (size_t i = 0; i < list->size - 1; i++)
    {
      temparr[i] = list->elements[i];
    }
    free(list->elements);
    list->length = SCALE * (list->length);
    list->elements = malloc(list->length * sizeof(void *));
    assert(list->elements != NULL);
    for (size_t i = 0; i < list->length; i++)
    {
      list->elements[i] = NULL;
    }
    for (size_t i = 0; i < list->size - 1; i++)
    {
      list->elements[i] = temparr[i];
    }
    free(temparr);
    assert(list->size <= list->length);
  }
  list->elements[list->size - 1] = value;
}

void *list_remove(list_t *list, size_t index)
{
  assert(index < list->size);
  void *e_end = list->elements[index];
  list->elements[index] = NULL;
  for (size_t i = index + 1; i < list->size; i++)
  {
    void *el = list->elements[i];
    list->elements[i] = NULL;
    list->elements[i - 1] = el;
  }
  list->size = list->size - 1;
  return e_end;
}
