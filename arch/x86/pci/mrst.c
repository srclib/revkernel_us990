/*
 * Moorestown PCI support
 *   Copyright (c) 2008 Intel Corporation
 *     Jesse Barnes <jesse.barnes@intel.com>
 *
 * Moorestown has an interesting PCI implementation:
 *   - configuration space is memory mapped (as defined by MCFG)
 *   - Lincroft devices also have a real, type 1 configuration space
 *   - Early Lincroft silicon has a type 1 access bug that will cause
 *     a hang if non-existent devices are accessed
 *   - some devices have the "fixed BAR" capability, which means
 *     they can't be relocated or modified; check for that during
 *     BAR sizing
 *
 * So, we use the MCFG space for all reads and writes, but also send
 * Lincroft writes to type 1 space.  But only read/write if the device
 * actually exists, otherwise return all 1s for reads and bit bucket
 * the writes.
 */

#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/dmi.h>

#include <asm/acpi.h>
#include <asm/segment.h>
#include <asm/io.h>
#include <asm/smp.h>
#include <asm/pci_x86.h>
#include <asm/hw_irq.h>
#include <asm/io_apic.h>

#define PCIE_CAP_OFFSET	0x100

/*                  */
#define PCIE_VNDR_CAP_ID_FIXED_BAR 0x00	/*                 */
#define PCI_FIXED_BAR_0_SIZE	0x04
#define PCI_FIXED_BAR_1_SIZE	0x08
#define PCI_FIXED_BAR_2_SIZE	0x0c
#define PCI_FIXED_BAR_3_SIZE	0x10
#define PCI_FIXED_BAR_4_SIZE	0x14
#define PCI_FIXED_BAR_5_SIZE	0x1c

static int pci_soc_mode = 0;

/* 
                                                                  
                
                             
  
                                                                      
                           
 */
static int fixed_bar_cap(struct pci_bus *bus, unsigned int devfn)
{
	int pos;
	u32 pcie_cap = 0, cap_data;

	pos = PCIE_CAP_OFFSET;

	if (!raw_pci_ext_ops)
		return 0;

	while (pos) {
		if (raw_pci_ext_ops->read(pci_domain_nr(bus), bus->number,
					  devfn, pos, 4, &pcie_cap))
			return 0;

		if (PCI_EXT_CAP_ID(pcie_cap) == 0x0000 ||
			PCI_EXT_CAP_ID(pcie_cap) == 0xffff)
			break;

		if (PCI_EXT_CAP_ID(pcie_cap) == PCI_EXT_CAP_ID_VNDR) {
			raw_pci_ext_ops->read(pci_domain_nr(bus), bus->number,
					      devfn, pos + 4, 4, &cap_data);
			if ((cap_data & 0xffff) == PCIE_VNDR_CAP_ID_FIXED_BAR)
				return pos;
		}

		pos = PCI_EXT_CAP_NEXT(pcie_cap);
	}

	return 0;
}

static int pci_device_update_fixed(struct pci_bus *bus, unsigned int devfn,
				   int reg, int len, u32 val, int offset)
{
	u32 size;
	unsigned int domain, busnum;
	int bar = (reg - PCI_BASE_ADDRESS_0) >> 2;

	domain = pci_domain_nr(bus);
	busnum = bus->number;

	if (val == ~0 && len == 4) {
		unsigned long decode;

		raw_pci_ext_ops->read(domain, busnum, devfn,
			       offset + 8 + (bar * 4), 4, &size);

		/*                                                         */
		if (size) {
			decode = size - 1;
			decode |= decode >> 1;
			decode |= decode >> 2;
			decode |= decode >> 4;
			decode |= decode >> 8;
			decode |= decode >> 16;
			decode++;
			decode = ~(decode - 1);
		} else {
			decode = 0;
		}

		/*
                                                                 
                                                     
    
                                                                
   */
		return raw_pci_ext_ops->write(domain, busnum, devfn, reg, 4,
				       decode);
	}

	/*                                                      */
	return raw_pci_ext_ops->write(domain, busnum, devfn, reg, len, val);
}

/* 
                                                
                   
                                        
  
                                                                             
                                                                            
                                                                       
           
 */
static bool type1_access_ok(unsigned int bus, unsigned int devfn, int reg)
{
	/*                                                                   
                                                              
   
                                                                        
                                                                     
                                                         
  */
	if (reg >= 0x100 || reg == PCI_STATUS || reg == PCI_HEADER_TYPE)
		return 0;
	if (bus == 0 && (devfn == PCI_DEVFN(2, 0)
				|| devfn == PCI_DEVFN(0, 0)
				|| devfn == PCI_DEVFN(3, 0)))
		return 1;
	return 0; /*                    */
}

