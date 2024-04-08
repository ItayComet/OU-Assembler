assembler: line_utils.o hash_table.o machine_utils.o first_pass.o second_pass.o assembler.o file_creator.o pre_assembler.o macros.o
	gcc -g -Wall -ansi -pedantic line_utils.o hash_table.o machine_utils.o second_pass.o first_pass.o assembler.o file_creator.o pre_assembler.o macros.o -o assembler

macros.o: macros.c
	gcc -c -g -Wall -ansi -pedantic macros.c -o macros.o

pre_assembler.o: pre_assembler.c
	gcc -c -g -Wall -ansi -pedantic pre_assembler.c -o pre_assembler.o

assembler.o: assembler.c
	gcc -c -g -Wall -ansi -pedantic assembler.c -o assembler.o

second_pass.o: second_pass.c
	gcc -c -g -Wall -ansi -pedantic second_pass.c -o second_pass.o 

file_creator.o: file_creator.c
	gcc -c -g -Wall -ansi -pedantic file_creator.c -o file_creator.o 

first_pass.o: first_pass.c
	gcc -c -g -Wall -ansi -pedantic first_pass.c -o first_pass.o
	
line_utils.o: line_utils.c
	gcc -c -g -Wall -ansi -pedantic line_utils.c -o line_utils.o
	
machine_utils.o: machine_utils.c
	gcc -c -g -Wall -ansi -pedantic machine_utils.c -o machine_utils.o

hash_table.o: hash_table.c
	gcc -c -g -Wall -ansi -pedantic hash_table.c -o hash_table.o
