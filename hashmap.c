#include "utils.h"

int hashIt(char *key, int bucket_counter)
{
	return strlen(key) % bucket_counter;
}

int createHashmap(HashM map, int capacity)
{
	*map = malloc(sizeof(struct hashmap));
	if (*map == NULL)
		return -ENOMEM;

	(*map)->bucket_count = capacity;

	(*map)->buckets = calloc(capacity, sizeof(SymbolMapping));
	if ((*map)->buckets == NULL)
		return -ENOMEM;

	(*map)->size = 0;

	return 0;
}

int expand(Hashmap map)
{

	int i, new_index;
	SymbolM new_buckets;
	unsigned int new_bucket_count;

	new_bucket_count = map->bucket_count * 2;
	new_buckets = calloc(new_bucket_count, sizeof(SymbolMapping));

	if (new_buckets == NULL)
		return -ENOMEM;

	/* Move current entries into the new_buckets */
	for (i = 0; i < map->bucket_count; i++) {
		SymbolMapping bucket_entry = map->buckets[i];

		while (bucket_entry != NULL) {
			SymbolMapping next = bucket_entry->next;
			/* Calculate the new index (map size changed) */
			new_index = hashIt(bucket_entry->symbol,
				new_bucket_count);

			/*
			 * The current element will become the head
			 * of the list in the corresponding new bucket
			 */
			bucket_entry->next = new_buckets[new_index];
			new_buckets[new_index] = bucket_entry;

			bucket_entry = next;
		}
	}

	/* Update buckets in map */
	free(map->buckets);
	map->buckets = new_buckets;
	map->bucket_count = new_bucket_count;

	return 0;
}

int createElement(SymbolM elem, char *key, char *value)
{
	*elem = malloc(sizeof(struct symbol_mapping));
	if (*elem == NULL)
		return -ENOMEM;

	(*elem)->symbol = calloc(strlen(key) + 1, sizeof(char));
	if ((*elem)->symbol == NULL)
		return -ENOMEM;
	strcpy((*elem)->symbol, key);

	if (value != NULL) {
		(*elem)->mapping = calloc(strlen(value) + 1, sizeof(char));
		if ((*elem)->mapping == NULL)
			return -ENOMEM;
		strcpy((*elem)->mapping, value);
	} else {
		(*elem)->mapping = calloc(1, sizeof(char));
		if ((*elem)->mapping == NULL)
			return -ENOMEM;
	}

	(*elem)->next = NULL;

	return 0;
}

int addElement(Hashmap map, char *key, char *value)
{
	/*
	 * if the number of elements in the map exceeds the number of buckets
	 * -> resize it
	 */
	int r, index;
	SymbolMapping bucket_entry;
	SymbolMapping prev_bucket_entry;

	index = hashIt(key, map->bucket_count);
	bucket_entry = map->buckets[index];
	prev_bucket_entry = bucket_entry;

	if (bucket_entry == NULL) {
		r = createElement(&bucket_entry, key, value);
		if (r != 0)
			return r;
		map->buckets[index] = bucket_entry;
		map->size++;
	} else {
		while (prev_bucket_entry != NULL) {

			/* got to the end of bucket(element not found) -> add */
			if (bucket_entry == NULL) {
				r = createElement(&bucket_entry, key, value);
				if (r != 0)
					return r;

				prev_bucket_entry->next = bucket_entry;
				map->size++;
				if (map->size >= map->bucket_count) {
					r = expand(map);
					if (r != 0)
						return r;
				}
				return 0;
			}
			/* element found -> replace value */
			if (strcmp(key, bucket_entry->symbol) == 0) {
				free(bucket_entry->mapping);

				if (value != NULL) {
					bucket_entry->mapping =
						calloc(strlen(value) + 1,
						       sizeof(char));
					if (bucket_entry->mapping == NULL)
						return -ENOMEM;
					strcpy(bucket_entry->mapping, value);
				} else {
					bucket_entry->mapping = calloc(1,
						sizeof(char));
					if (bucket_entry->mapping == NULL)
						return -ENOMEM;
				}

				return 0;
			}

			prev_bucket_entry = bucket_entry;
			bucket_entry = bucket_entry->next;
		}
	}
	return 0;
}

void removeElement(Hashmap map, char *key)
{
	/* find the corresponding bucket index */
	int index;
	SymbolMapping bucket_entry, prev;

	index = hashIt(key, map->bucket_count);
	bucket_entry = map->buckets[index];
	prev = NULL;

	while (bucket_entry != NULL) {

		if (strcmp(bucket_entry->symbol, key) == 0) {
			if (prev == NULL)
				map->buckets[index] = bucket_entry->next;
			else
				prev->next = bucket_entry->next;

			free(bucket_entry->symbol);
			free(bucket_entry->mapping);
			free(bucket_entry);
			return;
		}
		prev = bucket_entry;
		bucket_entry = bucket_entry->next;
	}
}

SymbolMapping find(Hashmap map, char *key)
{
	/* find the corresponding bucket index */
	int index;
	SymbolMapping bucket_entry;

	index = hashIt(key, map->bucket_count);
	bucket_entry = map->buckets[index];

	while (bucket_entry != NULL) {
		if (strcmp(bucket_entry->symbol, key) == 0)
			return bucket_entry;
		bucket_entry = bucket_entry->next;
	}
	return NULL;
}

void freeHashmap(HashM map)
{
	int i;

	for (i = 0; i < (*map)->bucket_count; i++) {
		SymbolMapping bucket_entry = (*map)->buckets[i];

		while (bucket_entry != NULL) {
			SymbolMapping next = bucket_entry->next;

			free(bucket_entry->symbol);
			free(bucket_entry->mapping);
			free(bucket_entry);
			bucket_entry = next;
		}
	}
	free((*map)->buckets);
	free(*map);
}

void printHashmap(Hashmap map)
{
	int i;

	for (i = 0; i < map->bucket_count; i++) {
		SymbolMapping current = map->buckets[i];

		printf("\nindex %d: ", i);

		while (current != NULL) {
			printf("(key = %s, value = %s),",
			       current->symbol, current->mapping);
			current = current->next;
		}
	}
	printf("\n");
}
