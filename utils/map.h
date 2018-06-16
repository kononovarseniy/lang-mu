#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <stdlib.h>

#define MAP_FAILED (-1)
#define MAP_SUCCESS 0
#define MAP_EXISTS 1

typedef struct Map
{
    size_t count;
    size_t *keys;
    size_t value_size;
    void *values;
} Map, *pMap;

pMap create_map(size_t value_size);
void free_map(pMap map);

int map_set(pMap map, size_t key, void *value);
int map_get(pMap map, size_t key, void *value);
size_t *map_get_keys(pMap map);
void *map_get_values(pMap map);

#endif // MAP_H_INCLUDED
