#ifndef LINE_H/*inclusion guard*/
#define LINE_H
#include "machine_utils.h"
/*lots of misc functions related to strings and lines to aid in the project*/
#define LINE_LENGTH_LIMIT 80
/*My implementation of the getline function from an extension to standard library
It recieves a pointer to a char pointer (which has to be allocated, to any size, at start), if the buffer is to small it reallocates the char pointer. it then fills the line until it gets to a newline char or EOF
it returns the number of characters in the line an 0 if there are none (only happens when EOF is reached because the newline char is counted as a character)
	@param fp: a pointer to the stream
	@param buff_ptr: a pointer of an char array pointer, has to be allocated with malloc before
	@param buff_size: a pointer to an int which holds the allocated size of *buff_ptr
*/
int getline(FILE *fp, char** buff_ptr, int *buff_size);

/*My version of strtok, it gets a string of a line and returns an allocated array of strings.
Each string in the array is word in the line, or comma, which counts as a word here
Also, if there are apostrophes it saves everything between them, including the apostrophes, in one string
	@param lineStr: the string we want to tokenize
	@param num_of_words: a pointer to an integer which will hold the number of strings in the returned array
*/
char** tokenize_line(char* lineStr, int* num_of_words);

/*frees an array of strings which was made with the tokenize_line() function
	@param line: the array of strings
	@param num_of_words: the length of it
*/
void free_tokenized_line(char** line, int num_of_words);

/*This function recieves two strings and returns a new allocated string which is them joined up, it doesn't change the original strings, the name of the function and the parameters is because I used it for file names
	@param name: the first string
	@param extension: the second string
*/
char* add_extension(char* name, char* extension);

/*This function recieves two strings, and reallocates first and joins the second with it, it then returns the new pointer to the string
	@param str: the string to reallocated
	@param extension: the string that will be added to str
*/
char* realloc_strcat(char* str, char* extension);

/*returns a pointer to an allocated copy of @param str*/
char* strdup(char* str);

/*returns non-zero value if the string it gets is a saved word in the assembly langauage*/
int is_saved_word(char* word);

/*returns non-zero value if the string it recieves has correct tag syntax
	note: if the string ends with ':' it will return 0, as a tag can't have a ':' char in it
*/
int has_correct_tag_synt(char* tag);

/* returns non-zero value if the string has a correct struct call syntax*/
int is_strct_adr(char* str);

/*returns non-zero value if the string is a name of a register (not including PSW)*/
int is_register(char* str);

/*returns a non-zero value if the string starts with a '#' char*/
int is_imm_adr(char* str);

/*returns a non-zero value if the string is an integer*/
int is_integer(char* str);

/*get a string which represents an integer and returns the integer as an int type*/
int str_to_int(char* str);

/*returns a non-zero value if the string starts and ends with a quote mark*/
int has_quotes(char* str);

/*returns a non zero value if the string starts with '.' char*/
int is_directive(char* word);

/*returns a non zero value if the string ends with colons, also removes them from the string*/
int ends_with_colon(char* str);

/*returns a non zero value if the string contains only a ',' char*/
int is_comma(char* str);

/*returns a non zero value if the string is a valid instruction*/
int is_instruction(char* word);

/*returns the which directive type is the string, if it isn't a directive it returns DIR_ERROR*/
directive_type get_dir_type(char* word);

/*returns the which instruction type is the string, if it isn't a instruction it returns INST_ERROR*/
instruction_type get_instruction_type(char* word);

/*returns the which addressing type is the string, if it isn't a addressing it returns ADR_ERROR*/
addressing_type get_adr_type(char* str);

#endif
