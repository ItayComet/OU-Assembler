;first error, no parameters
.data
;This line is tooooooo longgggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg
;second error, unknown instruction
dov S1.1, W
		add r2,STR
S2:		.struct -1	 , 	 "12345"

Wrong_tag_syntax: 	jmp W
		macro m1
		inc K
		; a comment
This line should be a comment, however it is an error
		mov Wrong_struct_syntax.2 ,	r4	
;r9 is not a register name
		endmacro
r5:	prn 	#-50
prn: mov #1, r2
;a saved word can't be a tag

mov 1, r2
		sub r1	, r4
		m1
		bne #3
END:	hlt
STR:	.string "abcdef"
LENGTH:	.data 6,-512,511333333333
S1:		.struct 8, "ab"
.extern wrong_extern_synt
