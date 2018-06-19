 AREA    Divarth, CODE, READONLY
       
        EXPORT __rt_udiv 
        EXPORT __rt_sdiv
;/***************************************************************/
;求除数需要移位的次数，为简化除法
;/***************************************************************/
Myclz
        mov r1,#32
Myclzstr
        cmp r0,#0
        bmi Myclzret
        add r0,r0,r0
        subs r1,r1,#1
        bne Myclzstr
Myclzret
        rsb r0,r1,#32
        mov pc,lr
;/***************************************************************/
; name : __rt_udiv
 
; in : r0 除数
;      r1 被除数
; out：r0  = r1/r0
;      r1  = r1%r0
;/***************************************************************/
__rt_udiv
          cmp  r0, #0
          IMPORT  __rt_div0  ;除数为零函数由c语言编写
          beq  __rt_div0
          cmp  r0, r1
          movhi r0, #0
          movhi pc,lr
          stmfd sp!, { r4-r7,lr}
          mov  r4, r1
          mov  r5, r0
          bl Myclz
          mov r7, r0
          mov r0,r4
          bl Myclz
          sub r7,r7,r0
          mov r5,r5,lsl r7
          mov  r0,#0
__rt_udiv_1
          add r0,r0,r0
          subs r1,r4,r5
          movhs r4,r1
          addhs r0,r0, #1
          mov   r5,r5,lsr #1
          subs  r7,r7,#1
          bcs  __rt_udiv_1
          mov r1,r4
          ldmfd sp!,{r4-r7,pc}
;/***************************************************************/
; name : __rt_sdiv
 
; in : r0 除数
;      r1 被除数
; out：r0  = r1/r0
;      r1  = r1%r0
;/***************************************************************/
__rt_sdiv  
          mov r2,#0
          cmp r0,#0
          addlt r2,r2,#1
          rsblt r0,r0,#0
          cmp  r1,#0
          addlt r2,r2,#2
          rsblt r1,r1,#0
          cmp r2,#0
          beq __rt_udiv
          stmfd sp!,{r2,lr}
          bl __rt_udiv
          ldmfd sp!,{r2}
          cmp r2,#2
          rsble r0,r0,#0
          rsbge r1,r1,#0
          ldmfd sp!, {pc}
        END
