/* Universidad de La Frontera
* Ingeniería Civil Electrónica/Telemática
*
* IIE-344 ESTRUCTURA DE DATOS Y ALGORITMOS
*
* Tarea 2
*
* Autor 1: Paloma Ponce (p.ponce03@ufromail.cl)
* Autor 2: Sebastian Mayorga (s.mayorga01@ufromail.cl)
* Autor 3: Sergio Oyarce (s.oyarce01@ufromail.cl)
*
* Fecha: 14-11-2024
*
* Descripción general del programa:
* Este código implementa una estructura de datos HashMap en C,
  un tipo de contenedor que permite almacenar y recuperar pares de clave-valor
  de manera eficiente. Los elementos se agrupan en "slots" (casillas) calculados
  mediante una función hash para dispersar las claves de manera uniforme a través
  del mapa, lo que permite un acceso rápido incluso con grandes volúmenes de datos.

*/
#include "HashMap.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * Nombre de la función: map_entry_init
 * Tipo de función: MapEntry *
 * Parámetros:
     - "key" de tipo int.
     - "value" de tipo Value.
 * Dato de retorno: puntero a MapEntry.
 * Descripción de la función: Crea una nueva entrada en el HashMap asignando
  una clave y un valor, y retornando un puntero a la nueva entrada.
 */
MapEntry *map_entry_init(int key, Value value) {
  MapEntry *entry = malloc(sizeof(MapEntry));
  entry->key = key;
  entry->value = value;
  entry->next = NULL;
  return entry;
}
/**
 * Nombre de la función: map_entry_free
 * Tipo de función: void
 * Parámetros:
     - "entry" de tipo MapEntry *.
 * Descripción de la función: Libera la memoria de una entrada
  y de todas las entradas enlazadas en la lista.
 */
void map_entry_free(MapEntry *entry) {
  while (entry && entry->next) {
    map_entry_free(entry->next);
  }
  free(entry);
}
/**
 * Nombre de la función: hash_map_init
 * Tipo de función: HashMap *
 * Parámetros:
     - "map_size" de tipo size_t.
 * Dato de retorno: puntero a HashMap.
 * Descripción de la función: Inicializa un HashMap con un tamaño
  especificado y devuelve un puntero al mapa creado.
 */
HashMap *hash_map_init(size_t map_size) {
  HashMap *map = malloc(sizeof(HashMap));
  map->count = 0;
  map->size = map_size;
  map->entries = malloc(sizeof(MapEntry *) * map_size);
  for (size_t i = 0; i < map_size; ++i) {
    map->entries[i] = NULL;
  }
  return map;
}
/**
 * Nombre de la función: hash_map_free
 * Tipo de función: void
 * Parámetros:
     - "map" de tipo HashMap *.
 * Descripción de la función: Libera toda la memoria asociada
  al HashMap y sus entradas.
 */
void hash_map_free(HashMap *map) {
  if (!map) return;
  for (size_t i = 0; i < map->size; ++i) {
    if (map->entries[i]) {
      map_entry_free(map->entries[i]);
      map->entries[i] = NULL;
    }
  }
  free(map->entries);
  free(map);
}
/**
 * Nombre de la función: hash_function
 * Tipo de función: size_t
 * Parámetros:
     - "map" de tipo HashMap *.
     - "key" de tipo int.
 * Dato de retorno: índice (size_t) en el HashMap.
 * Descripción de la función: Calcula el índice correspondiente
  para una clave usando el tamaño del HashMap.
 */
size_t hash_function(HashMap *map, int key) {
  return key % map->size;
}
/**
 * Nombre de la función: hash_map_resize
 * Tipo de función: void
 * Parámetros:
     - "map" de tipo HashMap *.
 * Descripción de la función: Duplica el tamaño del HashMap
   cuando el factor de carga excede el 70%.
 */
void hash_map_resize(HashMap *map) {
  if ((float)map->count / map->size < 0.7) return;

  size_t new_size = map->size * 2;
  MapEntry **new_entries = calloc(new_size, sizeof(MapEntry*));

  for (size_t i = 0; i < map->size; ++i) {
    MapEntry *entry = map->entries[i];
    while (entry) {
      size_t new_slot = entry->key % new_size;
      MapEntry *next_entry = entry->next;
      entry->next = new_entries[new_slot];
      new_entries[new_slot] = entry;
      entry = next_entry;
    }
  }
  free(map->entries);
  map->entries = new_entries;
  map->size = new_size;
}
/**
 * Nombre de la función: hash_map_insert
 * Tipo de función: void
 * Parámetros:
     - "map" de tipo HashMap *.
     - "key" de tipo int.
     - "value" de tipo Value.
 * Descripción de la función: Inserta una nueva entrada en
  el HashMap. Realiza el manejo de colisiones y redimensionamiento.
 */
void hash_map_insert(HashMap *map, int key, Value value) {
  MapEntry *entry = map_entry_init(key, value);
  size_t slot = hash_function(map, key);
  if (map->entries[slot] == NULL) {
    map->entries[slot] = entry;
  } else {
    MapEntry *iter = map->entries[slot];
    while (iter->next) {
      iter = iter->next;
    }
    iter->next = entry;
  }
  map->count += 1;
  hash_map_resize(map);
}
/**
 * Nombre de la función: hash_map_get
 * Tipo de función: bool
 * Parámetros:
     - "map" de tipo HashMap *.
     - "key" de tipo int.
     - "value" de tipo Value *.
 * Dato de retorno: bool, indica si se encontró la clave.
 * Descripción de la función: Busca una entrada en el HashMap.
   Si encuentra la clave, asigna el valor a "value" y retorna true.
 */
