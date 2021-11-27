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

	inv_pt* ipt = (inv_pt *)(INV_TABLE_START);

	// if((pd_index == 24) || (pd_index == 57)){
	// kprintf("pd_base:%x pt_base:%x",pd_base,pt_base);
	// kprintf("pd_index:%d pt_index:%d",pd_index,pt_index);
	// kprintf("pt_valid:%d pd_pres:%d pt_pres:%d\n",pt_base[pt_index].pt_valid,pd_base[pd_index].pd_pres,pt_base[pt_index].pt_pres);
	
	// }
	
	if((pt_base[pt_index].pt_valid == 1) && (pd_base[pd_index].pd_pres == 1) && (pt_base[pt_index].pt_pres == 0)){

		// if(pd_index == 24){
		// 	kprintf("Reached inside if\n");	
		// }

		if (free_ffs_pages() > 0)
		{			
			mem = getffsmem(4096);
			write_cr3((unsigned long) PT_START);
			proctab[currpid].pages_used += 1;
			pt_base[pt_index].pt_pres = 1;
			pt_base[pt_index].pt_base = ((uint32)mem >> 12);
			int inv;
			for(inv = 0;inv < 16*1024;inv++){
				if(ipt[inv].proc_pid == 0){
					ipt[inv].proc_pid = currpid;
					ipt[inv].pd_offset = pd_index;
					ipt[inv].pt_offset = pt_index;
					ipt[inv].swp_addr = 0;
				}
			}
		}
		// if(pd_index == 24){
		// 	kprintf("*Reached inside if\n");	
		// }
		else
		{
			kprintf("ELSE CALLED\n");
			swapping(pt_index, pd_index);
			write_cr3((unsigned long) PT_START);
		}

		
		
	}

	// if(pd_index == 24){
	// 	kprintf("**Reached inside if\n");	
	// }

	else
	{
	// 	kprintf("pd_base:%x pt_base:%x",pd_base,pt_base);
	// kprintf("pd_index:%d pt_index:%d",pd_index,pt_index);
	// kprintf("pt_valid:%d pd_pres:%d pt_pres:%d\n",pt_base[pt_index].pt_valid,pd_base[pd_index].pd_pres,pt_base[pt_index].pt_pres);
		kprintf("P%d:: SEGMENTATION_FAULT\n", currpid);
		kill(currpid);
	}

	char* ptr = (char*) vaddr;
	
	write_cr3((unsigned long) proctab[currpid].pdbr);
	restore(mask);
}

