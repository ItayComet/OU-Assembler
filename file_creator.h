#include "machine_utils.h"
#include "hash_table.h"
/*creates a file with the name @param name, and its extension is '.ob'.
the file is the instruction and data code translated to base 32 numeric system
the first line of the file is the length of the instruction code in memory cells and the length of the data code in memory length.
each line after the first starts with a memory cell number and the second number is the word in this memory cell
The first memory cell number is 100
	@param name: the name of the file without the extension
	@param instruction_code: the array that holds the instruction code words
	@param data_code: the array that holds the data code words
	@param ic: number of instruction code words
	@param dc: number of data code words 
*/
int create_ob_file(char* name, machine_word** instruction_code, machine_word** data_code, int ic, int dc);

/*creates an entry file for the object file, each line consists of the name of the entry and the number of the cell where the tag is declared (not as an entry, but the actual declaration)
	@param name: the of the file without the extension
	@param tags_tab: the hash table which holds all of the tags and their value
	@param entries_tab: an array of strings, each cell holds an entry name
	@param entries_num: number of entries in entries_tab
	@param ic: length of instruction code (the offset of the first address in the data code)
*/
int create_ent_file(char* name, hash_table* tags_tab, char** entries_tab, int entries_num, int ic);

/*creates an externs file for the object file, the string is constructed during the second pass, this function just creates the file and puts the string in it
	@param name: the name of the file without the extension
	@param ext_str: the string from the second pass
*/
int create_ext_file(char* name, char* ext_str);
