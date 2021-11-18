#include <xinu.h>
void pagefault_handler()
{
	pd_t* pd_base;
	pt_t* pt_base;
	uint32 pd_index,pt_index;
	uint32 vaddr = (uint32)read_cr2();
	pd_index = (vaddr >> 22) & (0x3FF);
	pt_index = (vaddr >> 12) & (0x3FF);
	pd_base = (pd_t*)(proctab[currpid].pdbr);
	pt_base = (pt_t*)(pd_base[pd_index].pd_base);

	if(proctab[currpid].pages_alloc == proctab[currpid].pages_used){
		kprintf("SEGMENTATION FAULT\n");
		kill(currpid);
	}

	else{
		if(pt_base[pt_index].pt_valid == 1 && pd_base[pd_index].pd_pres == 0){
			char* mem = (char*)getffsmem(4096);
			proctab[currpid].pages_used += 1;
			pt_base[pt_index].pt_pres = 1;
			write_cr3((unsigned int)(proctab[currpid].pdbr));
		}
		else if(pt_base[pt_index].pt_valid == 0){
			kprintf("SEGMENTATION FAULT\n");
			kill(currpid);
		}
	}
	
}

