#include "map.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

pMap create_map(size_t value_size)
{
    pMap res = malloc(sizeof(Map));
    if (res == NULL)
    {
        perror("create_map: malloc failed");
        return NULL;
    }
    res->count = 0;
    res->value_size = value_size;
    res->keys = NULL;
    res->values = NULL;

    return res;
}
void free_map(pMap map)
{
    free(map->keys);
    free(map->values);
}

int map_set(pMap map, size_t key, void *value)
{
    for (int i = 0; i < map->count; i++)
    {
        if (map->keys[i] == key)
        {
            memcpy(map->values + i * map->value_size, value, map->value_size);
            return MAP_EXISTS;
        }
    }
    size_t index = map->count;
    size_t count = index + 1;
    size_t *keys = realloc(map->keys, count * sizeof(size_t));
    if (keys == NULL)
    {
        perror("map_set: realloc failed");
        return MAP_FAILED;
    }

    void *values = realloc(map->values, count * map->value_size);
    if (values == NULL)
    {
        perror("map_set: realloc failed");
        return MAP_FAILED;
    }
    keys[index] = key;
    memcpy(values + index * map->value_size, value, map->value_size);

    map->count = count;
    map->keys = keys;
    map->values = values;
    return MAP_SUCCESS;
}

int map_get(pMap map, size_t key, void *value)
{
    for (int i = 0; i < map->count; i++)
    {
        if (map->keys[i] == key)
        {
            memcpy(value, map->values + i * map->value_size, map->value_size);
            return MAP_SUCCESS;
        }
    }
    return MAP_FAILED;
}
