#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "line_utils.h"

machine_word* new_machine_word(){
	machine_word* wpt;
	wpt = malloc(sizeof(machine_word));
	if(wpt == NULL)
		return NULL;
	wpt->b0 = 0;
	wpt->b1 = 0;
	wpt->b2 = 0;
	wpt->b3 = 0;
	wpt->b4 = 0;
	wpt->b5 = 0;
	wpt->b6 = 0;
	wpt->b7 = 0;
	wpt->b8 = 0;
	wpt->b9 = 0;
	return wpt;
}

/*debugging function*/
char* word_to_str(machine_word* wpt){
	char* str = malloc(sizeof(char)*11);
	if(str == NULL)
		return NULL;
	str[0] = wpt->b9 == 0? '0' : '1';
	str[1] = wpt->b8 == 0? '0' : '1';
	str[2] = wpt->b7 == 0? '0' : '1';
	str[3] = wpt->b6 == 0? '0' : '1';
	str[4] = wpt->b5 == 0? '0' : '1';
	str[5] = wpt->b4 == 0? '0' : '1';
	str[6] = wpt->b3 == 0? '0' : '1';
	str[7] = wpt->b2 == 0? '0' : '1';
	str[8] = wpt->b1 == 0? '0' : '1';
	str[9] = wpt->b0 == 0? '0' : '1';
	str[10] = '\0';
	return str;
}

void set_encoding_type(machine_word* wpt, encoding_type type){
	if(type == ENC_ABS){/*absolute*/
		wpt->b0 = 0;
		wpt->b1 = 0;
	}
	if(type == ENC_EXT){/*external*/
		wpt->b0 = 1;
		wpt->b1 = 0;
	}
	if(type == ENC_REL){/*relocatable*/
		wpt->b0 = 0;
		wpt->b1 = 1;
	}
}

void set_adressing_dest(machine_word* wpt, addressing_type type){
	if(type == ADR_IMM){
		wpt->b2 = 0;
		wpt->b3 = 0;
	}
	if(type == ADR_DIR){
		wpt->b2 = 1;
		wpt->b3 = 0;
	}
	if(type == ADR_STRCT){
		wpt->b2 = 0;
		wpt->b3 = 1;
	}
	if(type == ADR_REG){
		wpt->b2 = 1;
		wpt->b3 = 1;
	}
}
void set_adressing_src(machine_word* wpt, addressing_type type){
	if(type == ADR_IMM){
		wpt->b4 = 0;
		wpt->b5 = 0;
	}
	if(type == ADR_DIR){
		wpt->b4 = 1;
		wpt->b5 = 0;
	}
	if(type == ADR_STRCT){
		wpt->b4 = 0;
		wpt->b5 = 1;
	}
	if(type == ADR_REG){
		wpt->b4 = 1;
		wpt->b5 = 1;
	}
}

void set_instruction_type(machine_word* wpt, instruction_type inst){
	int i;
	int binary_num[4];
	
	for(i = 0; i < 4; i++){/*turn it into a binary num*/
		binary_num[i] = inst%2;
		inst /= 2;
	}
	wpt->b6 = binary_num[0];
	wpt->b7 = binary_num[1];
	wpt->b8 = binary_num[2];
	wpt->b9 = binary_num[3];	
}

void hold_num(machine_word *wpt, int num, int bits){
	int i = 0, isNegative = 0;
	int *binary_num;
	binary_num = malloc(sizeof(int)*bits);
	if(num < 0){
		isNegative = 1;
		num *= -1;/*if x is a positive number, we need to find the binary rep of x before we can find the binary rep of -x*/
	}
	if(num > 512)
		return;

	for(i = 0; i < bits; i++){
		binary_num[i] = num%2;
			num /= 2;
	}
	if(isNegative){
		for(i = 0; i < bits; i++){/*'not' operator*/
			if(binary_num[i])
				binary_num[i] = 0;
			else
				binary_num[i] = 1;
		}
		i = 0;
		while(binary_num[i] != 0){/*add 1*/
			binary_num[i] = 0;
			i++;
		}
		binary_num[i] = 1;
	}
	if(bits == 10){
		wpt->b0 = binary_num[0];
		wpt->b1 = binary_num[1];
		wpt->b2 = binary_num[2];
		wpt->b3 = binary_num[3];
		wpt->b4 = binary_num[4];
		wpt->b5 = binary_num[5];
		wpt->b6 = binary_num[6];
		wpt->b7 = binary_num[7];
		wpt->b8 = binary_num[8];
		wpt->b9 = binary_num[9];
	}
	if(bits == 8){
		wpt->b2 = binary_num[0];
		wpt->b3 = binary_num[1];
		wpt->b4 = binary_num[2];
		wpt->b5 = binary_num[3];
		wpt->b6 = binary_num[4];
		wpt->b7 = binary_num[5];
		wpt->b8 = binary_num[6];
		wpt->b9 = binary_num[7];
	}
	free(binary_num);
}

