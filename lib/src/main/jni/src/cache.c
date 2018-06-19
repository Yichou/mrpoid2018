#include "mr_types.h"



#if 0
void mono_arch_flush_icache (uint8 *code, int size)
{
	__asm __volatile ("mov r0, %0\n"
		"mov r1, %1\n"
		"mov r2, %2\n"
		"swi 0x9f0002       @ sys_cacheflush"
		: /* no outputs */
	: "r" (code), "r" (code + size), "r" (0)
		: "r0", "r1", "r3" );
}

int sys_cacheflush(void * beg, void * end, int flag)
{
	register unsigned long _beg __asm ("a1") = (unsigned long) beg;
	register unsigned long _end __asm ("a2") = (unsigned long) end;
	register unsigned long _flg __asm ("a3") = flag;


	__asm __volatile ("swi 0x9f0002        @ sys_cacheflush" :
	"=r" (_beg) :
	"0" (_beg),
		"r" (_end),
		"r" (_flg));


	return _beg;
}
#endif
