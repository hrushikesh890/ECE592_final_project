#include <xinu.h>
void pagefault_handler()
{
	intmask mask;
	pd_t* pd_base;
	pt_t* pt_base;
	uint32 pd_index, pt_index;
	uint32 vaddr = (uint32)read_cr2();
	write_cr3((unsigned long) PT_START);
	pd_index = (vaddr >> 22) & (0x3FF);
	pt_index = (vaddr >> 12) & (0x3FF);
	kprintf("In pagefault 1.1\n");
	pd_base = (pd_t*)(proctab[currpid].pdbr);
	//pd_base = ((uint32)(pd_base) << 12);
	pt_base = (pt_t*)(pd_base[pd_index].pd_base);
	pt_base = ((uint32)(pt_base) << 12);
	kprintf("In pagefault 2.1\n");
	kprintf("pd_base:0x%x pd_index:%d pt_base:0x%x pt_index:%d\n",(uint32)pd_base,pd_index,(uint32)pt_base,pt_index);
	kprintf("pd_base:0x%x\n",pd_base[pd_index].pd_base);
	
	if(pt_base[pt_index].pt_valid == 1 && pt_base[pt_index].pt_pres == 0){
		//kprintf("In pagefault 2.2\n");
		uint32 mem = (uint32)getffsmem(4096);
		//kprintf("In pagefault 2\n");
		proctab[currpid].pages_used += 1;
		pt_base[pt_index].pt_pres = 1;
		pt_base[pt_index].pt_base = (mem >> 12);
		write_cr3((unsigned long)(proctab[currpid].pdbr));
	}
	else if(pt_base[pt_index].pt_valid == 0){
		kprintf("P%d::SEGMENTATION FAULT\n", currpid);
		kill(currpid);
	}
	
	write_cr3((unsigned long) proctab[currpid].pdbr);
}

