#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "line_utils.h"
#include "macros.h"

void free_macros_table(struct macro** macro_t, int num_of_macros);
int add_macro_to_table(struct macro*** macro_t_pt, struct macro* mpt, int buff_len, int num_of_macros);
int search_macro(struct macro** macro_t, char* name, int num_of_macros);

/*returns -1 for memory allocation error, -2 in case of synatax error, also prints the synatx errors and their line numbers to the standard outpout*/
int pre_assemble(FILE* fp, char* name){
	char *line_buffer;
	int buffer_len, line_len;
	FILE *new_fp;
	char** tokenized_line;
	struct macro* newest_macro;
	struct macro** macros_table;
	int macros_table_buff, num_of_macros;
	char *new_name;
	int line_num = 0, macro_ind = 0, words_num;
	
	struct status{
		unsigned int syntaxErr : 1;
		unsigned int inMacro : 1;
	} status;
	status.syntaxErr = 0;
	status.inMacro = 0;
	
	/*create a new file with .am extension*/
	new_name = add_extension(name, ".am");
	new_fp = fopen(new_name, "w");
	printf("The file '%s' has been created\n", new_name);
	free(new_name);
	
	/*create buffer for the line*/
	buffer_len = 64;
	line_buffer = malloc(buffer_len*sizeof(char));
	if(line_buffer == NULL){
		fclose(new_fp);
		return -1;/*memory allocation failed*/
	}
	
	/*create a table of macros*/
	macros_table_buff = 8;
	num_of_macros = 0;
	macros_table = malloc(macros_table_buff * sizeof(struct macro*));
	if(macros_table == NULL){
		fclose(new_fp);
		free(line_buffer);
		return -1;/*memory allocation failed*/
	}
	
	
	
	while((line_len = getline(fp, &line_buffer, &buffer_len)) > 0){
		line_num++;
		tokenized_line = tokenize_line(line_buffer, &words_num);
		if(tokenized_line == NULL){
			free(line_buffer);
			free_macros_table(macros_table, num_of_macros);
			fclose(new_fp);
			return -1;
		}
		if(words_num == 0){/*empty line, we won't print it to the file as it has no use in any part of the assembler*/
			free_tokenized_line(tokenized_line, words_num);
			continue;
		}
		
		if(status.inMacro){
			if(strcmp(tokenized_line[0], "endmacro") == 0)/*macro ends*/
				status.inMacro = 0;
			else if(add_macro_line(newest_macro, line_buffer) < 0){/*free all, memory allocation problem*/
					free_macros_table(macros_table, num_of_macros);
					free(line_buffer);
					free_tokenized_line(tokenized_line, words_num);
					fclose(new_fp);
					return -1;
			}
		}
		
		else{/*not in macro*/
			if(strcmp(tokenized_line[0],"macro") == 0){/*a declaration of a macro*/
				if(words_num != 2){/*a macro line should have 2 words: a declaration and a name*/
					status.syntaxErr = 1;
					printf("Syntax error, line %d: wrong number of words in a macro declaration\n", line_num);
					fputs(line_buffer, new_fp);/*in this case we'll ignore that this part is a macro and keep on reading the file*/
					free_tokenized_line(tokenized_line, words_num);
					continue;
				}
				if((macro_ind = search_macro(macros_table, tokenized_line[1], num_of_macros)) > -2){/*macro exists already or there is an allocation error*/
					if(macro_ind == -1){/*memory allocation error*/
						free_macros_table(macros_table, num_of_macros);
						free(line_buffer);
						free_tokenized_line(tokenized_line, words_num);
						fclose(new_fp);
						return -1;
					}
					status.syntaxErr = 1;
					printf("Synatx error, line %d: macro name '%s' was declared more than once\n", line_num, tokenized_line[1]);
					fputs(line_buffer, new_fp);/*in this case we'll print the macro as it is*/
					free_tokenized_line(tokenized_line, words_num);
					continue;/*moves to the next line, act as if we are not in a macro*/
					
				}
				if(!has_correct_tag_synt(tokenized_line[1])){/*macro name isn't allowed, we will act the same as we will in the other syntax errors*/
					status.syntaxErr = 1;
					printf("Syntax error, line %d: macro name is not allowed\n", line_num);
					fputs(line_buffer, new_fp);
					free_tokenized_line(tokenized_line, words_num);
					continue;
				}
				else{/*everything is fine, we can create a new macro*/
					status.inMacro = 1;
					newest_macro = new_macro(tokenized_line[1]);
					if(newest_macro == NULL){/*memory allocation error*/
						free_macros_table(macros_table, num_of_macros);
						free(line_buffer);
						free_tokenized_line(tokenized_line, words_num);
						fclose(new_fp);
						return -1;
					}
					macros_table_buff = add_macro_to_table(&macros_table, newest_macro, macros_table_buff, num_of_macros);
					if(macros_table_buff < 0){/*allocation error, the macros table and the new macro were freed inside the function*/
						free(line_buffer);
						free_tokenized_line(tokenized_line, words_num);
						fclose(new_fp);
						return -1;
					}
					num_of_macros++;
				}
				
			}
			else if((macro_ind = search_macro(macros_table, tokenized_line[0], num_of_macros)) > -2){/*a macro call\memory allocation error*/
				if(macro_ind == -1){/*memory allocation error*/
					free_macros_table(macros_table, num_of_macros);
					free(line_buffer);
					free_tokenized_line(tokenized_line, words_num);
					fclose(new_fp);
					return -1;
				}
				if(words_num > 1){/*a call to a macro has to be alone in a line*/
					status.syntaxErr = 1;
					printf("Syntax error, line %d: a macro has to be called in a different line\n", line_num);
					fputs(line_buffer, new_fp);
					free_tokenized_line(tokenized_line, words_num);
					continue;
				}
				write_macro(macros_table[macro_ind], new_fp);
			}
			else/*not start of macro call or new macro*/
				fputs(line_buffer, new_fp);

		}
		free_tokenized_line(tokenized_line, words_num);
	}
	fclose(new_fp);
	free_macros_table(macros_table, num_of_macros);
	if(line_len == -1){/*exited loop because of a memory allocation error*/
		return -1;/*my getline function implemention frees the line in this case*/
	}
	free(line_buffer);
	if(status.syntaxErr == 1)
		return -2;
	return 0;
	
}