char get_base32_digit(int digit){
	if(digit >= 32 || digit < 0)
		return '\0';
	switch(digit){
		case 0:
			return '!';
		case 1:
			return '@';
		case 2:
			return '#';
		case 3:
			return '$';
		case 4:
			return '%';
		case 5:
			return '^';
		case 6:
			return '&';
		case 7:
			return '*';
		case 8:
			return '<';
		case 9:
			return '>';
		case 10:
			return 'a';
		case 11:
			return 'b';
		case 12:
			return 'c';
		case 13:
			return 'd';
		case 14:
			return 'e';
		case 15:
			return 'f';
		case 16:
			return 'g';
		case 17:
			return 'h';
		case 18:
			return 'i';
		case 19:
			return 'j';
		case 20:
			return 'k';
		case 21:
			return 'l';
		case 22:
			return 'm';
		case 23:
			return 'n';
		case 24:
			return 'o';
		case 25:
			return 'p';
		case 26:
			return 'q';
		case 27:
			return 'r';
		case 28:
			return 's';
		case 29:
			return 't';
		case 30:
			return 'u';
		case 31:
			return 'v';
	}
	return '\0';
}

char* get_word_base32_form(machine_word* wpt){
	int small_half = 0, big_half = 0;
	char* base32_form;
	base32_form = malloc(sizeof(char)*3);
	if(base32_form == NULL)
		return NULL;
	small_half = wpt->b0 + (wpt->b1)*2 + (wpt->b2)*2*2 + (wpt->b3)*2*2*2 + (wpt->b4)*2*2*2*2;
	big_half = wpt->b5 + (wpt->b6)*2 + (wpt->b7)*2*2 + (wpt->b8)*2*2*2 + (wpt->b9)*2*2*2*2;
	base32_form[0] = get_base32_digit(big_half);
	base32_form[1] = get_base32_digit(small_half);
	base32_form[2] = '\0';
	return base32_form;
}

char* get_num_base32_form(int num){
	char* base32_form;
	if(num > 1023)/*won't happen since I am using it only for line counting*/
		return NULL;
	base32_form = malloc(sizeof(char)*3);
	if(base32_form == NULL)
		return NULL;
	base32_form[1] = get_base32_digit(num%32);
	base32_form[0] = get_base32_digit((num/32)%32);
	base32_form[2] = '\0';
	return base32_form;
	
}

int get_num_extra_words(addressing_type src, addressing_type dst){
	if(src < 0){
		if(dst != ADR_STRCT)
			return 1;
		return 2;
	}
	if(src == ADR_REG && dst == ADR_REG)
		return 1;
	if(src == ADR_STRCT && dst == ADR_STRCT)
		return 4;
	if(src == ADR_STRCT || dst == ADR_STRCT)
		return 3;
	return 2;
}

void hold_reg_num(machine_word *wpt, int reg_src, int reg_dest){
	int binary_num[4], i;
	if(reg_src >= 0){
		for(i = 0; i < 4; i++){
			binary_num[i] = reg_src % 2;
			reg_src /= 2;
		}
		wpt->b6 = binary_num[0];
		wpt->b7 = binary_num[1];
		wpt->b8 = binary_num[2];
		wpt->b9 = binary_num[3];
	}
	if(reg_dest >= 0){
		for(i = 0; i < 4; i++){
			binary_num[i] = reg_dest % 2;
			reg_dest /= 2;
		}
		wpt->b2 = binary_num[0];
		wpt->b3 = binary_num[1];
		wpt->b4 = binary_num[2];
		wpt->b5 = binary_num[3];
	}
}
