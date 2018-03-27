#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#define MAP_FAILED -1
#define MAP_SUCCESS 0
#define MAP_EXISTS 1

typedef struct Map
{
    int count;
    int *keys;
    void **values;
} Map, *pMap;

pMap create_map(void);
void free_map(pMap map);

int map_set(pMap map, int key, void *value);
int map_get(pMap map, int key, void **value);

#endif // MAP_H_INCLUDED