/*returns the index of the macro if it exists, -1 for a memory allocation problem and -2 if the macro isn't in the list*/
int search_macro(struct macro** macro_t, char* name, int num_of_macros){
	int i;
	char* currName;
	for(i = 0; i < num_of_macros; i++){
		currName = get_macro_name(macro_t[i]);
		if(currName == NULL)
			return -1;/*memory allocation error*/
		if(strcmp(currName,name) == 0){
			free(currName);
			return i;
		}
		free(currName);
	}
	return -2;
}

/*adds a macro to the macro table, if there is a memory allocation error it returns -1, else it returns the buffer length of the table*/
int add_macro_to_table(struct macro*** macro_t_pt, struct macro* mpt, int buff_len, int num_of_macros){
	struct macro** temp_table;
	int new_buff_len = buff_len;
	if(num_of_macros >= buff_len){
		new_buff_len *= 2;
		temp_table = realloc(*macro_t_pt, sizeof(struct macro*)*new_buff_len);
		if(temp_table == NULL){
			free_macros_table(*macro_t_pt, num_of_macros);
			free_macro(mpt);
			return -1;/*memory allocation error*/
		}
		*macro_t_pt = temp_table;
	}
	(*macro_t_pt)[num_of_macros] = mpt;
	return new_buff_len;
}

void free_macros_table(struct macro** macro_t, int num_of_macros){
	int i;
	for(i = 0; i < num_of_macros; i++){
		free_macro(macro_t[i]);
	}
	free(macro_t);
}