static int pci_read(struct pci_bus *bus, unsigned int devfn, int where,
		    int size, u32 *value)
{
	if (type1_access_ok(bus->number, devfn, where))
		return pci_direct_conf1.read(pci_domain_nr(bus), bus->number,
					devfn, where, size, value);
	return raw_pci_ext_ops->read(pci_domain_nr(bus), bus->number,
			      devfn, where, size, value);
}

static int pci_write(struct pci_bus *bus, unsigned int devfn, int where,
		     int size, u32 value)
{
	int offset;

	/*                                                                    
                                           
  */
	if (where == PCI_ROM_ADDRESS)
		return 0;

	/*
                                                  
                                                               
                                                     
                                                                    
  */
	offset = fixed_bar_cap(bus, devfn);
	if (offset &&
	    (where >= PCI_BASE_ADDRESS_0 && where <= PCI_BASE_ADDRESS_5)) {
		return pci_device_update_fixed(bus, devfn, where, size, value,
					       offset);
	}

	/*
                                                   
                                                                
                                                                   
  */
	if (type1_access_ok(bus->number, devfn, where))
		return pci_direct_conf1.write(pci_domain_nr(bus), bus->number,
					      devfn, where, size, value);
	return raw_pci_ext_ops->write(pci_domain_nr(bus), bus->number, devfn,
			       where, size, value);
}

static int mrst_pci_irq_enable(struct pci_dev *dev)
{
	u8 pin;
	struct io_apic_irq_attr irq_attr;

	pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin);

	/*                                                           
                                                             
  */
	irq_attr.ioapic = mp_find_ioapic(dev->irq);
	irq_attr.ioapic_pin = dev->irq;
	irq_attr.trigger = 1; /*       */
	irq_attr.polarity = 1; /*            */
	io_apic_set_pci_routing(&dev->dev, dev->irq, &irq_attr);

	return 0;
}

struct pci_ops pci_mrst_ops = {
	.read = pci_read,
	.write = pci_write,
};

/* 
                                        
  
                                                                
                                                        
 */
int __init pci_mrst_init(void)
{
	printk(KERN_INFO "Intel MID platform detected, using MID PCI ops\n");
	pci_mmcfg_late_init();
	pcibios_enable_irq = mrst_pci_irq_enable;
	pci_root_ops = pci_mrst_ops;
	pci_soc_mode = 1;
	/*                             */
	return 1;
}

/*                                                                         
                                       
 */
static void __devinit pci_d3delay_fixup(struct pci_dev *dev)
{
	/*                                                                   
                                                                     */
        if (!pci_soc_mode)
            return;
	/*                                                                  
                                  
  */
	if (type1_access_ok(dev->bus->number, dev->devfn, PCI_DEVICE_ID))
		return;
	dev->d3_delay = 0;
}
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_INTEL, PCI_ANY_ID, pci_d3delay_fixup);

static void __devinit mrst_power_off_unused_dev(struct pci_dev *dev)
{
	pci_set_power_state(dev, PCI_D3cold);
}
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_INTEL, 0x0801, mrst_power_off_unused_dev);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_INTEL, 0x0809, mrst_power_off_unused_dev);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_INTEL, 0x080C, mrst_power_off_unused_dev);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_INTEL, 0x0812, mrst_power_off_unused_dev);
DECLARE_PCI_FIXUP_FINAL(PCI_VENDOR_ID_INTEL, 0x0815, mrst_power_off_unused_dev);

/*
                                                                    
 */
static void __devinit pci_fixed_bar_fixup(struct pci_dev *dev)
{
	unsigned long offset;
	u32 size;
	int i;

	if (!pci_soc_mode)
		return;

	/*                                        */
	if (dev->cfg_size < PCIE_CAP_OFFSET + 4)
		return;

	/*                                                                    */
	offset = fixed_bar_cap(dev->bus, dev->devfn);
	if (!offset || PCI_DEVFN(2, 0) == dev->devfn ||
	    PCI_DEVFN(2, 2) == dev->devfn)
		return;

	for (i = 0; i < PCI_ROM_RESOURCE; i++) {
		pci_read_config_dword(dev, offset + 8 + (i * 4), &size);
		dev->resource[i].end = dev->resource[i].start + size - 1;
		dev->resource[i].flags |= IORESOURCE_PCI_FIXED;
	}
}
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_INTEL, PCI_ANY_ID, pci_fixed_bar_fixup);