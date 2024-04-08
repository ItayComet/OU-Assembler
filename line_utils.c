#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "line_utils.h"

char* strdup(char* str){
	char* dup;
	int len, i;
	len = strlen(str);
	dup = malloc(sizeof(char)*(len + 1));
	if(dup == NULL)
		return NULL;
	for(i = 0; i <= len; i++){
		dup[i] = str[i];
	}
	return dup;
}

char** tokenize_line(char* lineStr, int* num_of_words){
	int inWord = 0, inString = 0;
	char stringToAdd[2];
	char **tokenized_line, **temp_tok_pt;
	char *currWord, *tempWordPt;
	char c;
	int i, currWordSize = 5, length = strlen(lineStr), currWordInd = 0, tok_buffer_length;
	
	stringToAdd[1] = '\0';/*every time we will add a char with the strcat(), so index 0 will be the current char*/
	
	/*allocate the tokenized array*/
	tok_buffer_length = 8;
	tokenized_line = malloc(sizeof(char*) * tok_buffer_length);
	if(tokenized_line == NULL)
		return NULL;
	
	/*allocate the word buffer, this one will hold each string of a word we get*/
	currWord = malloc(sizeof(char)*currWordSize);
		if(currWord == NULL){
			free(tokenized_line);
			return NULL;
		}
	
	/*fructure the line*/
	for(i = 0; i <= length; i++){
		c = lineStr[i];
		if((!inString && (isspace(c) || c == ',')) || c == '\0' || (inString && c == '"')){
			if(inWord || (inString && (c == '"' || c== '\0'))){/*flag we are not in a word anymore and add the wod to the array*/
				if(inString && c == '"'){/*add the \" to the string*/
					stringToAdd[0] = c;
					if(currWordSize <= strlen(currWord) + 1){
						currWordSize *= 2;
						tempWordPt = currWord;
						currWord = realloc(currWord, sizeof(char)*currWordSize);
						if(currWord == NULL){
							free(tempWordPt);
							free_tokenized_line(tokenized_line, currWordInd);
							return NULL;
						}
					}
					strcat(currWord, stringToAdd);
				}
				inWord = 0;
				inString = 0;
				tokenized_line[currWordInd] = strdup(currWord);
				if(tokenized_line[currWordInd] == NULL){/*free everything*/
					free_tokenized_line(tokenized_line, currWordInd);
					free(currWord);
					return NULL;
				}
				currWordInd++;
				if(currWordInd >= tok_buffer_length){/*reallocation*/
					tok_buffer_length *= 2;
					temp_tok_pt = tokenized_line;
					tokenized_line = realloc(tokenized_line, sizeof(char*) * tok_buffer_length);
						if(tokenized_line == NULL){
							free_tokenized_line(temp_tok_pt, currWordInd);
							free(currWord);
							return NULL;
						}
				}
			}
			if(c == ','){/*add it as a separate tokenized word*/
				tokenized_line[currWordInd] = strdup(",");
				if(tokenized_line[currWordInd] == NULL){/*free everything*/
					free_tokenized_line(tokenized_line, currWordInd);
					free(currWord);
					return NULL;
				}
				currWordInd++;
				if(currWordInd >= tok_buffer_length){/*reallocation*/
					tok_buffer_length *= 2;
					temp_tok_pt = tokenized_line;
					tokenized_line = realloc(tokenized_line, sizeof(char*) * tok_buffer_length);
						if(tokenized_line == NULL){
							free_tokenized_line(temp_tok_pt, currWordInd);
							free(currWord);
							return NULL;
						}
				}
			}
		}
		else{
			if(!inWord && !inString){/*start a new word*/
				if(c == '"')
					inString = 1;
				else
					inWord = 1;
				currWord[0] = '\0';
			}
			stringToAdd[0] = c;
			if(currWordSize <= strlen(currWord) + 1){
				currWordSize *= 2;
				tempWordPt = currWord;
				currWord = realloc(currWord, sizeof(char)*currWordSize);
				if(currWord == NULL){
					free(tempWordPt);
					free_tokenized_line(tokenized_line, currWordInd);
					return NULL;
				}
			}
			strcat(currWord, stringToAdd);
		}
	}
	free(currWord);
	
	*num_of_words = currWordInd;
	return tokenized_line;
}

