#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_utils.h"
#include "hash_table.h"

void add_to_ext_str(char** ext_str, char* tag, int code_line_num);
int fill_empty_words(char** tok_line, hash_table* tags_table, machine_word** instruction_code, int *ic, int d_addr_offset, int line_num, char** ext_str);

int second_pass(FILE* fp, machine_word** instruction_code, hash_table* tags_table, int d_addr_offset, char** entries_tab, char** ext_str){
	int ic = 0, line_cnt = 0, buffer_len;
	int words_num, temp_words_num, ent_val, ent_num = 0;
	char *line_buffer, **tok_line, **temp_tok_line;
	int syntaxErr = 0;
	
	/*create buffer for the line*/
	buffer_len = 64;
	line_buffer = malloc(buffer_len*sizeof(char));
	if(line_buffer == NULL){
		return -1;/*memory allocation failed*/
	}
	
	while((getline(fp, &line_buffer, &buffer_len)) > 0){
		line_cnt++;
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
		we will keep a temp_tok_line so we can free it later in both cases, just as we did in the first pass, but this time we don't need to save or use the tag*/
		temp_tok_line = tok_line;
		temp_words_num = words_num;
		
		/*check if there is a tag: if it ends with colons, it is a tag, because we checked the syntax in the first pass*/
		if(ends_with_colon(tok_line[0])){
				tok_line++;
				words_num--;
		}
		
		if(is_directive(tok_line[0])){/*we only put entries in the table and ignore other directives*/
			if(get_dir_type(tok_line[0]) == DIR_ENT){/*it won't be an error since we checked it in the first pass*/
				ent_val = hash_lookup_val(tags_table,tok_line[1]);
				if(ent_val < 0){
					printf("Syntax error, line %d: the tag '%s' is not declared in this file\n", line_cnt, tok_line[1]);
					syntaxErr = 1;
					free_tokenized_line(temp_tok_line, temp_words_num);
					continue;
				}
				entries_tab[ent_num] = strdup(tok_line[1]);
				if(entries_tab[ent_num] == NULL){/*allocation error*/
					free_tokenized_line(temp_tok_line, temp_words_num);
					free(line_buffer);
					return -1;
				}
				ent_num++;
				
			}
		}
		else{/*it's an instruction line, else we would have found an error in the first pass*/
			if(fill_empty_words(tok_line, tags_table, instruction_code, &ic, d_addr_offset, line_cnt, ext_str) != 0)
				syntaxErr = 1;
		}
		free_tokenized_line(temp_tok_line, temp_words_num);
	}
	if(syntaxErr != 0)
		return -2;
	free(line_buffer);
	return 0;
		
}

int fill_imm_num(char* num_str, machine_word** instruction_code, int* ic, int line_num){
	int num;
	if(!is_integer(num_str)){
		printf("Syntax error, line %d: An immediate addressing parameter has to be an integer\n", line_num);
			return -2;
	}
	num = str_to_int(num_str);
	if(num > 127 || num < -128){/*the limits of 8bit num*/
		printf("Syntax error, line %d: An integer given in immediate addressing method has to be between -128 and 127\n", line_num);
		return -2;
	}
	hold_num(instruction_code[*ic], num, 8);
	(*ic)++;
	return 0;
}

int fill_tag_word(char* tag, hash_table* tags_table, machine_word** instruction_code, int* ic, int line_num, int d_addr_offset, char** ext_str){
	tag_sign sign;
	int tag_address = hash_lookup_val(tags_table, tag);
	sign = hash_lookup_sign(tags_table, tag);
	if(tag_address < 0){
		printf("Syntax error, line %d: The tag '%s' is not declared in this file\n", line_num, tag);
		return -2;
	}
	if(sign != SIGN_EXTERN)
		tag_address += FIRST_MEMORY_ADDRESS;
	if(sign == SIGN_DATA || sign == SIGN_STRCT){
		tag_address += d_addr_offset;/*add the offset of the data code*/
	}
	hold_num(instruction_code[*ic], tag_address, 8);
	if(sign == SIGN_EXTERN){
		set_encoding_type(instruction_code[*ic], ENC_EXT);
		add_to_ext_str(ext_str, tag, *ic + FIRST_MEMORY_ADDRESS);
	}
	else
		set_encoding_type(instruction_code[*ic], ENC_REL);
	(*ic)++;
	return 0;
}