bool hash_map_get(HashMap *map, int key, Value *value) {
  size_t slot = hash_function(map, key);
  MapEntry *iter = map->entries[slot];
  while (iter) {
    if (iter->key == key) {
      *value = iter->value;
      return true;
    }
    iter = iter->next;
  }
  return false;
}
/**
 * Nombre de la función: hash_map_delete
 * Tipo de función: Value *
 * Parámetros:
     - "map" de tipo HashMap *.
     - "key" de tipo int.
 * Dato de retorno: puntero a Value, o NULL si no se encontró la clave.
 * Descripción de la función: Elimina una entrada del HashMap
   y retorna un puntero al valor eliminado, o NULL si no se encuentra.
 */
Value *hash_map_delete(HashMap *map, int key) {
  size_t slot = hash_function(map, key);
  MapEntry *iter = map->entries[slot];
  MapEntry *prv = NULL;
  while (iter) {
    if (iter->key == key) {
      if (!prv) {
        map->entries[slot] = iter->next;
      } else {
        prv->next = iter->next;
      }
      iter->next = NULL;
      Value *value = &iter->value;
      map_entry_free(iter);
      map->count -= 1;
      return value;
    }
    prv = iter;
    iter = iter->next;
  }
  return NULL;
}
/**
 * Nombre de la función: serialize_to_file
 * Tipo de función: void
 * Parámetros:
     - "map" de tipo HashMap *.
     - "filename" de tipo const char *.
 * Descripción de la función: Guarda el contenido del HashMap en
   un archivo de texto, incluyendo el tamaño y cada entrada.
 */
void serialize_to_file(HashMap *map, const char *filename) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    perror("Error al abrir el archivo para escribir");
    return;
  }
  fprintf(file, "%zu\n", map->size);
  for (size_t i = 0; i < map->size; ++i) {
    MapEntry *entry = map->entries[i];
    while (entry) {
      // Índice | Clave | Valor
      if (entry->value.type == INT) {
        fprintf(file, "%zu|%d:%d\n", i, entry->key, entry->value.intValue);
      } else if (entry->value.type == STRING) {
        fprintf(file, "%zu|%d:\"%s\"\n", i, entry->key, entry->value.strValue);
      }
      entry = entry->next;
    }
  }
  fclose(file);
}
/**
 * Nombre de la función: deserialize_from_file
 * Tipo de función: void
 * Parámetros:
     - "map" de tipo HashMap *.
     - "filename" de tipo const char *.
 * Descripción de la función: Reconstruye el HashMap a
   partir de un archivo serializado.
 */
void deserialize_from_file(HashMap *map, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error al abrir el archivo para leer");
    return;
  }
  size_t total_size;
  fscanf(file, "%zu\n", &total_size);
  size_t index;
  int key;
  char value[255];
  while (fscanf(file, "%zu|%d:%s", &index, &key, value) != EOF) {
    Value val;
    if (value[0] == '\"') {
      val.type = STRING;
      value[strlen(value) - 1] = '\0';
      val.strValue = strdup(value + 1);
    } else { // Si es un INT
      val.type = INT;
      val.intValue = atoi(value);
    }
    hash_map_insert(map, key, val);
  }
  fclose(file);
}
/**
 * Nombre de la función: print_map
 * Tipo de función: void
 * Parámetros:
     - "map" de tipo HashMap *.
     - "title" de tipo const char *.
 * Descripción de la función: Imprime el contenido del HashMap,
   mostrando cada entrada (clave y valor) en el índice correspondiente.
 */
void print_map(HashMap *map, const char *title) {
  printf("%s: (%ld/%ld):\n", title, map->count, map->size);
  for (size_t i = 0; i < map->size; ++i) {
    MapEntry *iter = map->entries[i];
    if (iter) {
      printf("\t[%ld]: ", i);
      while (iter) {
        printf("(%d, ", iter->key);
        if (iter->value.type == INT) {
          printf("%d", iter->value.intValue);
        } else if (iter->value.type == STRING) {
          printf("\"%s\"", iter->value.strValue);
        }
        printf(") ");
        iter = iter->next;
      }
      printf("\n");
    }
  }
}

int main() {
  HashMap *map = hash_map_init(100);

  Value val1;
  val1.type = STRING;
  val1.strValue = "example_value";
  hash_map_insert(map, 7, val1);

  Value val2;
  val2.type = INT;
  val2.intValue = 9;
  hash_map_insert(map, 5, val2);

  Value val3;
  val3.type = INT;
  val3.intValue = 78;
  hash_map_insert(map, 305, val3);

  serialize_to_file(map, "map_data.txt");

  HashMap *new_map = hash_map_init(100);
  deserialize_from_file(new_map, "mapa.txt");

  print_map(new_map, "Mapa deserializado");

  hash_map_free(map);
  hash_map_free(new_map);

  return 0;
}