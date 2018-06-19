



#define _JBLEN 32

typedef int jmp_buf[_JBLEN];

//extern void mr_longjmp(jmp_buf __jmpb);
//extern int mr_setjmp(jmp_buf __jmpb);

extern void mr_longjmp(void);
extern void mr_setjmp(void);

