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
	//pt_base = (pt_t*)(pd_base[pd_index].pd_base);
	kprintf("vadrr = %x, pd_index = %x, pt_index = %x\n", vaddr, pd_index, pt_index);

    mask = disable();
    write_cr3((unsigned long)PT_START);

	if(nbytes == 0){
		write_cr3((unsigned long) proctab[currpid].pdbr);
		restore(mask);
		return (char *)SYSERR;
	}

	//nbytes = (uint32) roundmb(nbytes);
	
	
	if (nbytes > (npages*4096))
	{
		npages += 1;
	}

	kprintf("In vfree npages:%d\n",npages);
	int i,j,k;
	for(i = pd_index; i<1024; i++){
		kprintf("In for loop i:%d\n",i);
		if(pd_base[i].pd_pres == 0){
			kprintf("WHY IS THIS 0?\n");
			write_cr3((unsigned long) proctab[currpid].pdbr);
			restore(mask);
			return SYSERR;
		}

		if((i == pd_index) && (pd_base[i].pd_pres == 1)){
			pt_base = (pt_t*)(pd_base[i].pd_base << 12);
			for(k = pt_index; k<1024;k++){
				kprintf("pt_index +%d\n", k);
				if(pt_base[k].pt_valid == 0){
					kprintf("PT 0\n");
					write_cr3((unsigned long) proctab[currpid].pdbr);
					restore(mask);
					return SYSERR;
				}
				else{
					pt_base[k].pt_pres = 0;
					pt_base[k].pt_write = 0;
					pt_base[k].pt_valid = 0;
					npages--;
				}
				if (npages == 0)	break;
			}
		}
		else{

			if(pd_base[i].pd_pres == 0){
				write_cr3((unsigned long) proctab[currpid].pdbr);
				restore(mask);
				return SYSERR;
			}		

			else{
				pt_base = (pt_t*)pd_base[i].pd_base;

				for(j=0; j<1024 ;i++)
				{
					if(pt_base[j].pt_valid == 0){
						write_cr3((unsigned long) proctab[currpid].pdbr);
						restore(mask);
						return SYSERR;
					}
					else{
						pt_base[j].pt_pres = 0;
						pt_base[j].pt_write = 0;
						pt_base[j].pt_valid = 0;
						npages -= 1;
					}

					if (npages == 0)	break;
				}

			}

		}
		kprintf("In for loop npages:%d\n",npages);
		if (npages == 0)	break;

	}
	kprintf("*********************************\n");
	kprintf("alloc:%d npages:%d\n",proctab[currpid].pages_alloc,npages);
	proctab[currpid].pages_alloc -= npages;
	kprintf("After alloc:%d npages:%d\n",proctab[currpid].pages_alloc,npages);
	write_cr3((unsigned long) proctab[currpid].pdbr);
	// kprintf("alloc:%d npages:%d\n",proctab[currpid].pages_alloc,npages);
	// proctab[currpid].pages_alloc -= npages;
	// kprintf("After alloc:%d npages:%d\n",proctab[currpid].pages_alloc,npages);
	restore(mask);
	
	return OK;
}
