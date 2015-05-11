#ifndef __ALPHA_MACHVEC_H
#define __ALPHA_MACHVEC_H 1

#include <linux/types.h>

/*
                                                                 
                              
 */
 
#ifdef __KERNEL__

/*                                                                    
                                                              */

struct task_struct;
struct mm_struct;
struct vm_area_struct;
struct linux_hose_info;
struct pci_dev;
struct pci_ops;
struct pci_controller;
struct _alpha_agp_info;
struct rtc_time;

struct alpha_machine_vector
{
	/*                                                       
                                                         
                                                   */
	unsigned long hae_cache;
	unsigned long *hae_register;

	int nr_irqs;
	int rtc_port;
	unsigned int max_asn;
	unsigned long max_isa_dma_address;
	unsigned long irq_probe_mask;
	unsigned long iack_sc;
	unsigned long min_io_address;
	unsigned long min_mem_address;
	unsigned long pci_dac_offset;

	void (*mv_pci_tbi)(struct pci_controller *hose,
			   dma_addr_t start, dma_addr_t end);

	unsigned int (*mv_ioread8)(void __iomem *);
	unsigned int (*mv_ioread16)(void __iomem *);
	unsigned int (*mv_ioread32)(void __iomem *);

	void (*mv_iowrite8)(u8, void __iomem *);
	void (*mv_iowrite16)(u16, void __iomem *);
	void (*mv_iowrite32)(u32, void __iomem *);

	u8 (*mv_readb)(const volatile void __iomem *);
	u16 (*mv_readw)(const volatile void __iomem *);
	u32 (*mv_readl)(const volatile void __iomem *);
	u64 (*mv_readq)(const volatile void __iomem *);

	void (*mv_writeb)(u8, volatile void __iomem *);
	void (*mv_writew)(u16, volatile void __iomem *);
	void (*mv_writel)(u32, volatile void __iomem *);
	void (*mv_writeq)(u64, volatile void __iomem *);

	void __iomem *(*mv_ioportmap)(unsigned long);
	void __iomem *(*mv_ioremap)(unsigned long, unsigned long);
	void (*mv_iounmap)(volatile void __iomem *);
	int (*mv_is_ioaddr)(unsigned long);
	int (*mv_is_mmio)(const volatile void __iomem *);

	void (*mv_switch_mm)(struct mm_struct *, struct mm_struct *,
			     struct task_struct *);
	void (*mv_activate_mm)(struct mm_struct *, struct mm_struct *);

	void (*mv_flush_tlb_current)(struct mm_struct *);
	void (*mv_flush_tlb_current_page)(struct mm_struct * mm,
					  struct vm_area_struct *vma,
					  unsigned long addr);

	void (*update_irq_hw)(unsigned long, unsigned long, int);
	void (*ack_irq)(unsigned long);
	void (*device_interrupt)(unsigned long vector);
	void (*machine_check)(unsigned long vector, unsigned long la);

	void (*smp_callin)(void);
	void (*init_arch)(void);
	void (*init_irq)(void);
	void (*init_rtc)(void);
	void (*init_pci)(void);
	void (*kill_arch)(int);

	u8 (*pci_swizzle)(struct pci_dev *, u8 *);
	int (*pci_map_irq)(const struct pci_dev *, u8, u8);
	struct pci_ops *pci_ops;

	struct _alpha_agp_info *(*agp_info)(void);

	unsigned int (*rtc_get_time)(struct rtc_time *);
	int (*rtc_set_time)(struct rtc_time *);

	const char *vector_name;

	/*                  */
	int (*pa_to_nid)(unsigned long);
	int (*cpuid_to_nid)(int);
	unsigned long (*node_mem_start)(int);
	unsigned long (*node_mem_size)(int);

	/*                              */
	union {
	    struct {
		unsigned long gru_int_req_bits;
	    } cia;

	    struct {
		unsigned long gamma_bias;
	    } t2;

	    struct {
		unsigned int route_tab;
	    } sio;
	} sys;
};

extern struct alpha_machine_vector alpha_mv;

#ifdef CONFIG_ALPHA_GENERIC
extern int alpha_using_srm;
#else
#ifdef CONFIG_ALPHA_SRM
#define alpha_using_srm 1
#else
#define alpha_using_srm 0
#endif
#endif /*         */

#endif
#endif /*                   */