;testing error catching in the second pass
.entry STR
.extern R4
MAIN: mov S1.1 ,LENGTH
add r2,STR
LOOP: jmp END
macro m1
inc R4
mov UndeclaredStruct.2 ,r3
endmacro
prn #6555555555555555555555555
;this number is too big
sub r2, r4
m1
bne UndeclaredTag
END: hlt
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data -22
S1: .struct 8, "ab"
