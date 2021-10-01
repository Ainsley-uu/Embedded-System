ORG    00H
    MOV   DPTR, #TABLE  ;source pointer
    MOV   R0, #30H
    MOV   R1, #0H      ;sum
    MOV    R3, #15      ;counter
    MOV    A, #0H
BACK:   MOVC  A, @A+DPTR    ;get a byte from source
    MOV    @R0,A      
    MOV    R2, A      ;
    MOV    A, R1
    ADD    A, R2      ;addition
    MOV    R1, A      
    INC    R0        ;increment source pointer
    MOV    A, R0
	CLR	   C
    SUBB  A, #30H
    DJNZ  R3, BACK    ;keep doing for 15 bytes
    
    MOV    A, R1      
    XRL    A, #0FFH    ;Negate
    MOV    3FH, A      ;Deposit value into 3F
    
    ORG    30H
TABLE:  DB    27,5,32,47,38,235,79,17,187,58,23,35,211,104,9
    END
