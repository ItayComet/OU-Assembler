#ifndef MACHINE_H/*inclusion guard*/
#define MACHINE_H

#define MEMORY_SPACE 156/*256-100 since we don't use the first 100 memory cell*/
#define FIRST_MEMORY_ADDRESS 100

/*encoding types*/
typedef enum encoding_types{
	ENC_ABS,
	ENC_EXT,
	ENC_REL,
	ENC_ERROR = -1
} encoding_type;

/*addressing types*/
typedef enum addressing_types{
	ADR_IMM,/*immediate addressing*/
	ADR_DIR,/*direct addressing*/
	ADR_STRCT,/*access to struct addressing*/
	ADR_REG,/*register addressing*/
	ADR_ERROR = -1
	
} addressing_type;

typedef enum instructions{
	INST_MOV,INST_CMP,INST_ADD, INST_SUB,
	INST_NOT,INST_CLR, INST_LEA,INST_INC,INST_DEC,INST_JMP,
	INST_BNE,INST_GET,INST_PRN,INST_JSR,INST_RTS,INST_HLT, INST_ERROR = -1
} instruction_type;

typedef enum directives{
	DIR_DATA, DIR_STR, DIR_STRCT, DIR_EXT, DIR_ENT, DIR_ERROR = -1
} directive_type;

/*each one of the fields represents a bit in a machine word, (b stands for bit and the number is its place)*/
typedef struct machine_word{
	unsigned int b0 : 1;
	unsigned int b1 : 1;
	unsigned int b2 : 1;
	unsigned int b3 : 1;
	unsigned int b4 : 1;
	unsigned int b5 : 1;
	unsigned int b6 : 1;
	unsigned int b7 : 1;
	unsigned int b8 : 1;
	unsigned int b9 : 1;
} machine_word;

/*creates a new machine_word and returns its address, or NULL in case of an allocation failure, all of the bits are set to 0*/
machine_word* new_machine_word();

/*sets the encoding type (changes the first 2 bits)
	@param wpt: a pointer to the machine_word
	@param type: desired encoding type for wpt
*/
void set_encoding_type(machine_word* wpt, encoding_type type);

/*sets the addressing type for the destination operator in the first word of an instruction
	@param wpt: a pointer to the desired machine_word
	@param type: addressing type for the destination operator
*/
void set_adressing_dest(machine_word* wpt, addressing_type type);

/*sets the addressing type for the source operator in the first word of an instruction
	@param wpt: a pointer to the desired machine_word
	@param type: addressing type for the source operator
*/
void set_adressing_src(machine_word* wpt, addressing_type type);

/*sets the instruction type in the first word of an instruction
	@param wpt: a pointer to the desired machine_word
	@param inst: the instruction's number
*/
void set_instruction_type(machine_word* wpt, instruction_type inst);

/*sets the bits of a word to be a binary representation of num
if the number of bits is 10, the word will be filled, if the number is 8, the number will be set in the 8 leftmost bits (b1 and b0 won't be changed)
the function is undefined for bits other than 8 or 10
	@param wpt: a pointer to the desired machine_word
	@param num: an integer with the number
	@param bits: the number of bits, 8 or 10
*/
void hold_num(machine_word *wpt, int num, int bits);

/*When given a number between 0 and 31, returns its symbol in the base 32 numeral system
	@param digit: the number
*/
char get_base32_digit(int digit);

/*This function recieves a pointer to a machine words and returns the number it holds in a base 32 numeral system representation (the returned value is an allocated string)
	@param wpt: a pointer to the machine word
*/
char* get_word_base32_form(machine_word* wpt);

/*This function recieves an integer and returns the number it holds in a base 32 numeral system representation, if it's bigger than 1023 it returns NULL (the returned value is an allocated string)
	@param num: the number to convert
*/
char* get_num_base32_form(int num);

/*This function gets the addressing type of the source and destination in an instruction line and returns the extra number of words needed to be coded for this line (all of the words minus the first word)
	@param src: the addressing type of the source, if there is no source, you can fill -1
	@param dest: the addressing type of the destination
*/
int get_num_extra_words(addressing_type src, addressing_type dst);

/*Fills a machine word with the number of registers, if the users want to fill only the source or dest register, they can fill -1 in the other
	@param wpt: a pointer to the machine word to change
	@param reg_src: the number of the source register
	@param reg_dest: the number of the destination register
*/
void hold_reg_num(machine_word *wpt, int reg_src, int reg_dest);

/*This function returns the number held in the word as a binary number, used for debugging*/
char* word_to_str(machine_word* wpt);

#endif
