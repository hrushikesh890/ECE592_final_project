#include<xinu.h>

syscall vfree(char*  ptr, uint32 nbytes){

    intmask mask;
    uint32 npages = (nbytes/4096);
	uint32 pd_index, pt_index;
	uint32 vaddr = (uint32)(ptr);

    pd_t* pd_base;
	pt_t* pt_base;
	
	pd_index = (vaddr >> 22) & (0x3FF);
	pt_index = (vaddr >> 12) & (0x3FF);

	pd_base = (pd_t*)(proctab[currpid].pdbr);
	pt_base = (pt_t*)(pd_base[pd_index].pd_base);
	//kprintf("vadrr = %x, pd_index = %x, pt_index = %x\n", vaddr, pd_index, pt_index);

    mask = disable();
    write_cr3((unsigned long)PT_START);
	
	if(nbytes == 0){
		write_cr3((unsigned long) proctab[currpid].pdbr);
		restore(mask);
		//kprintf("Reached here 1\n");
		return (char *)SYSERR;
	}

	//nbytes = (uint32) roundmb(nbytes);
	
	
	if (nbytes > (npages*4096))
	{
		npages += 1;
	}

	//kprintf("In vfree npages:%d\n",npages);
	int i,j,k;
	uint32 npages_count = npages;

	for(i = pd_index; i<1024; i++){

		if(pd_base[i].pd_pres == 0){
			write_cr3((unsigned long) proctab[currpid].pdbr);
			restore(mask);
			//kprintf("Reached here 2\n");
			return SYSERR;
		}
		else{
			if(i == pd_index){
				pt_base = (pt_t*)(pd_base[i].pd_base << 12);
				for(k = pt_index; k<1024; k++){
					if(pt_base[k].pt_valid == 0){
						write_cr3((unsigned long) proctab[currpid].pdbr);
						restore(mask);
						//kprintf("Reached here 3\n");
						return SYSERR;
					}
					else{
						npages_count--;
					}
					if(npages_count == 0) break;
				}
			}
			else{
				pt_base = (pt_t*)(pd_base[i].pd_base << 12);
				for(j=0;j<1024;j++){
					if(pt_base[j].pt_valid == 0){
						//kprintf("i: %d pt_base:%x pt_base[%d].valid:%d\n",i,pt_base,j,pt_base[j].pt_valid);
						write_cr3((unsigned long) proctab[currpid].pdbr);
						restore(mask);
						//kprintf("Reached here 4\n");
						return SYSERR;
					}
					else{
						npages_count--;
					}
					if(npages_count == 0)	break;
				}
			}
		}

		if(npages_count == 0)	break;
	}

	int dir,tb;

	npages_count = npages;
	for(dir = pd_index;dir<1024;dir++){
		if(dir == pd_index){
			pt_base = (pt_t*)(pd_base[i].pd_base << 12);
			for(tb = pt_index;tb<1024;tb++){
				pt_base[tb].pt_pres = 0;
				pt_base[tb].pt_write = 0;
				pt_base[tb].pt_valid = 0;
				npages--;
				proctab[currpid].pages_used--;
				// char* addr = (char*) ((dir << 22) | (tb << 12));
				// kprintf("vfree return address:0x%x, pd_index:%d, pt_index:%d\n", addr,dir,tb);
				if(npages == 0)	break;
			}
		}
		else{
			pt_base = (pt_t*)(pd_base[i].pd_base << 12);
			for(tb = 0; tb < 1024; tb++){
				pt_base[tb].pt_pres = 0;
				pt_base[tb].pt_write = 0;
				pt_base[tb].pt_valid = 0;
				npages--;
				proctab[currpid].pages_used--;
				// char* addr = (char*) ((dir << 22) | (tb << 12));
				// kprintf("vfree return address:0x%x, pd_index:%d, pt_index:%d\n", addr,dir,tb);
				if(npages == 0)	break;
			}
		}

		if(npages == 0)	break;
	}

	// kprintf("*********************************\n");
	// kprintf("alloc:%d npages:%d\n",proctab[currpid].pages_alloc,npages_count);
	proctab[currpid].pages_alloc -= npages_count;
	// kprintf("After alloc:%d npages:%d\n",proctab[currpid].pages_alloc,npages_count);
	write_cr3((unsigned long) proctab[currpid].pdbr);

	restore(mask);
	//kprintf("End of vfree\n");
	return OK;
}
