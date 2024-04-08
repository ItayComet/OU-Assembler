#include "machine_utils.h"
#include "hash_table.h"
/*the first pass goes over each non-empty and non-comment line:
	first it checks if there is a tag, if there is a tag and if then it checks its syntax
	then it checks if it's an instruction line or a directive line:
		for directive lines we save all of the data in the data code
		for instruction lines we check the syntax and save the first word and a place holder for the number of words the line needs, we'll fill the rest of the words in the second pass
	it also counts the entries declarations so we can allocate an exact size array for them
	@param fp: a pointer to the stream of the '.am' file
	@param instruction_code: an array of machine words, the words will be coded from the instruction lines
	@param data_code: an array of machine words, the words will be coded from the directive lines
	@param tags_table: an hash table to hold the tags and their addresses in
	@param instruction_counter: a pointer to an int which represents how many instructions words were coded in
	@param data_counter: a pointer to an int which represents how many data words were coded in
	@param entries_num: a pointer to an int which represents how many entries were declared
*/
int first_pass(FILE* fp, machine_word **instruction_code, machine_word **data_code, hash_table *tags_table, int *instruction_counter, int *data_counter, int *entries_num);
