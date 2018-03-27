#include "map.h"

#include <stdlib.h>
#include <stdio.h>

pMap create_map(void)
{
    pMap res = malloc(sizeof(Map));
    if (res == NULL)
    {
        perror("create_map: malloc failed");
        return NULL;
    }
    res->count = 0;
    res->keys = NULL;
    res->values = NULL;

    return res;
}
void free_map(pMap map)
{
    free(map->keys);
    free(map->values);
}

int map_set(pMap map, int key, void *value)
{
    for (int i = 0; i < map->count; i++)
    {
        if (map->keys[i] == key)
        {
            map->values[i] = value;
            return MAP_EXISTS;
        }
    }
    int count = map->count + 1;
    int *keys = realloc(map->keys, count * sizeof(int));
    if (keys == NULL)
    {
        perror("map_set: realloc failed");
        return MAP_FAILED;
    }

    void **values = realloc(map->values, count * sizeof(void*));
    if (values == NULL)
    {
        perror("map_set: realloc failed");
        return MAP_FAILED;
    }
    keys[map->count] = key;
    values[map->count] = value;

    map->count = count;
    map->keys = keys;
    map->values = values;
    return MAP_SUCCESS;
}

int map_get(pMap map, int key, void **value)
{
    for (int i = 0; i < map->count; i++)
    {
        if (map->keys[i] == key)
        {
            *value = map->values[i];
            return MAP_SUCCESS;
        }
    }
    return MAP_FAILED;
}
