#include <xinu.h>
void pagefault_handler()
{

	intmask mask;
	pd_t* pd_base;
	pt_t* pt_base;
	uint32 pd_index, pt_index;
	char* mem;
	
	mask = disable();
	write_cr3((unsigned long) PT_START);
	uint32 vaddr = (uint32)read_cr2();
	pd_index = (vaddr >> 22) & (0x3FF);
	pt_index = (vaddr >> 12) & (0x3FF);
	//kprintf("In pagefault 1.1\n");
	pd_base = (pd_t*)(proctab[currpid].pdbr);
	//pd_base = ((uint32)(pd_base) << 12);
	pt_base = (pt_t*)(pd_base[pd_index].pd_base<<12);

	if(vaddr == 96468992){
		kprintf("IN PAGE FAULT HANDLER\n");
		kprintf("curr pdbr :%x\n",proctab[currpid].pdbr);
		kprintf("1. vaddr:0x%x pd_index:%d pt_index:%d cr3:%x pd_base:%x\n",vaddr,pd_index,pt_index,(uint32)read_cr3(),&pd_base[0]);
		kprintf("2. pd_pres:%d pt_valid:%d pt_pres:%d\n",pd_base[pd_index].pd_pres,pt_base[pt_index].pt_valid,pt_base[pt_index].pt_pres);
	}
	//pt_base = ((uint32)(pt_base) << 12);
	//kprintf("In pagefault 2.1\n");
	//kprintf("curr pdbr :%x\n",proctab[currpid].pdbr);
	//kprintf("1. vaddr:0x%x pd_index:%d pt_index:%d cr3:%x pd_base:%x\n",vaddr,pd_index,pt_index,(uint32)read_cr3(),&pd_base[0]);
	//kprintf("2. pd_pres:%d pt_valid:%d pt_pres:%d\n",pd_base[pd_index].pd_pres,pt_base[pt_index].pt_valid,pt_base[pt_index].pt_pres);
	if((pt_base[pt_index].pt_valid == 1) && (pd_base[pd_index].pd_pres == 1) && (pt_base[pt_index].pt_pres == 0)){
		//kprintf("In pagefault 2.2\n");
		mem = getffsmem(4096);
		write_cr3((unsigned long) PT_START);
		//kprintf("In pagefault 2\n");
		proctab[currpid].pages_used += 1;
		//kprintf("3. pd_base:0x%x pt_base:0x%x\n",(uint32)&pd_base[pd_index],(uint32)&pt_base[pt_index]);
		//kprintf("4. **address:0x%x, pd_index:%d, pt_index:%d\n", vaddr,pd_index,pt_index);
		//kprintf("Mem: %x\n",(uint32)mem);
		//kprintf("pt_pres:%d\n",pt_base[pt_index].pt_pres);
		pt_base[pt_index].pt_pres = 1;
		//pt_base[pt_index].pt_base = (mem >> 12);
		
		pt_base[pt_index].pt_base = ((uint32)mem >> 12);
		if(vaddr == 96468992){
			kprintf("pres:%d write:%d valid:%d\n",pt_base[pt_index].pt_pres,pt_base[pt_index].pt_write,pt_base[pt_index].pt_valid);
		}
		//kprintf("5. pd_base:0x%x pt_base:0x%x\n",(uint32)&pd_base[pd_index],(uint32)&pt_base[pt_index]);
		//kprintf("6. **address:0x%x, pd_index:%d, pt_index:%d\n", vaddr,pd_index,pt_index);
		//write_cr3((unsigned long)(proctab[currpid].pdbr));
		
	}

	else{
		//kprintf("7. address:0x%x, pd_index:%d, pt_index:%d\n", vaddr,pd_index,pt_index);
		kprintf("P%d::SEGMENTATION_FAULT\n", currpid);
		kill(currpid);
	}

	char* ptr = (char*) vaddr;
	if((vaddr > 96468900) && (vaddr < 96469000)){
		kprintf("vaddr value:%d\n", *ptr);
	}
	if(vaddr == 96468992){
		kprintf("*pres:%d write:%d valid:%d\n",pt_base[pt_index].pt_pres,pt_base[pt_index].pt_write,pt_base[pt_index].pt_valid);
	}
	//kprintf("9. **address:0x%x, pd_index:%d, pt_index:%d\n", vaddr,pd_index,pt_index);
	write_cr3((unsigned long) proctab[currpid].pdbr);
	restore(mask);
}

