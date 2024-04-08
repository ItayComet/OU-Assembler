/*an hashtable structure and its functions. It handles collisions with a linked list, it can handle only positive values*/
#ifndef HASH_TABLE_H/*inclusion guard*/
#define HASH_TABLE_H
#define HASH_SIZE 128 /*making it bigger would be a memory waste, since I doubt the number of tags can get much bigger than a 1000, and if it does, the search and add functions would just become linear*/


typedef enum tag_signs{
	SIGN_CODE, SIGN_DATA, SIGN_STRCT, SIGN_EXTERN, SIGN_ENTRY, SIGN_NO_KEY = -1
} tag_sign;
typedef struct hash_item{
	char* key;
	int val;
	tag_sign sign;
	struct hash_item* next;
} hash_item;


typedef struct hash_table{
	hash_item** items;
	int size;
} hash_table;

/*creates a new hash_table and returns it, returns NULL if the allocation failed
	@param size: the size of the table
*/
hash_table* new_hash_table(int size);

/*frees @param tab, and all of it's items*/
void free_hash_table(hash_table* tab);

/*adds a new item to the hash_table, returns -1 for an allocation failure
	@param tab: hash_table to add the item to
	@param key: the key of the item
	@param val: the value of the item
*/
int hash_add(hash_table* tab, char* key, int val, tag_sign sign);

/*returns the value of the key if it exists in the hash_table, and -1 if it isn't
	@param tab: the table
	@param key: the key to search
*/
int hash_lookup_val(hash_table* tab, char* key);

/*returns the sign of the key if it exists in the hash_table, and 0> value if it isn't
	@param tab: the table
	@param key: the key to search
*/
tag_sign hash_lookup_sign(hash_table* tab, char* key);

/*if the key exists in the table, this function changes its sign field
	@param tab: the has_table
	@param key: the key to change
	@param new_sign: the new sign
*/
void hash_change_sign(hash_table* tab, char* key, tag_sign new_sign);

#endif