int fill_strct_words(char* tag, hash_table* tags_table, machine_word** instruction_code, int* ic, int line_num, int d_addr_offset, char** ext_str){
	int tag_address;
	tag_sign sign;
	int strct_field = tag[strlen(tag)-1] - '0';
	
	tag[strlen(tag)-2] = '\0';/*turn the struct into its tag*/
	tag_address = hash_lookup_val(tags_table, tag);
	sign = hash_lookup_sign(tags_table, tag);
	if(tag_address < 0){
		printf("Syntax error, line %d: The struct '%s' is not declared in this file\n", line_num, tag);
		return -2;
	}
	if(sign != SIGN_STRCT && sign != SIGN_EXTERN){
	/*I don't count this one as a syntax error, the users just won't get the address they might want, however a warning will be printed*/
		printf("Warning, line %d: trying to access fields of a non struct data\n", line_num);
	}
	if(sign != SIGN_EXTERN)
		tag_address += FIRST_MEMORY_ADDRESS;
	if(sign == SIGN_STRCT || sign == SIGN_DATA)
		tag_address += d_addr_offset;/*add the offset of the data code*/
	hold_num(instruction_code[*ic], tag_address, 8);
	if(sign == SIGN_EXTERN){
		set_encoding_type(instruction_code[*ic], ENC_EXT);
		add_to_ext_str(ext_str, tag, *ic + FIRST_MEMORY_ADDRESS);
	}
	else
		set_encoding_type(instruction_code[*ic], ENC_REL);
	(*ic)++;
	hold_num(instruction_code[*ic], strct_field, 8);
	(*ic)++;
	return 0;
}

int fill_next_word(char* operand, addressing_type adr_type, hash_table* tags_table, machine_word** instruction_code, int* ic, int d_addr_offset, int line_num, char** ext_str){
	if(adr_type == ADR_IMM){
		if(fill_imm_num(operand, instruction_code, ic, line_num) != 0)
			return -2;
	}
	else if(adr_type == ADR_DIR){
		if(fill_tag_word(operand, tags_table, instruction_code, ic, line_num, d_addr_offset, ext_str) != 0)
			return -2;
	}
	else if(adr_type == ADR_STRCT){
		if(fill_strct_words(operand, tags_table, instruction_code, ic, line_num, d_addr_offset, ext_str) != 0)
			return -2;
	}
	return 0;
}

int fill_empty_words(char** tok_line, hash_table* tags_table, machine_word** instruction_code, int *ic, int d_addr_offset, int line_num, char** ext_str){
	addressing_type src_adr_type, dest_adr_type;
	instruction_type inst = get_instruction_type(tok_line[0]);
	int reg_src_num, reg_dest_num;
	(*ic)++;/*an instruction word is coded anyway, and since we already did it in the first pass, all we have to do now is to code its extra words*/
		if(inst <= INST_SUB || inst == INST_LEA){/*an instruction with two operands*/
			src_adr_type = get_adr_type(tok_line[1]);
			dest_adr_type = get_adr_type(tok_line[3]);
			if(src_adr_type == ADR_REG && dest_adr_type == ADR_REG){/*the only case with a shared word between two operands*/
				reg_src_num = tok_line[1][1] - '0';/*the syntax was checked in the first pass*/
				reg_dest_num = tok_line[3][1] - '0';
				hold_reg_num(instruction_code[*ic], reg_src_num, reg_dest_num);
				(*ic)++;
			}
			else{
				/*fill src words*/
				if(src_adr_type == ADR_REG){
					reg_src_num = tok_line[1][1] - '0';
					hold_reg_num(instruction_code[*ic], reg_src_num, -1);
					(*ic)++;
				}
				else{
					if(fill_next_word(tok_line[1], src_adr_type, tags_table, instruction_code, ic, d_addr_offset, line_num, ext_str))
						return -2;/*syntax error*/
				}
				
				/*fill dest words*/
				if(dest_adr_type == ADR_REG){
					reg_dest_num = tok_line[3][1] - '0';
					hold_reg_num(instruction_code[*ic], -1, reg_dest_num);
					(*ic)++;
				}
				else{
					if(fill_next_word(tok_line[3], dest_adr_type, tags_table, instruction_code, ic, d_addr_offset, line_num, ext_str))
						return -2;/*syntax error*/
				}
			}
		}
		else if(inst <= INST_JSR){/*an instruction with one operand*/
			dest_adr_type = get_adr_type(tok_line[1]);
			if(dest_adr_type == ADR_REG){
				reg_dest_num = tok_line[1][1] - '0';
				hold_reg_num(instruction_code[*ic], -1, reg_dest_num);
				(*ic)++;
			}
			else{
				if(fill_next_word(tok_line[1], dest_adr_type, tags_table, instruction_code, ic, d_addr_offset, line_num, ext_str))
					return -2;/*syntax error*/
			}
		}
		else{/*an instruction with no operands*/
			/*nothing to see here*/
		}
		return 0;
	
}

void add_to_ext_str(char** ext_str, char* tag, int code_line_num){
	char* line_num;
	*ext_str = realloc_strcat(*ext_str, tag);
	*ext_str = realloc_strcat(*ext_str, "\t");
	line_num = get_num_base32_form(code_line_num);
	if(line_num == NULL){
		free(*ext_str);
		*ext_str = NULL;
		return;
	}
	*ext_str = realloc_strcat(*ext_str, line_num);
	*ext_str = realloc_strcat(*ext_str, "\n");
	free(line_num);
}