void free_tokenized_line(char** line, int num_of_words){
	int i;
	for(i = 0; i < num_of_words; i++){
		free(line[i]);
	}
	free(line);
}

/*buff_ptr has to point to an ALLOCATED char array, as it might be reallocated using realloc*/
int getline(FILE *fp, char** buff_ptr, int *buff_size){
	char c;
	int len = 0;
	char* temp_ptr;
	
	if(*buff_size < 8){/*arbitary choice for a number*/
		*buff_size = 8;
		temp_ptr = *buff_ptr;
		*buff_ptr = realloc(*buff_ptr, sizeof(char)*8);
		if(*buff_ptr == NULL){
			free(temp_ptr);
			return -1;
		}
	}
		
	while(((c = fgetc(fp)) != '\n') && (c != EOF)){
		if(*buff_size <= len + 3){
			*buff_size *= 2;
			temp_ptr = *buff_ptr;
			*buff_ptr = realloc(*buff_ptr, sizeof(char)*(*buff_size));
			if(*buff_ptr == NULL){
				free(temp_ptr);
				return -1;
			}
		}
		(*buff_ptr)[len] = c;
		len++;
	}
	
	if(c == '\n'){/*to make sure '\n' is added to the file and EOF doesn't, so when len is 0 we'll know we've finished reading*/
		(*buff_ptr)[len] = '\n';
		len++;
	}
	(*buff_ptr)[len] = '\0';
	return len;
}

