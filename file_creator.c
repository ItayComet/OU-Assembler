#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_utils.h"
#include "hash_table.h"
#include "machine_utils.h"

int create_ob_file(char* name, machine_word** instruction_code, machine_word** data_code, int ic, int dc){
	char* file_name;
	FILE* fp;
	int i, line_counter = FIRST_MEMORY_ADDRESS;
	char *word_32base, *address_32base;
	
	file_name = add_extension(name, ".ob");
	if(file_name == NULL)/*allocation error*/
		return -1;
	fp = fopen(file_name, "w");
	/*print number of instruction lines and number of data lines (machine code length)*/
	address_32base = get_num_base32_form(ic);
	if(address_32base == NULL){
		free(file_name);
		return -1;
	}
	fprintf(fp, "%s ", address_32base);
	free(address_32base);
	address_32base = get_num_base32_form(dc);
	if(address_32base == NULL){
		free(file_name);
		return -1;
	}
	fprintf(fp, "%s\n", address_32base);
	free(address_32base);
	
	/*print the instruction code*/
	for(i = 0; i < ic; i++, line_counter++){
		address_32base = get_num_base32_form(line_counter);
		if(address_32base == NULL){
			free(file_name);
			return -1;
		}
		word_32base = get_word_base32_form(instruction_code[i]);
		if(word_32base == NULL){
			free(file_name);
			free(address_32base);
			return -1;
		}
		
		fprintf(fp, "%s %s\n", address_32base, word_32base);
		free(address_32base);
		free(word_32base); 
	}
	for(i = 0; i < dc; i++, line_counter++){
		address_32base = get_num_base32_form(line_counter);
		if(address_32base == NULL){
			free(file_name);
			return -1;
		}
		word_32base = get_word_base32_form(data_code[i]);
		if(word_32base == NULL){
			free(file_name);
			free(address_32base);
			return -1;
		}
		
		fprintf(fp, "%s %s\n", address_32base, word_32base);
		free(address_32base);
		free(word_32base); 
	}
	printf("The file \"%s\" was created\n", file_name);
	free(file_name);
	fclose(fp);
	return 0;
}

int create_ent_file(char* name, hash_table* tags_tab, char** entries_tab, int entries_num, int ic){
	char *file_name, *address_32base;
	int i, address;
	FILE *fp;
	tag_sign sign;
	
	file_name = add_extension(name, ".ent");
	if(file_name == NULL)/*allocation error*/
		return -1;
	fp = fopen(file_name, "w");
	for(i = 0; i < entries_num; i++){
		sign = hash_lookup_sign(tags_tab,entries_tab[i]);
		address = hash_lookup_val(tags_tab, entries_tab[i]) + FIRST_MEMORY_ADDRESS;
		if(sign == SIGN_DATA || sign == SIGN_STRCT)
			address += ic;
		address_32base = get_num_base32_form(address);
		if(address_32base == NULL){
			free(file_name);
			return -1;
		}
		fprintf(fp, "%s\t%s\n", entries_tab[i], address_32base);
	}
	printf("The file \"%s\" was created\n", file_name);
	free(file_name);
	fclose(fp);
	return 0;
}

int create_ext_file(char* name, char* ext_str){
	char* file_name;
	FILE* fp;
	file_name = add_extension(name, ".ext");
	if(file_name == NULL)/*allocation error*/
		return -1;
	fp = fopen(file_name, "w");
	fputs(ext_str, fp);
	fclose(fp);
	printf("The file \"%s\" was created\n", file_name);
	free(file_name);
	return 0;
}
