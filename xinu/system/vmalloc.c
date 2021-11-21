#include <xinu.h>


char* vmalloc(uint32 nbytes){
	intmask mask;
	pd_t* pd_base = proctab[currpid].pdbr;
	pt_t* pt_base;
	char* retVal;
	int i,j;
	uint32 npages = (nbytes/4096);
	uint32	npages_count;
	uint32 pd_index = 8, pt_index = 0;
	int dir, tb, k;

	mask = disable();

	write_cr3((unsigned long)PT_START);

	if(nbytes == 0){
		write_cr3((unsigned long) proctab[currpid].pdbr);
		restore(mask);
		return (char *)SYSERR;
	}

	//nbytes = (uint32) roundmb(nbytes);
	//kprintf("proctab %x\n", proctab[currpid].pdbr);
	

	//kprintf("In vmalloc\n");
	
	if (nbytes > (npages*4096))
	{
		npages += 1;
	}
	npages_count = npages;
	proctab[currpid].pages_alloc += npages;
	
	
	for(i = 8; i < 1024; i++){
		if(pd_base[i].pd_pres == 0){
			npages_count -= 1024*1024;
		}
		else
		{
			
			pt_base = (pt_t*)(pd_base[i].pd_base << 12);
			//pt_base = ((uint32)(pt_base) << 12);
			for(j = 0; j < 1024; j++)
			{
				if((pt_base[j].pt_valid == 0) && (npages_count > 0))
				{
					npages_count--;
					if(npages_count == 0)	break;
				}
				else if((pt_base[j].pt_valid == 1) && (npages_count > 0)){
					npages_count = npages;
					
					//pt_index = j+1;
					if(j<1023){
						pt_index = j+1;
						pd_index = i;
					}	
					else if(j == 1023){
						pt_index = 0;
						pd_index = i+1;
					}
				}
				
			}
		}

		if(npages_count <= 0){
			break;
		}
	}
	
	//kprintf("pd_index:%d, pt_index:%d, npages:%d\n",pd_index,pt_index,npages);
	for(dir = pd_index; dir < 1024; dir++){
		
		//kprintf("dir:%d pd_pres:%d\n",dir,pd_base[dir].pd_pres);
		if((dir == pd_index) && (pd_base[dir].pd_pres == 1))
		{
			pt_base = (pt_t*)(pd_base[dir].pd_base << 12);
			//pt_base = ((uint32)(pt_base) << 12);
			for(k = pt_index; k < 1024; k++)
			{
				pt_base[k].pt_pres = 0;
				pt_base[k].pt_write = 1;
				pt_base[k].pt_valid = 1;
				npages--;
				if (npages == 0)	break;
			}
		}
		else
		{
			if (pd_base[dir].pd_pres == 0)
			{
				pd_base[dir].pd_pres = 1;
				pd_base[dir].pd_write = 1;
				pt_base = (pt_t*)(getptmem(4096));
				//kprintf("getptmem %x\n", &pt_base[0]);
				pd_base[dir].pd_base = ((uint32) &pt_base[0] >> 12);
				//kprintf("getptmem %x\n", &pt_base[0]);
			}
			else
			{
				pt_base = (pt_t*)(pd_base[dir].pd_base << 12);
				//pt_base = ((uint32)(pt_base) << 12);
			}
			//kprintf("pt_base:%x\n", (uint32)&pt_base[0]);
			
			for(tb=0; tb<1024 ;tb++)
			{
				//kprintf("*tb:%d pt_pres:%d pt_valid:%d npages:%d\n",tb,pt_base[tb].pt_pres,pt_base[tb].pt_valid,npages);
				if ((pt_base[tb].pt_valid == 0) && /*(pt_base[tb].pt_pres == 0) &&*/ (npages > 0))
				{
					pt_base[tb].pt_pres = 0;
					pt_base[tb].pt_write = 1;
					pt_base[tb].pt_valid = 1;
					npages -= 1;
					//kprintf("tb:%d npages:%d\n",tb,npages);
					
				}
				//kprintf("tb:%d pt_pres:%d pt_valid:%d\n",tb,pt_base[tb].pt_pres,pt_base[tb].pt_valid);
				if (npages == 0)
				{	
					break;
				}
				
			}
			
		}

		if (npages == 0)	break;

	}
	// kprintf("**pd_index:%d, pt_index:%d\n", pd_index,pt_index);
		
	// for(i = 0; i < 9; i++)
	// {
	// 	pt_base = (pd_base[pd_index].pd_base << 12);
	// 	pt_base = ((uint32)(pt_base) << 12);
	// 	kprintf("pt_base:%x %x\n",(uint32) &pd_base[i], (uint32)pd_base[i].pd_base);
	// }

	pt_base = (pt_t *) (pd_base[pd_index].pd_base);
	char* addr = (char*) ((pd_index << 22) | (pt_index << 12));
	//kprintf("return address:0x%x\n", addr);

	write_cr3((unsigned long) proctab[currpid].pdbr);
	restore(mask);
	return (char*) addr;
}
