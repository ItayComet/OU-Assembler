/*The second pass does a few things which will be listed here, but its main purpose is to fill the empty words we addedd to the instruction_code in the first pass
It also fills the entries table and creates a string for externals (each line in the string consists of the name of the tag and where it was used)
	@param fp: a pointer to the '.am' file
	@param instruction_code: the array of the instruction code words from the first pass
	@param tags_table: the hash table which holds the tags (the one which was filled in the first pass)
	@param d_addr_offset: the offset of each address of a data code word is the length of the instruction code
	@param entries_tab: an allocated table of strings (since the number of entries was counted in the first pass, the array can be allocated to hold the exact number of entries)
	@param ext_str: a pointer to the externals string, since it can be reallocated

returns 0 on success, -1 for an allocation error, -2 for a syntax error
*/
int second_pass(FILE* fp, machine_word** instruction_code, hash_table* tags_table, int d_addr_offset, char** entries_tab, char** ext_str);
