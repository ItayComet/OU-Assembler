#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_utils.h"
#include "first_pass.h"

int add_to_instruction_code(char** tok_line, int words_num, int line_num, machine_word** instruction_code, hash_table *tags_table, char* tag, int* instruction_counter);
int add_to_data_code(directive_type dir, char** tok_line, int words_num, int line_num, machine_word** data_code, hash_table *tags_table, char* tag, int* data_counter);

int first_pass(FILE* fp, machine_word **instruction_code, machine_word **data_code, hash_table *tags_table, int *instruction_counter, int *data_counter, int *entries_num){
	char *line_buffer, *tag = NULL, **tok_line, **temp_tok_line;
	int func_status, buffer_len, words_num, temp_words_num, line_cnt = 0, line_len;
	directive_type dir;
	struct status{
		unsigned int syntaxErr : 1;
		unsigned int tagged : 1;
		unsigned int memoryOverflow : 1;
	} status;
	
	/*create buffer for the line*/
	buffer_len = 64;
	line_buffer = malloc(buffer_len*sizeof(char));
	if(line_buffer == NULL){
		return -1;/*memory allocation failed*/
	}
	
	*data_counter = 0, *instruction_counter = 0;/*IC/DC, represents the next available place in the code arrays, the user provides the address of the integers*/
	
	while((line_len = getline(fp, &line_buffer, &buffer_len)) > 0){
		line_cnt++;
		if(line_len > LINE_LENGTH_LIMIT + 1){/*+'\n'*/
			printf("Syntax error, line %d: number of characters exceeds the limit of %d\n", line_cnt, LINE_LENGTH_LIMIT);
			continue;
		}
		tok_line = tokenize_line(line_buffer, &words_num);
		if(tok_line == NULL){/*memory allocation error*/
			free(line_buffer);
			return -1;
		}
		if(words_num == 0 || tok_line[0][0] == ';'){/*empty line or a comment, we ignore this line*/
			free_tokenized_line(tok_line, words_num);
			continue;
		}
		
		/*If there is a tag we will use the increment operator on tok_line
		we will keep a temp_tok_line so we can free it later in both cases, same for the number of words*/
		temp_tok_line = tok_line;
		temp_words_num = words_num;
		
		/*check if there is a tag*/
		if(ends_with_colon(tok_line[0])){/*also removes the colon for easy use*/
			if(words_num <= 1){/*no words after the tag*/
				printf("Syntax error, line %d: A line has to contain more than a tag\n", line_cnt);
				status.syntaxErr = 1;
				free_tokenized_line(temp_tok_line, temp_words_num);
				continue;
			}
			if(!has_correct_tag_synt(tok_line[0])){
				printf("Syntax error, line %d: Wrong tag syntax for '%s'\n", line_cnt, tok_line[0]);
				status.syntaxErr = 1;
				free_tokenized_line(temp_tok_line, temp_words_num);
				continue;
			}
			if(hash_lookup_val(tags_table,tok_line[0]) >= 0){
				printf("Syntax error, line %d: Tag '%s' can not be declared more than once\n", line_cnt, tok_line[0]);
				status.syntaxErr = 1;
				free_tokenized_line(temp_tok_line, temp_words_num);
				continue;
			}
			else{/*syntax is ok and tag doesn't exist*/
				tag = tok_line[0];
				tok_line++;
				words_num--;
			}
		}
		if(is_directive(tok_line[0])){/*directive line*/
			dir = get_dir_type(tok_line[0]);
			if(dir == DIR_ERROR){
				status.syntaxErr = 1;
				printf("Syntax error, line %d: non existant command\n", line_cnt);
				free_tokenized_line(temp_tok_line, temp_words_num);
				continue;
			}
			else if(dir == DIR_DATA || dir == DIR_STR || dir == DIR_STRCT){
				func_status = add_to_data_code(dir, tok_line, words_num, line_cnt, data_code, tags_table, tag, data_counter);
				if(func_status < 0){
					if(func_status == -1){/*allocation failure*/
						free(line_buffer);
						free_tokenized_line(temp_tok_line, temp_words_num);
						return -1;
					}
					if(func_status == -2){/*syntax error*/
						status.syntaxErr = 1;
					}
					if(func_status == -3 || (*data_counter + *instruction_counter > MEMORY_SPACE)){/*memory overflow*/
						printf("Overflow error, line %d: exceeding available memory space\n", line_cnt);
						return -3;
					}
				}
			}
			else if(dir == DIR_EXT){
				if(words_num != 2){
					printf("Synatx error, line %d: '.extern' must have exactly 1 parameter\n", line_cnt);
					status.syntaxErr = 1;
					free_tokenized_line(temp_tok_line, temp_words_num);
					continue;
				}
				if(!has_correct_tag_synt(tok_line[1])){
					printf("Syntax error, line %d: .extern parameter is not a valid tag\n", line_cnt);
					status.syntaxErr = 1;
					free_tokenized_line(temp_tok_line, temp_words_num);
					continue;
				}
				if(hash_lookup_val(tags_table,tok_line[1]) >= 0){
					printf("Syntax error, line %d: '%s' was already declared as a tag\n", line_cnt, tok_line[1]);
					status.syntaxErr = 1;
					free_tokenized_line(temp_tok_line, temp_words_num);
					continue;
				}
				func_status = hash_add(tags_table, tok_line[1], 0, SIGN_EXTERN);
				if(func_status < 0){/*allocation failure*/
					free_tokenized_line(temp_tok_line, temp_words_num);
					free(line_buffer);
					return -1;
				}
			}
			else if(dir == DIR_ENT){
			/*we don't handle .entry lines in the first pass, but I will count them here so I can make an array for them with the exact size*/
				if(words_num != 2){
					printf("Syntax error, line %d: '.entry' must have exactly 1 tag as a parameter\n", line_cnt);
					status.syntaxErr = 1;
				}
				(*entries_num)++;
			}
		}
		else if(is_instruction(tok_line[0])){
			func_status = add_to_instruction_code(tok_line, words_num, line_cnt, instruction_code, tags_table, tag, instruction_counter);
			if(func_status < 0){
				if(func_status == -1){/*allocation failure*/
					free(line_buffer);
					free_tokenized_line(temp_tok_line, temp_words_num);
					return -1;
				}
				else if(func_status == -2){/*syntax error*/
						status.syntaxErr = 1;
				}
				else if(func_status == -3 || (*data_counter + *instruction_counter > MEMORY_SPACE)){/*memory overflow*/
					printf("Overflow error, line %d: exceeding available memory space\n", line_cnt);
					return -3;
				}
			}
		}
		else{/*synatx error*/
			status.syntaxErr = 1;
			printf("Syntax error, line %d: operation not recognized\n", line_cnt);
		}
		
		
		free_tokenized_line(temp_tok_line, temp_words_num);
		tag = NULL;
	}
	free(line_buffer);
	if(status.syntaxErr)
		return -2;
	return 0;
}

