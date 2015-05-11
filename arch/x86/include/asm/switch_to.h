#ifndef _ASM_X86_SWITCH_TO_H
#define _ASM_X86_SWITCH_TO_H

struct task_struct; /*                                                       */
struct task_struct *__switch_to(struct task_struct *prev,
				struct task_struct *next);
struct tss_struct;
void __switch_to_xtra(struct task_struct *prev_p, struct task_struct *next_p,
		      struct tss_struct *tss);

#ifdef CONFIG_X86_32

#ifdef CONFIG_CC_STACKPROTECTOR
#define __switch_canary							\
	"movl %P[task_canary](%[next]), %%ebx\n\t"			\
	"movl %%ebx, "__percpu_arg([stack_canary])"\n\t"
#define __switch_canary_oparam						\
	, [stack_canary] "=m" (stack_canary.canary)
#define __switch_canary_iparam						\
	, [task_canary] "i" (offsetof(struct task_struct, stack_canary))
#else	/*                   */
#define __switch_canary
#define __switch_canary_oparam
#define __switch_canary_iparam
#endif	/*                   */

/*
                                                                       
                                                                     
 */
#define switch_to(prev, next, last)					\
do {									\
	/*         
                                                             
                                                    
                                                              
                                                                
                         
  */								\
	unsigned long ebx, ecx, edx, esi, edi;				\
									\
	asm volatile("pushfl\n\t"		/*               */	\
		     "pushl %%ebp\n\t"		/*               */	\
		     "movl %%esp,%[prev_sp]\n\t"	/*               */ \
		     "movl %[next_sp],%%esp\n\t"	/*               */ \
		     "movl $1f,%[prev_ip]\n\t"	/*               */	\
		     "pushl %[next_ip]\n\t"	/*               */	\
		     __switch_canary					\
		     "jmp __switch_to\n"	/*               */	\
		     "1:\t"						\
		     "popl %%ebp\n\t"		/*               */	\
		     "popfl\n"			/*               */	\
									\
		     /*                   */				\
		     : [prev_sp] "=m" (prev->thread.sp),		\
		       [prev_ip] "=m" (prev->thread.ip),		\
		       "=a" (last),					\
									\
		       /*                             */		\
		       "=b" (ebx), "=c" (ecx), "=d" (edx),		\
		       "=S" (esi), "=D" (edi)				\
		       							\
		       __switch_canary_oparam				\
									\
		       /*                   */				\
		     : [next_sp]  "m" (next->thread.sp),		\
		       [next_ip]  "m" (next->thread.ip),		\
		       							\
		       /*                                       */	\
		       [prev]     "a" (prev),				\
		       [next]     "d" (next)				\
									\
		       __switch_canary_iparam				\
									\
		     : /*                            */			\
			"memory");					\
} while (0)

#else /*               */

/*                                          */
#define SAVE_CONTEXT    "pushf ; pushq %%rbp ; movq %%rsi,%%rbp\n\t"
#define RESTORE_CONTEXT "movq %%rbp,%%rsi ; popq %%rbp ; popf\t"

#define __EXTRA_CLOBBER  \
	, "rcx", "rbx", "rdx", "r8", "r9", "r10", "r11", \
	  "r12", "r13", "r14", "r15"

#ifdef CONFIG_CC_STACKPROTECTOR
#define __switch_canary							  \
	"movq %P[task_canary](%%rsi),%%r8\n\t"				  \
	"movq %%r8,"__percpu_arg([gs_canary])"\n\t"
#define __switch_canary_oparam						  \
	, [gs_canary] "=m" (irq_stack_union.stack_canary)
#define __switch_canary_iparam						  \
	, [task_canary] "i" (offsetof(struct task_struct, stack_canary))
#else	/*                   */
#define __switch_canary
#define __switch_canary_oparam
#define __switch_canary_iparam
#endif	/*                   */

/*                                               */
#define switch_to(prev, next, last) \
	asm volatile(SAVE_CONTEXT					  \
	     "movq %%rsp,%P[threadrsp](%[prev])\n\t" /*          */	  \
	     "movq %P[threadrsp](%[next]),%%rsp\n\t" /*             */	  \
	     "call __switch_to\n\t"					  \
	     "movq "__percpu_arg([current_task])",%%rsi\n\t"		  \
	     __switch_canary						  \
	     "movq %P[thread_info](%%rsi),%%r8\n\t"			  \
	     "movq %%rax,%%rdi\n\t" 					  \
	     "testl  %[_tif_fork],%P[ti_flags](%%r8)\n\t"		  \
	     "jnz   ret_from_fork\n\t"					  \
	     RESTORE_CONTEXT						  \
	     : "=a" (last)					  	  \
	       __switch_canary_oparam					  \
	     : [next] "S" (next), [prev] "D" (prev),			  \
	       [threadrsp] "i" (offsetof(struct task_struct, thread.sp)), \
	       [ti_flags] "i" (offsetof(struct thread_info, flags)),	  \
	       [_tif_fork] "i" (_TIF_FORK),			  	  \
	       [thread_info] "i" (offsetof(struct task_struct, stack)),   \
	       [current_task] "m" (current_task)			  \
	       __switch_canary_iparam					  \
	     : "memory", "cc" __EXTRA_CLOBBER)

#endif /*               */

#endif /*                      */