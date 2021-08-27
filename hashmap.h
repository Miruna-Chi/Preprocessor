#ifndef SO_CPP_HASHMAP_H
#define SO_CPP_HASHMAP_H

typedef struct symbol_mapping {
	char *symbol;
	char *mapping;
	struct symbol_mapping *next;
} *SymbolMapping, **SymbolM;

typedef struct hashmap {
	SymbolM buckets;
	unsigned int bucket_count;
	unsigned int size;
} *Hashmap, **HashM;

int hashIt(char *key, int map_size);

int createHashmap(HashM map, int capacity);

int expand(Hashmap map);

int createElement(SymbolM elem, char *key, char *value);

int addElement(Hashmap map, char *key, char *value);

void removeElement(Hashmap map, char *key);

SymbolMapping find(Hashmap map, char *key);

void freeHashmap(HashM map);

void printHashmap(Hashmap map);

#endif