int add_to_instruction_code(char** tok_line, int words_num, int line_num, machine_word** instruction_code, hash_table *tags_table, char* tag, int* instruction_counter){
	int num_of_extra_words = 0, i, func_status;
	machine_word *first_word;
	addressing_type dest_adr_type, src_adr_type;
	instruction_type inst;
	inst = get_instruction_type(tok_line[0]);
	
	/*check if there is space available*/
	if(*instruction_counter + 1 > MEMORY_SPACE)/*not enough memory to insert another word*/
		return -3;
	
	if(tag != NULL){/*in the case of an error in the line, we will not continue anyway and the tags_table won't be of any use, so it's ok to add the tag even before we check the syntax of the line*/
		func_status = hash_add(tags_table, tag, *instruction_counter, SIGN_CODE);
		if(func_status < 0)/*allocation failure*/
			return -1;
	}

	/*create the first word and set the 4 left bits to match the instruction*/ 
	first_word = new_machine_word();
	if(first_word == NULL)
		return -1;/*failed allocation*/
	instruction_code[*instruction_counter] = first_word;
	(*instruction_counter)++;
	set_instruction_type(first_word, inst);
	if(inst <= INST_SUB || inst == INST_LEA){/*an instruction with two operands*/
		if(words_num != 4){
			printf("Syntax error, line %d: an '%s' instruction should have 2 operands\n", line_num, tok_line[0]);
			return -2;
		}
		src_adr_type = get_adr_type(tok_line[1]);
		dest_adr_type = get_adr_type(tok_line[3]);
		if(src_adr_type == ADR_ERROR || dest_adr_type == ADR_ERROR){
			printf("Syntax error, line %d: no valid addressing method found\n", line_num);
			return -2;
		}
		/*update the first word*/
		set_adressing_dest(first_word, dest_adr_type);
		set_adressing_src(first_word, src_adr_type);
		
		if((src_adr_type == ADR_IMM || src_adr_type == ADR_REG) && inst == INST_LEA){
			printf("Syntax error, line %d: not a valid addressing method for 'lea' instruction\n", line_num);
			return -2;
		}
		if(dest_adr_type == ADR_IMM && inst != INST_CMP){
			printf("Syntax error, line %d: immediate addressing is not a valid addressing type for '%s'\n", line_num, tok_line[0]);
			return -2;
		}
		num_of_extra_words = get_num_extra_words(src_adr_type, dest_adr_type);
		if(*instruction_counter + num_of_extra_words > MEMORY_SPACE)/*not enough memory to insert this number of words*/
			return -3;
		for(i = 0; i < num_of_extra_words; i++){/*add empty words (all bits are set to 0)*/
			instruction_code[*instruction_counter] = new_machine_word();
			if(instruction_code[*instruction_counter] == NULL)
				return -1;
			(*instruction_counter)++;
		}
		
	}
	else if(inst <= INST_JSR){/*an instruction with one operand*/
		if(words_num != 2){
			printf("Syntax error, line %d: an '%s' instruction should have 1 operand\n", line_num, tok_line[0]);
			return -2;
		}
		dest_adr_type = get_adr_type(tok_line[1]);
		if(dest_adr_type == ADR_ERROR){
			printf("Syntax error, line %d: '%s' is not a valid addressing method\n", line_num, tok_line[1]);
			return -2;
		}
		if(dest_adr_type == ADR_IMM && inst != INST_PRN){/*prn is the only instruction of the ones that get 1 parameter that can get immediate addressing*/
			printf("Syntax error, line %d: immediate addressing is not a valid addressing type for '%s'\n", line_num, tok_line[0]);
			 return -2;
		}
		set_adressing_dest(first_word, dest_adr_type);
		num_of_extra_words = get_num_extra_words(-1, dest_adr_type);
		if(*instruction_counter + num_of_extra_words > MEMORY_SPACE)/*not enough memory to insert this number of words*/
			return -3;
		for(i = 0; i < num_of_extra_words; i++){
			instruction_code[*instruction_counter] = new_machine_word();
			if(instruction_code[*instruction_counter] == NULL)
				return -1;
			(*instruction_counter)++;
		}
		
		
	}
	else{/*an instruction with no operands*/
		/*the instruction was already coded to the word*/
		if(words_num != 1){
			printf("Syntax error, line %d: an '%s' instruction should have no operands\n", line_num, tok_line[0]);
			return -2;
		}
	}
	return 0;
}

