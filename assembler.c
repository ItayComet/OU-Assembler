#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_utils.h"
#include "first_pass.h"
#include "second_pass.h"
#include "hash_table.h"
#include "machine_utils.h"
#include "file_creator.h"
#include "pre_assembler.h"

int pre_assembler(int argc, char **argv);
int assembler(int argc, char **argv);

/*The main function of the assembler consists of the pre assembler, and then, if the pre assembler ends without errors, it continues to the assembler itself, the user is supposed to open the assembler with the names of the .as files without the '.as' extension itself*/
int main(int argc, char **argv){
	if(pre_assembler(argc, argv) == 0)/*The pre assembly stage was successful for all files*/
		assembler(argc, argv);
	return 0;
}

/*Here we run the pre_assemble function on each of the files the user had provided.
This is also the first time we actually check if the files actually exist.
If the pre_assembler finishes with no errors (there are only a few error checks here, it only checks the macros) then 0 will be returned, else, in a case of an error in one of the files (or an allocation error)
the program will return a non zero value
*/
int pre_assembler(int argc, char **argv){
	int i;
	FILE* fp;
	char* file_name;
	int err = 0;
	int func_status = 0;
	for(i = 1; i < argc; i++){
		file_name = add_extension(argv[i], ".as");
		if(file_name)
		fp = fopen(file_name, "r");
		if(fp == NULL){
			printf("The file \"%s\" doesn't exist, please make sure to write the name of the file without the extension\n", file_name);
			free(file_name);
			err = -2;
			continue;
		}
		printf("In file \"%s\":\n", file_name);
		free(file_name);
		func_status = pre_assemble(fp, argv[i]);
		fclose(fp);
		if(func_status < 0){
			if(func_status == -1){
				printf("pre assembly stage failed due to an allocation error\n");
				return -1;
			}
			else{
				printf("failed pre assembly for this file\n\n"); 
				err = -2;
				continue;
			}
		}
		else
			printf("Pre assembly ended successfully");
	}
	if(err < 0){
		printf("Pre assembly stage failed\n");
		return -2;
	}
	printf("Pre assembly stage is finished with no errors\n\n");
	return 0;
}


void free_entries_tab(char** tab, int length);
void free_code_table_content(machine_word** table, int length);
void free_code_table(machine_word** table, int length);
/*The assembler starts with the presumption that the '.am' files were created successully for all '.as' files, and there are no more macros or macro calls in the '.am' files, the warnings the assembler return are for the '.am' files and not the '.as' files.
The assembler starts by creating two arrays to hold the data and instruction code separatly, and after finishing (or failing) the two passes, the arrays will be cleaned of code, but still there for the next pass. A new hash table is created for the tags for each file.
Each file is compiled separatly, so if there is an error in one of the files, the assembler will still create everything for the other files.
If the first pass ends successfully with no errors, the assembler will create an array (and its size is the number of entries, as checked in the first pass) to hold the entries names to use late in the file creator. I chose to make the contents of the externs file as one big string (of course it could be an array of linked lists, but the space complexity of a string is not as big), which will be first allocated here and reallocated each time in the second pass with a dedicated function.
Note that the errors which are found on the second pass will not be found if the first pass fails.
*/
int assembler(int argc, char **argv){
	int i, func_status, data_counter, instruction_counter;
	int entries_num;
	FILE* fp;
	char **entries_tab;
	char *file_name, *ext_str;
	machine_word **instruction_code, **data_code;
	hash_table *tags_table;
	int err = 0;
	
	if(argc == 1){
		printf("Please open the program with the name of the files (without the '.as' extension) as parameters\n");
		return -2;
	}
	instruction_code = malloc(sizeof(machine_word*) * MEMORY_SPACE);
	if(instruction_code == NULL)
		return -1;
	data_code = malloc(sizeof(machine_word*) * MEMORY_SPACE);
	if(data_code == NULL){
		free(instruction_code);
		return -1;
	}
	
	for(i = 1; i < argc; i++){
		data_counter = 0;
		instruction_counter = 0;
		entries_num = 0;
		tags_table = new_hash_table(HASH_SIZE);
		if(tags_table == NULL){
			free_code_table(data_code, data_counter);
			free_code_table(instruction_code, instruction_counter);
			return -1;
		}
		file_name = add_extension(argv[i], ".am");
		if(file_name == NULL){
			free_code_table(data_code, data_counter);
			free_code_table(instruction_code, instruction_counter);
			free_hash_table(tags_table);
			return -1;
		}
		fp = fopen(file_name, "r");/*the file exists because we don't continue to this function if we didn't make one of the files*/
		printf("In file \"%s\":\n", file_name);
		free(file_name);
		func_status = first_pass(fp, instruction_code, data_code, tags_table, &instruction_counter, &data_counter, &entries_num);
		if(func_status < 0){
			free_code_table_content(data_code, data_counter);
			free_code_table_content(instruction_code, instruction_counter);
			free_hash_table(tags_table);
			if(func_status == -1)/*allocation error*/
				return -1;
			else{/*syntax errors in this file, we will compile the other files*/
				err = 1;
				printf("\n");
				continue;
			}
		}
		/*second pass preparations*/
		rewind(fp);
		ext_str = malloc(sizeof(char));
		entries_tab = calloc(entries_num, sizeof(char*));
		if(entries_tab == NULL || ext_str == NULL){
			free_code_table(data_code, data_counter);
			free_code_table(instruction_code, instruction_counter);
			free_hash_table(tags_table);
			free(ext_str);
			free(entries_tab);
			return -1;
		}
		ext_str[0] = '\0';
		/*second pass*/
		func_status = second_pass(fp, instruction_code, tags_table, instruction_counter, entries_tab, &ext_str);
		if(func_status < 0){
			free(ext_str);
			free_code_table_content(data_code, data_counter);
			free_code_table_content(instruction_code, instruction_counter);
			free_entries_tab(entries_tab, entries_num);
			free_hash_table(tags_table);
			if(func_status == -1){/*allocation error*/
				free(data_code);
				free(instruction_code);
				return -1;
			}
			else{/*syntax errors in this file, we will compile the other files*/
				err = 1;
				printf("\n");
				continue;
			}
		}
		/*create files*/
		create_ob_file(argv[i], instruction_code, data_code, instruction_counter, data_counter);
		if(entries_num > 0)
			create_ent_file(argv[i], tags_table, entries_tab, entries_num, instruction_counter);
		if(strlen(ext_str) > 0)
			create_ext_file(argv[i], ext_str);
		
		
		free_entries_tab(entries_tab, entries_num);
		free_code_table_content(data_code, data_counter);
		free_code_table_content(instruction_code, instruction_counter);
		free_hash_table(tags_table);
		free(ext_str);
		printf("\n");
		
	}
	free_code_table(data_code, 0);
	free_code_table(instruction_code, 0);
	if(err)
		printf("Errors were found in some of the files, assembly failed\n");
	else
		printf("Assembler finished successfully\n");
	return 0;
}

void print_machine_table(machine_word** table, int length){
	char* str;
	int i;
	for(i = 0; i < length; i++){
		str = word_to_str(table[i]);
		printf("%d. %s\n", i+1, str);
	}
}
void free_code_table_content(machine_word** table, int length){
	int i;
	for(i = 0; i < length; i++)
		free(table[i]);
}
void free_code_table(machine_word** table, int length){
	free_code_table_content(table, length);
	free(table);
}

void free_entries_tab(char** tab, int length){
	int i;
	for(i = 0; i < length && tab[i] != NULL; i++)
		free(tab[i]);
	free(tab);
}
