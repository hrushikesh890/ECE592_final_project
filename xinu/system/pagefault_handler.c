#include <xinu.h>
void pagefault_handler()
{

	intmask mask;
	pd_t* pd_base;
	pt_t* pt_base;
	uint32 pd_index, pt_index;
	char* mem;
	
	mask = disable();
	uint32 vaddr = (uint32)read_cr2();
	write_cr3((unsigned long) PT_START);
	
	pd_index = (vaddr >> 22) & (0x3FF);
	pt_index = (vaddr >> 12) & (0x3FF);
	pd_base = (pd_t*)(proctab[currpid].pdbr);
	pt_base = (pt_t*)(pd_base[pd_index].pd_base<<12);

	
	if((pt_base[pt_index].pt_valid == 1) && (pd_base[pd_index].pd_pres == 1) && (pt_base[pt_index].pt_pres == 0)){
		if (free_ffs_pages() > 0)
		{			
			mem = getffsmem(4096);
			write_cr3((unsigned long) PT_START);
			proctab[currpid].pages_used += 1;
			pt_base[pt_index].pt_pres = 1;
			pt_base[pt_index].pt_base = ((uint32)mem >> 12);
		}
		else
		{
			swapping(pt_index, pd_index);
		}
		
	}

	else
	{
		kprintf("P%d:: SEGMENTATION_FAULT\n", currpid);
		kill(currpid);
	}

	char* ptr = (char*) vaddr;
	
	write_cr3((unsigned long) proctab[currpid].pdbr);
	restore(mask);
}