/*returns -1 for an allocation error, or -2 for a syntax error, or -3 for memory overflow*/
int add_to_data_code(directive_type dir, char** tok_line, int words_num, int line_num, machine_word** data_code, hash_table *tags_table, char* tag, int* data_counter){
	int func_status, i;
	int curr_integer;
	if(tag != NULL){/*in the case of an error in the line, we will not continue anyway and the tags_table won't be of any use, so it's ok to add the tag even before we check the syntax of the line*/
		func_status = hash_add(tags_table, tag, *data_counter, SIGN_DATA);
		if(func_status < 0)/*allocation failure*/
			return -1;
	}
	
	if(dir == DIR_DATA){/*.data*/
		if(words_num == 1){
			printf("Syntax error, line %d: a '.data' command with no parameters\n", line_num);
			return -2;
		}
		if(words_num % 2 != 0){
			printf("Syntax error, line %d: each pair of integers have to be separated by a comma\n", line_num);
			return -2;
		}
			/*a loop to check the syntax of the sentence*/
		for(i = 1; i < words_num; i++){/*integers are in odd indices, commas in even indices, this loop checks validates the synatax and the length, to make sure there is no overflow*/
			if(i%2 == 0){/*even*/
				if(!is_comma(tok_line[i])){
					printf("Syntax error, line %d: each pair of integers have to be separated by a comma\n", line_num);
					return -2;
				}
			}
			else{/*odd*/
				if(!is_integer(tok_line[i])){
					printf("Syntax error, line %d: '%s' is not an integer\n", line_num, tok_line[i]);
					return -2;
				}
			}
		}
		if(words_num/2 + *data_counter > MEMORY_SPACE){/*exceeding available memory space*/
				return -3;
		}
		
		/*a loop to save the integers*/
		for(i = 1; i < words_num; i += 2){
			data_code[*data_counter] = new_machine_word();
			if(data_code[*data_counter] == NULL)/*failed allocation*/
				return -1;
			curr_integer = str_to_int(tok_line[i]);
			if(curr_integer > 511 || curr_integer < -512){/*limits of a 10bits num in 2's complement*/
				printf("Syntax error, line %d: 10bit system doesn't support this number\n", line_num);
				return -2;
			}
			hold_num(data_code[*data_counter], curr_integer, 10);
			(*data_counter)++;
		}
	}
	
	else if(dir == DIR_STR){/*.string*/
		if(words_num != 2){
			printf("Syntax error, line %d: .string must have 1 operator\n", line_num);
			return -2;
		}
		if(!has_quotes(tok_line[1])){
			printf("Syntax error, line %d: a .string paramater must be between quotation marks\n", line_num);
			return -2;
		}
		if((strlen(tok_line[1]) - 1 + *data_counter) > MEMORY_SPACE)/*length of string with out the quotes and plus an empty char*/
			return -3;
		
		for(i = 1; i < strlen(tok_line[1]) - 1; i++){
			data_code[*data_counter] = new_machine_word();
			if(data_code[*data_counter] == NULL)/*allocation failure*/
				return -1;
			hold_num(data_code[*data_counter], tok_line[1][i], 10);/*this function treats the char as its number*/
			(*data_counter)++;
		}
		/*add empty char at the end*/
		data_code[*data_counter] = new_machine_word();
		if(data_code[*data_counter] == NULL)/*allocation failure*/
			return -1;
		(*data_counter)++;
	}
	
	else if(dir == DIR_STRCT){/*.struct*/
		if(tag != NULL){
			hash_change_sign(tags_table, tag, SIGN_STRCT);
		}
		if(words_num != 4 || !is_comma(tok_line[2])){
			printf("Syntax error, line %d: .struct must have 2 operators with a comma between them\n", line_num);
			return -2;
		}
		/*check the integer*/
		if(!is_integer(tok_line[1])){
			printf("Syntax error, line %d: '%s' is not an integer\n", line_num, tok_line[1]);
			return -2;
		}
		curr_integer = str_to_int(tok_line[1]);
		if(curr_integer > 511 || curr_integer < -512){/*limits of a 10bits num in 2's complement*/
			printf("Syntax error, line %d: 10bit system doesn't support this number\n", line_num);
			return -2;
		}
		
		/*check the string*/
		if(!has_quotes(tok_line[3])){
			printf("Syntax error, line %d: the second .struct paramater must be between quotation marks\n", line_num);
			return -2;
		}
		
		if((strlen(tok_line[3]) + *data_counter) > MEMORY_SPACE)/*length of string without the quotes, plus an empty char, plus the integer*/
			return -3;
		
		/*add the integer*/
		data_code[*data_counter] = new_machine_word();
		if(data_code[*data_counter] == NULL)/*allocation failure*/
			return -1;
		hold_num(data_code[*data_counter], curr_integer, 10);
		(*data_counter)++;
		
		/*add the string*/
		for(i = 1; i < strlen(tok_line[3]) - 1; i++){
			data_code[*data_counter] = new_machine_word();
			if(data_code[*data_counter] == NULL)/*allocation failure*/
				return -1;
			hold_num(data_code[*data_counter], tok_line[3][i], 10);/*this function treats the char as its number*/
			(*data_counter)++;
		}
		/*add an empty char at the end*/
		data_code[*data_counter] = new_machine_word();
		if(data_code[*data_counter] == NULL)/*allocation failure*/
			return -1;
		(*data_counter)++;
	}
	
	return 0;
}
