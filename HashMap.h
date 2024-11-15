#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <stdlib.h>
#include <stdbool.h>

typedef enum { INT, STRING } ValueType;

typedef struct {
    ValueType type;
    union {
        int intValue;
        char *strValue;
    };
} Value;

typedef struct _MapEntry {
    int key;
    Value value;  // Usa Value directamente en lugar de void*
    struct _MapEntry *next;
} MapEntry;


MapEntry *map_entry_init(int key, Value value);
void map_entry_free(MapEntry *entry);


typedef struct {
    MapEntry **entries;
    size_t count;
    size_t size;
} HashMap;

HashMap *hash_map_init(size_t map_size);
void hash_map_free(HashMap *map);
void hash_map_insert(HashMap *map, int key, Value value);
bool hash_map_get(HashMap *map, int key, Value *value);
Value *hash_map_delete(HashMap *map, int key);
void serialize_to_file(HashMap *map, const char *filename);
void deserialize_from_file(HashMap *map, const char *filename);

#endif
