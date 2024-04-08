#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash_table.h"
#include "line_utils.h"

/*the hash function, hopefully returns very different results for different keys
	@param str: the key
	@param size: the size of the table
*/
int hash_function(char* str, int size) {
    int product = 0;
    int i;
    for (i = 0; str[i] != '\0'; i++)
        product += str[i]*(i+1);
    return product % size;
}

/*creates a new hash_item for the table
	@param key: the key of the item
	@param val: the value of the item
*/
hash_item* new_hash_item(char* key, int val, tag_sign sign){
	hash_item* item;
	item = malloc(sizeof(hash_item));
	if(item == NULL)
		return NULL;
	item->key = strdup(key);
	if(item->key == NULL){
		free(item);
		return NULL;
	}
	item->val = val;
	item->next = NULL;
	item->sign = sign;
	return item;
}

/*getters and setters*/
void set_next_item(hash_item* curr, hash_item* next){
	curr->next = next;
}
hash_item* get_next_item(hash_item* item){
	return item->next;
}

void free_hash_item(hash_item* item){/*frees the list in which item is the head*/
	hash_item *next_item, *curr_item;
	curr_item = item;
	while((next_item = curr_item->next) != NULL){
		free(curr_item);
		curr_item = next_item;
	}
	free(curr_item);
}

hash_table* new_hash_table(int size){
	hash_table* tab;
	tab = malloc(sizeof(hash_table));
	if(tab == NULL)
		return NULL;
	tab->size = size;
	tab->items = calloc(size,sizeof(hash_item*));
	if(tab->items == NULL){
		free(tab);
		return NULL;
	}
	return tab;
}

void free_hash_table(hash_table* tab){
	int i;
	for(i = 0; i < tab->size; i++){
		if(tab->items[i] != NULL){
			free_hash_item(tab->items[i]);
		}
	}
	free(tab->items);
	free(tab);
}

int hash_add(hash_table* tab, char* key, int val, tag_sign sign){
	hash_item *new_item;
	int hash_result;
	
	new_item = new_hash_item(key,val,sign);
	if(new_item == NULL)
		return -1;/*allocation error*/
	
	hash_result = hash_function(key, tab->size);
	if(tab->items[hash_result] != NULL)
		set_next_item(new_item, tab->items[hash_result]);
	tab->items[hash_result] = new_item;
	return 0;
	
}

hash_item* hash_lookup(hash_table* tab, char* key){
	int hash_result;
	hash_item* curr_item;
	
	hash_result = hash_function(key, tab->size);
	curr_item = tab->items[hash_result];
	while(curr_item != NULL){
		if(strcmp(curr_item->key, key) == 0)
			return curr_item;
		curr_item = get_next_item(curr_item);
	}
	return NULL;/*none found*/
}

int hash_lookup_val(hash_table* tab, char* key){
	hash_item* item = hash_lookup(tab, key);
	if(item == NULL)
		return -1;
	return item->val;
}

tag_sign hash_lookup_sign(hash_table* tab, char* key){
	hash_item* item = hash_lookup(tab, key);
	if(item == NULL)
		return SIGN_NO_KEY;
	return item->sign;
}

void hash_change_sign(hash_table* tab, char* key, tag_sign new_sign){
	hash_item* item = hash_lookup(tab, key);
	if(item != NULL)
		item->sign = new_sign;
}
