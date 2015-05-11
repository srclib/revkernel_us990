#ifndef __ASM_SH_PROCESSOR_H
#define __ASM_SH_PROCESSOR_H

#include <asm/cpu-features.h>
#include <asm/segment.h>
#include <asm/cache.h>

#ifndef __ASSEMBLY__
/*
                                                                  
  
                                                               
                                                                        
                                                 
 */
enum cpu_type {
	/*            */
	CPU_SH7619,

	/*             */
	CPU_SH7201, CPU_SH7203, CPU_SH7206, CPU_SH7263, CPU_MXG,

	/*            */
	CPU_SH7705, CPU_SH7706, CPU_SH7707,
	CPU_SH7708, CPU_SH7708S, CPU_SH7708R,
	CPU_SH7709, CPU_SH7709A, CPU_SH7710, CPU_SH7712,
	CPU_SH7720, CPU_SH7721, CPU_SH7729,

	/*            */
	CPU_SH7750, CPU_SH7750S, CPU_SH7750R, CPU_SH7751, CPU_SH7751R,
	CPU_SH7760, CPU_SH4_202, CPU_SH4_501,

	/*             */
	CPU_SH7763, CPU_SH7770, CPU_SH7780, CPU_SH7781, CPU_SH7785, CPU_SH7786,
	CPU_SH7723, CPU_SH7724, CPU_SH7757, CPU_SHX3,

	/*                 */
	CPU_SH7343, CPU_SH7722, CPU_SH7366, CPU_SH7372,

	/*            */
        CPU_SH5_101, CPU_SH5_103,

	/*                 */
	CPU_SH_NONE
};

enum cpu_family {
	CPU_FAMILY_SH2,
	CPU_FAMILY_SH2A,
	CPU_FAMILY_SH3,
	CPU_FAMILY_SH4,
	CPU_FAMILY_SH4A,
	CPU_FAMILY_SH4AL_DSP,
	CPU_FAMILY_SH5,
	CPU_FAMILY_UNKNOWN,
};

/*
                            
  
                                               
 */
struct tlb_info {
	unsigned long long next;
	unsigned long long first;
	unsigned long long last;

	unsigned int entries;
	unsigned int step;

	unsigned long flags;
};

struct sh_cpuinfo {
	unsigned int type, family;
	int cut_major, cut_minor;
	unsigned long loops_per_jiffy;
	unsigned long asid_cache;

	struct cache_info icache;	/*                 */
	struct cache_info dcache;	/*                 */
	struct cache_info scache;	/*                 */

	/*          */
	struct tlb_info itlb;
	struct tlb_info dtlb;

#ifdef CONFIG_SMP
	struct task_struct *idle;
#endif

	unsigned int phys_bits;
	unsigned long flags;
} __attribute__ ((aligned(L1_CACHE_BYTES)));

extern struct sh_cpuinfo cpu_data[];
#define boot_cpu_data cpu_data[0]
#define current_cpu_data cpu_data[smp_processor_id()]
#define raw_current_cpu_data cpu_data[raw_smp_processor_id()]

#define cpu_sleep()	__asm__ __volatile__ ("sleep" : : : "memory")
#define cpu_relax()	barrier()

void default_idle(void);
void cpu_idle_wait(void);
void stop_this_cpu(void *);

/*              */
struct seq_operations;
struct task_struct;

extern struct pt_regs fake_swapper_regs;

extern void cpu_init(void);
extern void cpu_probe(void);

/*                          */
extern unsigned int xstate_size;
extern void free_thread_xstate(struct task_struct *);
extern struct kmem_cache *task_xstate_cachep;

/*                        */
extern int get_unalign_ctl(struct task_struct *, unsigned long addr);
extern int set_unalign_ctl(struct task_struct *, unsigned int val);

#define GET_UNALIGN_CTL(tsk, addr)	get_unalign_ctl((tsk), (addr))
#define SET_UNALIGN_CTL(tsk, val)	set_unalign_ctl((tsk), (val))

/*                   */
extern unsigned int mem_init_done;

/*                        */
const char *get_cpu_subtype(struct sh_cpuinfo *c);
extern const struct seq_operations cpuinfo_op;

/*                     */
#define SH_THREAD_UAC_NOPRINT	(1 << 0)
#define SH_THREAD_UAC_SIGBUS	(1 << 1)
#define SH_THREAD_UAC_MASK	(SH_THREAD_UAC_NOPRINT | SH_THREAD_UAC_SIGBUS)

/*                                   */
#define MODE_PIN0 (1 << 0)
#define MODE_PIN1 (1 << 1)
#define MODE_PIN2 (1 << 2)
#define MODE_PIN3 (1 << 3)
#define MODE_PIN4 (1 << 4)
#define MODE_PIN5 (1 << 5)
#define MODE_PIN6 (1 << 6)
#define MODE_PIN7 (1 << 7)
#define MODE_PIN8 (1 << 8)
#define MODE_PIN9 (1 << 9)
#define MODE_PIN10 (1 << 10)
#define MODE_PIN11 (1 << 11)
#define MODE_PIN12 (1 << 12)
#define MODE_PIN13 (1 << 13)
#define MODE_PIN14 (1 << 14)
#define MODE_PIN15 (1 << 15)

int generic_mode_pins(void);
int test_mode_pin(int pin);

#ifdef CONFIG_VSYSCALL
int vsyscall_init(void);
#else
#define vsyscall_init() do { } while (0)
#endif

/*
                                                                 
 */
#ifdef CONFIG_CPU_SH2A
extern unsigned int instruction_size(unsigned int insn);
#elif defined(CONFIG_SUPERH32)
#define instruction_size(insn)	(2)
#else
#define instruction_size(insn)	(4)
#endif

#endif /*              */

#ifdef CONFIG_SUPERH32
# include "processor_32.h"
#else
# include "processor_64.h"
#endif

#endif /*                      */