int is_instruction(char* word){
	char* instructions[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","get","prn","jsr","rts","hlt"};
	int i;
	for(i = 0; i < 16; i++){
		if(strcmp(word, instructions[i]) == 0)
			return 1;
	}
	return 0;
}

instruction_type get_instruction_type(char* word){
	char* instructions[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","get","prn","jsr","rts","hlt"};
	instruction_type i;
	for(i = 0; i <= INST_HLT; i++){
		if(strcmp(word,instructions[i]) == 0)
			return i;
	}
	return INST_ERROR;
}

int is_saved_word(char* word){
	int i;
	char* saved_words[]={"macro","endmacro"};
	if(is_instruction(word))
		return 1;
	if(is_register(word))
		return 1;
	for(i = 0; i < 2; i++){
		if(strcmp(word, saved_words[i]) == 0)
			return 1;
	}
	return 0;
}

/*the function assumes it gets the tag without the ':' char*/
int has_correct_tag_synt(char* tag){
	int i, len = strlen(tag);
	if(!isalpha(tag[0]) || len > 30)
		return 0;
	if(is_saved_word(tag))
		return 0;
	for(i = 1; i < len; i++){
		if(!isalnum(tag[i]))
			return 0;
	}
	return 1;
}

int ends_with_colon(char* str){
	int i = 0;
	
	/*get the index of the last char before '\0'*/
	while(str[i] != '\0')
		i++;
	i--;/*the tokenize_line func doesn't return empty strings, so 'i' won't be -1*/
	
	if(str[i] == ':'){
		str[i] = '\0';/*remove the colon*/
		return 1;
	}
	return 0;
}

int is_directive(char* word){
	if(word[0] == '.')
		return 1;
	return 0;
}

directive_type get_dir_type(char* word){
	if(strcmp(word,".data") == 0){
		return DIR_DATA;
	}
	if(strcmp(word,".struct") == 0){
		return DIR_STRCT;
	}
	if(strcmp(word,".string") == 0){
		return DIR_STR;
	}
	if(strcmp(word,".entry") == 0){
		return DIR_ENT;
	}
	if(strcmp(word,".extern") == 0){
		return DIR_EXT;
	}
	return DIR_ERROR;/*since only directive lines start with a '.' char, and this one starts with it as well, we can be sure that since it is not a directive line, this is a syntax error*/
}



int is_integer(char* str){
	int i = 1;
	int len = strlen(str);
	if(len == 0)
		return 0;
	if(len == 1 && !isdigit(str[0]))
		return 0;
	if(!((str[0] == '+' || str[0] == '-') || isdigit(str[0]))){
		return 0;
	}
	while(str[i] != '\0'){
		if(!isdigit(str[i]))
			return 0;
		i++;
	}
	return 1;
	
}

int is_comma(char* str){
	if(strlen(str) != 1)
		return 0;
	return (str[0] == ',');
}

int has_quotes(char* str){
	if(str[0] == '"' && str[strlen(str) - 1] == '"')
		return 1;
	return 0;
}

int str_to_int(char* str){/*the function assumes that the str IS integer, in practice we won't use it without using is_integer() first*/
	int i, num = 0, curr_digit_place = 1;/*curr_digit_place will be multiplied by 10 each time we go a char left in the loop, to represent the digit place*/
	int len = strlen(str);
	for(i = len - 1; i >= 0; i--){
		if(i == 0 && (str[0] == '-' || str[0] == '+')){
			if(str[0] == '-')
				num *= -1;
		}
		else{
		num += (str[i] - '0') * curr_digit_place;
		curr_digit_place *= 10;
		}
	}
	return num;
}

int is_imm_adr(char* str){
	int i = 0;
	if(strlen(str) < 2)
		return 0;
	if(str[0] == '#'){
		while(str[i] != '\0'){
			str[i] = str[i+1];/*this loop deletes the # at the start of the word*/
			i++;
		}
		return 1;
	}
	return 0;
}

int is_register(char* str){
	if(strlen(str) != 2)
		return 0;
	if(str[0] == 'r' && ((str[1] - '0') >= 0 && (str[1] - '0') < 8))
		return 1;
	return 0; 
}

int is_strct_adr(char* str){
	int dot_pos = 0, len = strlen(str);
	while(str[dot_pos] != '.' && str[dot_pos] != '\0'){
		dot_pos++;
	}
	if(str[dot_pos] == '\0')/*not dot in string*/
		return 0;
	if(dot_pos == 0)
		return 0;/*no tag before the dot*/
	str[dot_pos] = '\0';/*temporarly making the string end before the dot*/
	if(!has_correct_tag_synt(str)){
		str[dot_pos] = '.';
		return 0;
	}
	str[dot_pos] = '.';
	if(len != dot_pos + 2)/*there should be a single digit after the dot and that's it*/
		return 0;
	if(str[dot_pos + 1] != '1' && str[dot_pos + 1] != '2')
		return 0;
	return 1;
}

addressing_type get_adr_type(char* str){
	if(is_imm_adr(str))
		return ADR_IMM;/*immediate addressing, also removes the # from str*/
	if(has_correct_tag_synt(str))
		return ADR_DIR;/*direct addressing*/
	if(is_strct_adr(str))
		return ADR_STRCT;/*access to struct addressing*/
	if(is_register(str))
		return ADR_REG;/*register addressing*/
	return ADR_ERROR; /*none of the above*/
}

char* add_extension(char* name, char* extension){
	char* new_str;/*the string we'll return*/
	int name_len = strlen(name);
	new_str = malloc(sizeof(char)*(name_len + strlen(extension) + 1));
	if(new_str == NULL)
		return NULL;
	new_str[0] = '\0';
	strcat(new_str, name);
	strcat(new_str, extension);
	return new_str;
}

char* realloc_strcat(char* str, char* extension){
	int new_size;
	char* new_str;
	
	if(str == NULL)
		return NULL;
	
	new_size = strlen(str) + strlen(extension) + 1;
	new_str = realloc(str, new_size*sizeof(char));
	if(new_str == NULL){
		free(str);
		return NULL;
	}
	strcat(new_str, extension);
	
	return new_str;
	
}
